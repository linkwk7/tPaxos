#ifndef TPAXOS_IOLOOP_H
#define TPAXOS_IOLOOP_H

#include <thread>
#include <set>
#include <queue>
#include "common.h"
#include "util\error.h"
#include "util\timer.h"
#include "util\queue.h"
#include "util\message.pb.h"

namespace tPaxos {
	class InstanceIOLoopReceiver {
	public:
		virtual void IOLoopOnTimeout(const uint32_t uTimerID, const int iTimerType) = 0;
		virtual void IOLoopOnMessage(PaxosMsg * pMsg) = 0;
	};

	class InstanceIOLoop {
	public:

		InstanceIOLoop(InstanceIOLoopReceiver * pInstance);

		~InstanceIOLoop();

		void Run();
		void Stop();

		void AddMessage(PaxosMsg * pMsg, bool bBack = true);
		void AddTimer(const int iTimeout, const int iType, uint32_t & uTimerID);
		void RemoveTimer(const uint32_t uTimerID);
		void SetAcceptInstanceID(const uint32_t uAccept);
		void Clear();

	private:
		void ExecuteLogic();

		void DealWithTimeout(int & iNextTimeout);
		void DealWithMessage(const int iNextTimeout);


	private:
		bool _bEnd;
		std::thread _oThread;

		InstanceIOLoopReceiver * _pInstance;
		
		uint32_t _uAcceptInstanceID;
		Queue<PaxosMsg *> _oMessages;
		Timers _oTimers;
		std::set<uint32_t> _oActiveTimer;
	};

	// For GroupIOLoop's std::priority_queue
	class PaxosMsgPtrLess {
	public:
		bool operator() (const PaxosMsg * lhs, const PaxosMsg * rhs) {
			return lhs->instanceid() > rhs->instanceid();
		}
	};

	class GroupIOLoopReceiver {
	public:
		virtual bool IOLoopOnMessage(PaxosMsg * pMsg) = 0;
		virtual void IOLoopNewPropose() = 0;
	};

	class GroupIOLoop {
	public:

		GroupIOLoop(GroupIOLoopReceiver * pGroup);

		~GroupIOLoop();

		void Run();
		void Stop();

		void AddMessage(PaxosMsg * pMsg, bool bBack = true);
		void AddRetry(PaxosMsg * pMsg);

	private:
		void ExecuteLogic();

		void DealWithMessage(const int iNextTimeout);
		void DealWithRetry();

	private:
		bool _bEnd;
		std::thread _oThread;

		GroupIOLoopReceiver * _pGroup;

		Queue<PaxosMsg *> _oMessages;
		std::priority_queue<PaxosMsg *, std::deque<PaxosMsg*>, PaxosMsgPtrLess> _oRetry;
	};

	class SyncerIOLoopReceiver {
	public:
		virtual void IOLoopOnTimeout() = 0;
		virtual void IOLoopOnTask(const SyncerTask & task) = 0;
	};

	class SyncerIOLoop {
	public:

		static const int PROBETIMEOUT = 500;

		SyncerIOLoop(SyncerIOLoopReceiver * pSyncer);

		~SyncerIOLoop();

		void Run();
		void Stop();

		void AddTask(const SyncerTask & task);

	private:

		void ExecuteLogic();

		void DealWithTimeout(int & iNextTimeout);
		void DealWithTask(const int iNextTimeout);

	private:
		bool _bEnd;
		std::thread _oThread;

		SyncerIOLoopReceiver * _pSyncer;

		uint64_t _uNextProbeAbsTimeout;
		Queue<SyncerTask> _oTasks;
	};
}

#endif
