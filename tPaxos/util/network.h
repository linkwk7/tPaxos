#ifndef TPAXOS_NETWORK_H
#define TPAXOS_NETWORK_H

#include <stdint.h>
#include <string>
#include <map>
#include <set>
#include <shared_mutex>
#include <thread>
#include <vector>
#include "error.h"
#include "queue.h"
#include "util.h"

namespace tPaxos {
	class NodeInfo {
	public:
		NodeInfo() = default;
		NodeInfo(const uint32_t uID, const std::string & ip, const int pt) : uNodeID(uID), sIP(ip), iPort(pt) {}

		uint32_t uNodeID;
		std::string sIP;
		int iPort;
	};

	class NetworkClient {
	public:
		class NetworkMsg {
		public:
			NetworkMsg() = default;
			NetworkMsg(const uint32_t uSID, const uint32_t uRID, const std::string & msg) : \
				uSenderID(uSID), uReceiverID(uRID), sMsg(msg) {}

			uint32_t uSenderID;
			uint32_t uReceiverID;
			std::string sMsg;
		};

		virtual ~NetworkClient() {};

		virtual Error Init(const std::map<uint32_t, NodeInfo> & oInfoMap) = 0;
		virtual Error Run() = 0;
		virtual Error Stop() = 0;

		virtual Error Send(const uint32_t uNodeID, const std::string & msg) = 0;
		virtual void Broadcast(bool bToSelf, const std::string & msg) = 0;
		virtual void OnReceive(const std::string & msg) = 0;

	private:
	};

	class NetworkReceiver {
	public:
		virtual void OnReceiveNewMessage(const std::string & msg) {}
	};

	class QueueTransfer;

	class QueueNetwork : public NetworkClient {
	public:
		QueueNetwork(const uint32_t uID, QueueTransfer * pTran, NetworkReceiver * pRecv) : \
			_uCurID(uID), _pTransfer(pTran), _pReceiver(pRecv) {}
		
		Error Init(const std::map<uint32_t, NodeInfo> & oInfoMap) override;
		Error Run() override { return Error(); }
		Error Stop() override { return Error(); }

		Error Send(const uint32_t uNodeID, const std::string & msg) override;
		void Broadcast(bool bToSelf, const std::string & msg) override;
		void OnReceive(const std::string & msg) override;

	private:
		uint32_t _uCurID;
		std::set<uint32_t> _oIDSet;
		QueueTransfer * _pTransfer;
		NetworkReceiver * _pReceiver;
	};

	class QueueTransfer {
	public:
		QueueTransfer(int n) : _iNodeNum(n), _bIOEnd(true), _iDropRate(0), _iReplicateRate(0) {}
		~QueueTransfer();

		void Init(const std::map<uint32_t, NetworkClient*> & infoMap);
		void Run();
		void Stop();

		void Add(const uint32_t uSID, const uint32_t uRID, const std::string & msg);
		
		void SetDropRate(int rate);
		void SetReplicateRate(int rate);
		void Connect(const uint32_t uNodeID);
		void Disconnect(const uint32_t uNodeID);
		
	private:
		void ExecuteLogic();

	private:
		int _iNodeNum;
		// Drop rate range from 0 to 100, 100 means drop every package
		int _iDropRate;
		int _iReplicateRate;
		std::shared_mutex _oLock;
		std::map<uint32_t, bool> _oConnected;

		std::map<uint32_t, NetworkClient*> _oNetworkMap;
		Queue<NetworkClient::NetworkMsg> _oMsgQueue;
		bool _bIOEnd;
		std::thread _oIOThread;
	};
}

#endif