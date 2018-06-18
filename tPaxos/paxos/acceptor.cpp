#include "acceptor.h"

namespace tPaxos {
	Acceptor::Acceptor(Config * pConfig, State * pState, NetworkClient * pNetwork) : _oPromiseBallot(0,0), \
		_oAcceptBallot(0, 0), _sValue(), _pConfig(pConfig), _pState(pState), _pNetwork(pNetwork) {}

	void Acceptor::OnPrepare(const PaxosMsg * pMsg) {
		PaxosMsg reply;
		
		reply.set_gltype(GroupMsgPaxos);
		reply.set_instanceid(_pConfig->uInstanceID);
		reply.set_nodeid(_pState->uNodeID);
		reply.set_iltype(PaxosMsgTypePrepareReply);
		reply.set_proposalid(pMsg->proposalid());
		
		BallotSequence msgBallot(pMsg->proposalid(), pMsg->nodeid());
		if (msgBallot < _oPromiseBallot) {
			reply.set_reject(true);
			reply.set_promisedproposalid(_oPromiseBallot.uProposalID);
		}
		else {
			_oPromiseBallot = msgBallot;
			
			reply.set_reject(false);
			reply.set_acceptedproposalid(_oAcceptBallot.uProposalID);
			reply.set_acceptednodeid(_oAcceptBallot.uNodeID);
			reply.set_value(_sValue);
		}

		_pNetwork->Send(pMsg->nodeid(), reply.SerializeAsString());
	}

	void Acceptor::OnAccept(const PaxosMsg * pMsg) {
		PaxosMsg reply;

		reply.set_gltype(GroupMsgPaxos);
		reply.set_instanceid(_pConfig->uInstanceID);
		reply.set_nodeid(_pState->uNodeID);
		reply.set_iltype(PaxosMsgTypeAcceptReply);
		reply.set_proposalid(pMsg->proposalid());

		BallotSequence msgBallot(pMsg->proposalid(), pMsg->nodeid());
		if (msgBallot < _oPromiseBallot) {
			reply.set_reject(true);
			reply.set_promisedproposalid(_oPromiseBallot.uProposalID);
		}
		else {
			_oPromiseBallot = msgBallot;
			_oAcceptBallot = msgBallot;
			_sValue = pMsg->value();

			reply.set_reject(false);
		}

		_pNetwork->Send(pMsg->nodeid(), reply.SerializeAsString());
	}

	void Acceptor::Reset() {
		_oPromiseBallot.Reset();
		_oAcceptBallot.Reset();
		_sValue = std::string();
	}

	std::string Acceptor::AcceptorInfo() const {
		return ToString("A:Node:", _pState->uNodeID, ",Ins:", _pConfig->uInstanceID, ",MaxOther:",_pState->uMaxOtherProposalID,"}");
	}
}