#include "ioloop.h"

namespace tPaxos {
	InstanceIOLoop::InstanceIOLoop(InstanceIOLoopReceiver * pInstance) : _bEnd(true), _pInstance(pInstance), _uAcceptInstanceID(0) {}

	InstanceIOLoop::~InstanceIOLoop() {
		Stop();
	}

	void InstanceIOLoop::Run() {
		if (_bEnd) {
			_bEnd = false;
			_oThread = std::thread(std::bind(&InstanceIOLoop::ExecuteLogic, this));
		}
	}

	void InstanceIOLoop::Stop() {
		if (!_bEnd) {
			_bEnd = true;
			_oThread.join();
		}
	}

	void InstanceIOLoop::AddMessage(PaxosMsg * pMsg, bool bBack) {
		if (_bEnd) {
			return;
		}

		_oMessages.lock();

		// Condition must put in lock to prevent race with SetAcceptInstanceID, 
		// in fact, here is using message queue's lock to protect uAcceptInstanceID
		if (pMsg->instanceid() == _uAcceptInstanceID) {
			_oMessages.add(pMsg, bBack);
		}

		_oMessages.unlock();
	}

	void InstanceIOLoop::AddTimer(const int iTimeout, const int iType, uint32_t & uTimerID) {
		if (_bEnd) {
			return;
		}

		_oTimers.AddTimer(Timers::GetSteadyClockMS()+iTimeout, iType, uTimerID);
		_oActiveTimer.insert(uTimerID);
	}

	void InstanceIOLoop::RemoveTimer(const uint32_t uTimerID) {
		if (_bEnd) {
			return;
		}

		_oActiveTimer.erase(uTimerID);
	}

	void InstanceIOLoop::SetAcceptInstanceID(const uint32_t uAccept) {
		if (_bEnd) {
			return;
		}

		// Here need to lock _oMessages's lock to avoid potential race condition 
		_oMessages.lock();

		_uAcceptInstanceID = uAccept;

		_oMessages.unlock();
	}

	// Clear will clear all container, but acceptinstanceid should set 0 before call this function
	void InstanceIOLoop::Clear() {
		if (_bEnd) {
			return;
		}

		// TODO : Add clear to message queue
		_oMessages.lock();

		for (; !_oMessages.empty();) {
			_oMessages.pop();
		}

		_oMessages.unlock();

		_oTimers.Clear();

		_oActiveTimer = std::set<uint32_t>();
	}

	void InstanceIOLoop::ExecuteLogic() {
		for (; true;) {
			if (_bEnd) {
				break;
			}

			int nextTimeout = 1000;

			// Deal with timeout
			DealWithTimeout(nextTimeout);

			// Deal with message
			DealWithMessage(nextTimeout);

		}
	}

	void InstanceIOLoop::DealWithTimeout(int & iNextTimeout) {
		for (; true;) {
			int nto = _oTimers.GetNextTimeout();

			// Timer heap is empty
			if (nto < 0) {
				iNextTimeout = 1000;
				return;
			}
			else if (nto > 0) {
				iNextTimeout = nto;
				return;
			}
			// Some timer already fire
			else {
				uint32_t timerID = 0;
				int timerType = 0;

				if (_oTimers.PopTimeout(timerID, timerType)) {
					auto timerIt = _oActiveTimer.find(timerID);
					// This is an active timer, instance may disable some timer in DealWithMessage
					if (timerIt != _oActiveTimer.end()) {
						// In _oInstance->IOLoopOnTimeout, intsance.proposer will restart prepare, this
						// will remove last prepare timer
						_oActiveTimer.erase(timerIt);

						_pInstance->IOLoopOnTimeout(timerID, timerType);
					}
				}
				else {
					assert(0);
				}
			}
		}
	}

	void InstanceIOLoop::DealWithMessage(const int iNextTimeout) {
		PaxosMsg * pMsg;
		_oMessages.lock();

		bool newMsg = _oMessages.peek(pMsg, iNextTimeout);

		if (!newMsg) {
			_oMessages.unlock();
		}
		else {
			_oMessages.pop();
			_oMessages.unlock();

			_pInstance->IOLoopOnMessage(pMsg);
		}
	}

	GroupIOLoop::GroupIOLoop(GroupIOLoopReceiver * pGroup) : _bEnd(true), _pGroup(pGroup) {}

	GroupIOLoop::~GroupIOLoop() {
		Stop();
	}

	void GroupIOLoop::Run() {
		if (_bEnd) {
			_bEnd = false;
			_oThread = std::thread(std::bind(&GroupIOLoop::ExecuteLogic, this));
		}
	}

	void GroupIOLoop::Stop() {
		if (!_bEnd) {
			_bEnd = true;
			_oThread.join();
		}
	}

	void GroupIOLoop::AddMessage(PaxosMsg * pMsg, bool bBack) {
		if (_bEnd) {
			return;
		}

		_oMessages.lock();

		_oMessages.add(pMsg, bBack);

		_oMessages.unlock();
	}

	void GroupIOLoop::AddRetry(PaxosMsg * pMsg) {
		if (_bEnd) {
			return;
		}

		_oRetry.push(pMsg);
	}

	void GroupIOLoop::ExecuteLogic() {
		for (; true;) {
			if (_bEnd) {
				break;
			}

			DealWithMessage(1000);

			DealWithRetry();

			_pGroup->IOLoopNewPropose();
		}
	}

	void GroupIOLoop::DealWithMessage(const int iNextTimeout) {
		PaxosMsg * pMsg = nullptr;
		_oMessages.lock();

		bool newMsg = _oMessages.peek(pMsg, iNextTimeout);
		if (!newMsg) {
			_oMessages.unlock();
		}
		else {
			_oMessages.pop();
			_oMessages.unlock();

			// Let group handle this message, if group find a proper instance to process
			// this message, otherwise group will add it to retry queue
			_pGroup->IOLoopOnMessage(pMsg);
		}
	}

	void GroupIOLoop::DealWithRetry() {
		for (; !_oRetry.empty(); ) {
			if (_pGroup->IOLoopOnMessage(_oRetry.top())) {
				_oRetry.pop();
			}
			else {
				_oRetry.pop();
				break;
			}
		}
	}

	SyncerIOLoop::SyncerIOLoop(SyncerIOLoopReceiver * pSyncer) : _bEnd(true), _pSyncer(pSyncer) {}

	SyncerIOLoop::~SyncerIOLoop() {
		Stop();
	}

	void SyncerIOLoop::Run() {
		if (_bEnd) {
			_bEnd = false;
			_oThread = std::thread(std::bind(&SyncerIOLoop::ExecuteLogic, this));

			_uNextProbeAbsTimeout = Timers::GetSteadyClockMS() + PROBETIMEOUT;
		}
	}

	void SyncerIOLoop::Stop() {
		if (!_bEnd) {
			_bEnd = true;
			_oThread.join();
		}
	}

	void SyncerIOLoop::AddTask(const SyncerTask & task) {
		if (_bEnd) {
			return;
		}

		_oTasks.lock();

		_oTasks.add(task);

		_oTasks.unlock();
	}

	void SyncerIOLoop::ExecuteLogic() {
		for (; true; ) {
			if (_bEnd) {
				break;
			}

			int nextTimeout = 1000;

			DealWithTimeout(nextTimeout);

			DealWithTask(nextTimeout);
		}
	}

	void SyncerIOLoop::DealWithTimeout(int & iNextTimeout) {
		uint64_t now = Timers::GetSteadyClockMS();
		if (now >= _uNextProbeAbsTimeout) {
			_pSyncer->IOLoopOnTimeout();

			iNextTimeout = PROBETIMEOUT;
			_uNextProbeAbsTimeout = Timers::GetSteadyClockMS() + static_cast<uint64_t>(PROBETIMEOUT);
		}
		else {
			iNextTimeout = static_cast<int>(_uNextProbeAbsTimeout - now);
		}
	}

	void SyncerIOLoop::DealWithTask(const int iNextTimeout) {
		SyncerTask task;
		_oTasks.lock();
		
		bool newTask = _oTasks.peek(task, iNextTimeout);
		if (!newTask) {
			_oTasks.unlock();
		}
		else {
			_oTasks.pop();
			_oTasks.unlock();

			_pSyncer->IOLoopOnTask(task);
		}
	}
}