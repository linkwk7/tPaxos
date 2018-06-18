#ifndef TPAXOS_PROPOSER_H
#define TPAXOS_PROPOSER_H

#include "common.h"
#include "ioloop.h"
#include "util/counter.h"
#include "util/network.h"
#include "util/util.h"
#include "util/message.pb.h"

namespace tPaxos {
	class Learner;

	class Proposer {
	public:
		Proposer(InstanceIOLoop * pIOLoop, Learner * pLearner, Config * pConfig, State * pState, NetworkClient * pNetwork);

		void NewProposal(const std::string & sValue);

		void OnPrepareReply(const PaxosMsg * pMsg);
		void OnAcceptReply(const PaxosMsg * pMsg);

		void OnPrepareTimeout();
		void OnAcceptTimeout();

		void Reset();

	private:
		void Prepare();
		void Accept();

	private:
		void ExitPrepare();
		void ExitAccept();

		void AddPrepareTimer();
		void AddAcceptTimer();

		std::string ProposerInfo() const;

	private:
		// Record proposer's last used proposal id
		uint32_t _uLastProposalID;
		// _oAcceptedBallot is use to record currently known highest accepted value
		// from other node, initial to {0,0}
		BallotSequence _oAcceptedBallot;
		// Value is either learn from other acceptor or from proposal value
		std::string _sValue;

		Counter _oCounter;
		bool _bPreparing;
		bool _bAccepting;
		uint32_t _uPrepareTimerID;
		uint32_t _uAcceptTimerID;
		int _iTimeout;

		InstanceIOLoop * _pIOLoop;
		Learner * _pLearner;

		Config * _pConfig;
		State * _pState;

		NetworkClient * _pNetwork;
	};
}

#endif
