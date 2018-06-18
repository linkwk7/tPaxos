#include "network.h"

namespace tPaxos {
	////////////////////////////////QueueNetwork////////////////////////////////

	Error QueueNetwork::Init(const std::map<uint32_t, NodeInfo> & oInfoMap) {
		for (auto & elem : oInfoMap) {
			_oIDSet.insert(elem.first);
		}
		return Error();
	}

	Error QueueNetwork::Send(const uint32_t uNodeID, const std::string & msg) {
		_pTransfer->Add(_uCurID, uNodeID, msg);

		return Error();
	}

	void QueueNetwork::Broadcast(bool bToSelf, const std::string & msg) {
		for (uint32_t id : _oIDSet) {
			if (bToSelf || id != _uCurID) {
				Send(id, msg);
			}
		}
	}

	void QueueNetwork::OnReceive(const std::string & msg) {
		_pReceiver->OnReceiveNewMessage(msg);
	}

	////////////////////////////////QueueTransfer////////////////////////////////

	QueueTransfer::~QueueTransfer() {
		Stop();
	}

	void QueueTransfer::Init(const std::map<uint32_t, NetworkClient*> & infoMap) {
		for (auto & elem : infoMap) {
			_oNetworkMap.insert(elem);
			_oConnected[elem.first] = true;
		}
	}

	void QueueTransfer::Run() {
		if (_bIOEnd) {
			_bIOEnd = false;

			_oIOThread = std::thread(std::bind(&QueueTransfer::ExecuteLogic, this));
		}
	}

	void QueueTransfer::ExecuteLogic() {
		while (!_bIOEnd) {
			NetworkClient::NetworkMsg msg;

			_oMsgQueue.lock();

			bool newMsg = _oMsgQueue.peek(msg, 1000);
			if (newMsg) {
				_oMsgQueue.pop();
			}

			_oMsgQueue.unlock();

			if (newMsg) {
				auto iter = _oNetworkMap.find(msg.uReceiverID);
				if (iter != _oNetworkMap.end()) {
					if (XorShift() % 100 < _iReplicateRate) {
						Add(msg.uSenderID, msg.uReceiverID, msg.sMsg);
					}

					if (XorShift() % 100 < _iDropRate) {
						continue;
					}

					iter->second->OnReceive(msg.sMsg);
				}
			}
		}
	}

	void QueueTransfer::Stop() {
		if (!_bIOEnd) {
			_bIOEnd = true;
			_oIOThread.join();
		}
	}

	void QueueTransfer::Add(const uint32_t uSID, const uint32_t uRID, const std::string & msg) {
		if (!_bIOEnd) {
			_oLock.lock_shared();
			if (!_oConnected[uSID] || !_oConnected[uRID]) {
				_oLock.unlock_shared();
				return;
			}
			_oLock.unlock_shared();

			_oMsgQueue.lock();

			_oMsgQueue.add(NetworkClient::NetworkMsg(uSID, uRID, msg));

			_oMsgQueue.unlock();
		}
	}

	void QueueTransfer::SetDropRate(int rate) {
		_iDropRate = rate;
	}

	void QueueTransfer::SetReplicateRate(int rate) {
		_iReplicateRate = rate;
	}

	void QueueTransfer::Connect(const uint32_t uNodeID) {
		_oLock.lock();
		_oConnected[uNodeID] = true;
		_oLock.unlock();
	}

	void QueueTransfer::Disconnect(const uint32_t uNodeID) {
		_oLock.lock();
		_oConnected[uNodeID] = false;
		_oLock.unlock();
	}
}