#ifndef TPAXOS_INSTANCE_H
#define TPAXOS_INSTANCE_H

#include "common.h"
#include "context.h"
#include "ioloop.h"
#include "proposer.h"
#include "acceptor.h"
#include "learner.h"
#include "util/network.h"
#include "util/util.h"

namespace tPaxos {
	class GroupReceiver {
	public:
		virtual void InstanceDoneConsensus(const uint32_t uInstanceID, const std::string & sValue) = 0;
	};

	class Instance : public InstanceIOLoopReceiver {
	public:

		Instance(GroupReceiver * pGroup, State * pState, NetworkClient * pNetwork);

		~Instance();

		void InitForNewInstance(const uint32_t uInstanceID);

		// This function will make a copy of incoming message and put its pointer 
		// into ioloop
		void OnReceiveNewMessage(const PaxosMsg & oMsg);

		void NewPropose(CommitCtx * pCtx);

		void IOLoopOnMessage(PaxosMsg * pMsg) override;
		void IOLoopOnTimeout(const uint32_t uTimerID, const int iTimerType) override;

	private:

		void Reset();

		std::string InstanceInfo() const;

	private:
		CommitCtx * _pCommitCtx;

		InstanceIOLoop _oIOLoop;

		GroupReceiver * _pGroup;

		Config _oConfig;
		State * _pState;
		NetworkClient * _pNetwork;

		Proposer _oProposer;
		Acceptor _oAcceptor;
		Learner _oLearner;
	};
}

#endif
