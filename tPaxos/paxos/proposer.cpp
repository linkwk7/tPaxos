#include "proposer.h"
#include "learner.h"

namespace tPaxos {
	Proposer::Proposer(InstanceIOLoop * pIOLoop, Learner * pLearner, Config * pConfig, State * pState, NetworkClient * pNetwork) : \
		_uLastProposalID(0), _oAcceptedBallot(0,0), _bPreparing(false), _bAccepting(false), \
		_uPrepareTimerID(0), _uAcceptTimerID(0), _iTimeout(60), _pIOLoop(pIOLoop), _pLearner(pLearner), \
		_pConfig(pConfig), _pState(pState), _pNetwork(pNetwork) {}

	// Instance should reset proposer before call this function
	void Proposer::NewProposal(const std::string & sValue) {
		ExitPrepare();
		ExitAccept();

		_sValue = sValue;

		_pState->oRWLock.lock_shared();
		uint32_t uLastAccept = _pState->uLastAcceptInstanceID;
		uint32_t uLastReject = _pState->uLastRejectInstanceID;
		_pState->oRWLock.unlock_shared();

		// Last instance commit success, this node become leader
		// But this is not a great solution, it may cause performance degeneration
		// Therefor this better work with a seperate master mechanism
		if (uLastAccept > uLastReject && uLastReject < _pConfig->uInstanceID) {
			Dprintf(InstanceLevel, "%s[NewProposal.Accept]:%s @ %d\n", ProposerInfo().c_str(), sValue.c_str(), _pConfig->uInstanceID);

			Accept();
		}
		else {
			Dprintf(InstanceLevel, "%s[NewProposal.Prepare]:%s @ %d\n", ProposerInfo().c_str(), sValue.c_str(), _pConfig->uInstanceID);

			Prepare();
		}
	}

	void Proposer::OnPrepareReply(const PaxosMsg * pMsg) {
		if (!_bPreparing) {
			return;
		}

		if (pMsg->proposalid() != _uLastProposalID) {
			// This must be an old message
			return;
		}

		if (pMsg->reject()) {
			_pState->oRWLock.lock_shared();

			if (pMsg->promisedproposalid() > _pState->uMaxOtherProposalID) {
				_pState->uMaxOtherProposalID = pMsg->promisedproposalid();
			}

			_pState->oRWLock.unlock_shared();

			_oCounter.Reject(pMsg->nodeid());

			Dprintf(InstanceLevel, "%s[PrepareReply.Reject]:Node:%u,ProposalID:%d,Promised:%d\n", ProposerInfo().c_str(), pMsg->nodeid(), pMsg->proposalid(), pMsg->promisedproposalid());
		}
		else {
			BallotSequence msgBallot(pMsg->acceptedproposalid(), pMsg->acceptednodeid());
			if (_oAcceptedBallot < msgBallot) {
				_oAcceptedBallot = msgBallot;
				_sValue = pMsg->value();
			}

			_oCounter.Accept(pMsg->nodeid());

			Dprintf(InstanceLevel, "%s[PrepareReply.Accept]:Node:%u,ProposalID:%d,AcceptP:%d,AcceptN:%d\n", ProposerInfo().c_str(), pMsg->nodeid(), pMsg->proposalid(), pMsg->acceptedproposalid(),\
				pMsg->acceptednodeid());
		}

		if (_oCounter.AcceptReach(_pState->uMajority)) {
			Dprintf(InstanceLevel, "%s[AcceptPhase]\n", ProposerInfo().c_str());

			Accept();
		}
		else if (_oCounter.RejectReach(_pState->uMajority) || _oCounter.ReceiveReach(_pState->uAll)) {
			// Reset prepare timer
			Dprintf(InstanceLevel, "%s[Retry]\n", ProposerInfo().c_str());
		}
	}

	void Proposer::OnAcceptReply(const PaxosMsg * pMsg) {
		if (!_bAccepting) {
			return;
		}

		if (pMsg->proposalid() != _uLastProposalID) {
			return;
		}

		if (pMsg->reject()) {
			_pState->oRWLock.lock_shared();

			if (pMsg->promisedproposalid() > _pState->uMaxOtherProposalID) {
				_pState->uMaxOtherProposalID = pMsg->promisedproposalid();
			}

			_pState->oRWLock.unlock_shared();

			_oCounter.Reject(pMsg->nodeid());

			Dprintf(InstanceLevel, "%s[AcceptReply.Reject]:Node:%u,ProposalID:%d,Promised:%d\n", ProposerInfo().c_str(), pMsg->nodeid(), pMsg->proposalid(), pMsg->promisedproposalid());
		}
		else {
			_oCounter.Accept(pMsg->nodeid());

			Dprintf(InstanceLevel, "%s[AcceptReply.Accept]:Node:%u,ProposalID:%d\n", ProposerInfo().c_str(), pMsg->nodeid(), pMsg->promisedproposalid());
		}

		if (_oCounter.AcceptReach(_pState->uMajority)) {
			ExitAccept();
			
			// Send notification to learner
			_pLearner->LearnFromProposer(_sValue);

			// Send notification to other node's learner
			PaxosMsg msg;
			msg.set_gltype(GroupMsgPaxos);
			msg.set_instanceid(_pConfig->uInstanceID);
			msg.set_nodeid(_pState->uNodeID);
			msg.set_iltype(PaxosMsgTypeLearnValue);
			msg.set_value(_sValue);

			_pNetwork->Broadcast(false, msg.SerializeAsString());

			Dprintf(InstanceLevel, "%s[NotifyLearner]:%s\n", ProposerInfo().c_str(), _sValue.c_str());
		}
		else if (_oCounter.RejectReach(_pState->uMajority) || _oCounter.ReceiveReach(_pState->uAll)) {
			// Reset timer, resart prepare
		}
	}

	void Proposer::OnPrepareTimeout() {
		Dprintf(InstanceLevel, "%s[PrepareTimeout]\n", ProposerInfo().c_str());

		Prepare();
	}

	void Proposer::OnAcceptTimeout() {
		Dprintf(InstanceLevel, "%s[AcceptTImeout]\n", ProposerInfo().c_str());

		// Restart prepare rather than restart accept, cause highest other proposal id may change
		Prepare();
	}

	// This function will called only when new proposal incoming, prepare timer timeout, accept timer timeout 
	// or didn't receive enough reply
	void Proposer::Prepare() {
		ExitAccept();
		_bPreparing = true;

		_oCounter.Reset();

		AddPrepareTimer();

		_pState->oRWLock.lock_shared();
		_uLastProposalID = _pState->uMaxOtherProposalID + 1;
		_pState->oRWLock.unlock_shared();

		PaxosMsg msg;
		msg.set_gltype(GroupMsgPaxos);
		msg.set_instanceid(_pConfig->uInstanceID);
		msg.set_nodeid(_pState->uNodeID);
		msg.set_iltype(PaxosMsgTypePrepare);
		msg.set_proposalid(_uLastProposalID);

		_pNetwork->Broadcast(true, msg.SerializeAsString());

		Dprintf(InstanceLevel, "%s[Prepare]\n", ProposerInfo().c_str());
	}

	// This function will called only proposer receive enough reply or this instance currently
	// is leader
	void Proposer::Accept() {
		ExitPrepare();
		_bAccepting = true;

		_oCounter.Reset();

		AddAcceptTimer();

		PaxosMsg msg;
		msg.set_gltype(GroupMsgPaxos);
		msg.set_instanceid(_pConfig->uInstanceID);
		msg.set_nodeid(_pState->uNodeID);
		msg.set_iltype(PaxosMsgTypeAccept);
		msg.set_proposalid(_uLastProposalID);
		msg.set_value(_sValue);

		_pNetwork->Broadcast(true, msg.SerializeAsString());

		Dprintf(InstanceLevel, "%s[Accept]\n", ProposerInfo().c_str());
	}

	void Proposer::Reset() {
		_uLastProposalID = 0;
		_oAcceptedBallot.Reset();
		_sValue = std::string();
		_oCounter.Reset();
		
		ExitPrepare();
		ExitAccept();
	}

	void Proposer::ExitPrepare() {
		if (_bPreparing) {
			Dprintf(InstanceLevel, "%s[ExitPrepare]\n", ProposerInfo().c_str());

			_bPreparing = false;
			if (_uPrepareTimerID != 0) {
				_pIOLoop->RemoveTimer(_uPrepareTimerID);
			}
			_uPrepareTimerID = 0;
		}
	}

	void Proposer::ExitAccept() {
		if (_bAccepting) {
			Dprintf(InstanceLevel, "%s[ExitAccept]\n", ProposerInfo().c_str());

			_bAccepting = false;
			if (_uAcceptTimerID != 0) {
				_pIOLoop->RemoveTimer(_uAcceptTimerID);
			}
			_uAcceptTimerID = 0;
		}
	}

	void Proposer::AddPrepareTimer() {
		if (_uPrepareTimerID != 0) {
			_pIOLoop->RemoveTimer(_uPrepareTimerID);
		}
		_pIOLoop->AddTimer(_iTimeout + XorShift()%20, PaxosTimerPrepare, _uPrepareTimerID);
		_iTimeout = std::min(_iTimeout * 2, 2048);
	}

	void Proposer::AddAcceptTimer() {
		if (_uAcceptTimerID != 0) {
			_pIOLoop->RemoveTimer(_uAcceptTimerID);
		}
		_pIOLoop->AddTimer(_iTimeout + XorShift() % 20, PaxosTimerAccept, _uAcceptTimerID);
		_iTimeout = std::min(_iTimeout * 2, 2048);
	}

	std::string Proposer::ProposerInfo() const {
		return ToString("{P:Node:", _pState->uNodeID, ",Ins:", _pConfig->uInstanceID, ",MaxOther:", _pState->uMaxOtherProposalID, \
			",LastPro:", _uLastProposalID, ",P:", _bPreparing, ",A:", _bAccepting, ",PT:", _uPrepareTimerID, ",AT:", _uAcceptTimerID, \
			",Value:", _sValue, "}");
	}
}