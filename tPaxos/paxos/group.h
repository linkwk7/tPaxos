#ifndef TPAXOS_GROUP_H
#define TPAXOS_GROUP_H

#include <stdint.h>
#include <vector>
#include <array>
#include <map>
#include <mutex>
#include "common.h"
#include "context.h"
#include "instance.h"
#include "ioloop.h"
#include "log.h"
#include "syncer.h"
#include "util/message.pb.h"
#include "util/network.h"

namespace tPaxos {
	class Group : public GroupIOLoopReceiver, public GroupReceiver, public NetworkReceiver {
	public:

		friend class Syncer;

		static const int MAXINSTANCENUM = 100;

		Group(const uint32_t uID);

		~Group();

		void Init(NetworkClient * pNetwork);

		// Using proposer's thread -> _oCommitCtxs
		void NewPropose(const std::string & pValue);

		// Using network's thread -> _oIOLoop
		void OnReceiveNewMessage(const std::string & sMsg);

		// Using instance's thread -> _oIOLoop
		void InstanceDoneConsensus(const uint32_t uInstanceID, const std::string & sValue) override;

		bool IOLoopOnMessage(PaxosMsg * pMsg) override;
		void IOLoopNewPropose() override;

	private:

		bool OnPaxosMessage(PaxosMsg * pMsg);
		bool OnInternalMessage(PaxosMsg * pMsg);
		bool OnProbeMessage(PaxosMsg * pMsg);
		bool OnProbeReplyMessage(PaxosMsg * pMsg);
		bool OnLookupMessage(PaxosMsg * pMsg);

		bool AllocateNewInstance(const uint32_t uInstanceID, int & iIndex);

		void UpdateMinMaxInstanceID(const uint32_t uDone);

		std::string GroupInfo() const;

	private:
		std::mutex _oLock;
		std::deque<CommitCtx*> _oCommitCtxs;

		State _oState;
		GroupIOLoop _oIOLoop;

		uint32_t _uMinInstanceID;
		uint32_t _uMaxInstanceID;
		std::vector<Instance*> _oInstances;
		std::vector<int> _oIdleInstance;
		std::array<bool, MAXINSTANCENUM> _oDoneInstanceID;
		std::map<uint32_t, int> _oInstanceMap;

		Log _oLog;

		Syncer _oSyncer;
	
		NetworkClient * _pNetwork;
	};
}

#endif
