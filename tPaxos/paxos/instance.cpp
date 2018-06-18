#include "instance.h"

namespace tPaxos {
	Instance::Instance(GroupReceiver * pGroup, State * pState, NetworkClient * pNetwork) : _pCommitCtx(nullptr), \
		_oIOLoop(this), _pGroup(pGroup), _pState(pState), _pNetwork(pNetwork),\
		_oProposer(&_oIOLoop, &_oLearner, &_oConfig, pState, pNetwork), \
		_oAcceptor(&_oConfig, pState, pNetwork) {
		_oIOLoop.Run();
	}

	Instance::~Instance() {
		_oIOLoop.Stop();
	}

	// Let this instance prepare for uInstanceID, this instance should be new or called Reset() before
	// call this function
	void Instance::InitForNewInstance(const uint32_t uInstanceID) {
		if (_oConfig.uInstanceID != 0) {
			Dprintf(InstanceLevel, "%s[AssertFail]:%d\n", InstanceInfo().c_str(), _oConfig.uInstanceID);
			assert(0);
		}

		_oIOLoop.SetAcceptInstanceID(uInstanceID);
		_oConfig.uInstanceID = uInstanceID;
	
		Dprintf(InstanceLevel, "%s[InitforNewInstance]:%d\n", InstanceInfo().c_str(), uInstanceID);
	}

	// OnReceiveNewMessage is the interface provide by instance, group will call this function when 
	// message for this instance is coming. Before call this function, this instance must initialize 
	// for that instance id
	void Instance::OnReceiveNewMessage(const PaxosMsg & oMsg) {
		_oIOLoop.AddMessage(new PaxosMsg(oMsg));

		Dprintf(InstanceLevel, "%s[ReceiveMsg]:%s\n", InstanceInfo().c_str(), ToString(oMsg).c_str());
	}

	// NewPropose must call after InitForNewInstance & Reset
	void Instance::NewPropose(CommitCtx * pCtx) {
		if (_pCommitCtx != nullptr) {
			Dprintf(InstanceLevel, "%s[AssertFail]\n", InstanceInfo().c_str());
			assert(0);
		}

		_pCommitCtx = pCtx;

		PaxosMsg * pMsg = new PaxosMsg();
		pMsg->set_gltype(GroupMsgPaxos);
		pMsg->set_instanceid(_oConfig.uInstanceID);
		pMsg->set_iltype(PaxosMsgTypeNewProposal);

		// When this instance is using NewPropose, then this instance should just allocate, instance's 
		// ioloop should be empty, and when group calling NewPropose, it should use group ioloop's thread
		// then add message to the back or front of ioloop's message queue should make no difference
		_oIOLoop.AddMessage(pMsg, false);

		Dprintf(InstanceLevel, "%s[NewPropose]:%s@%d\n", InstanceInfo().c_str(), pCtx->GetProposalValue().c_str(), _oConfig.uInstanceID);
	}

	void Instance::IOLoopOnMessage(PaxosMsg * pMsg) {
		assert(pMsg->instanceid() == _oConfig.uInstanceID || _oConfig.uInstanceID == 0);

		switch (pMsg->iltype()) {
		case PaxosMsgTypeNewProposal:
			_oProposer.NewProposal(_pCommitCtx->GetProposalValue());
			break;
		case PaxosMsgTypePrepare:
			_oAcceptor.OnPrepare(pMsg);
			break;
		case PaxosMsgTypePrepareReply:
			_oProposer.OnPrepareReply(pMsg);
			break;
		case PaxosMsgTypeAccept:
			_oAcceptor.OnAccept(pMsg);
			break;
		case PaxosMsgTypeAcceptReply:
			_oProposer.OnAcceptReply(pMsg);
			break;
		case PaxosMsgTypeLearnValue:
			_oLearner.LearnFromOthers(pMsg->value());
			break;
		default:
			assert(0);
		}

		// Memory is allocate when instance receive message from group
		delete pMsg;

		if (_oLearner.DoneConsensus()) {
			Dprintf(InstanceLevel, "%s[DoneConsensus]Reseting\n", InstanceInfo().c_str());

			// TODO : Modify here, this is because when context is commit, then it will instancely destroyed
			// this will cause _pCommitCtx->GetCommitStatus fail

			CommitCtx * pCtx = _pCommitCtx;
			uint32_t doneInstanceID = _oConfig.uInstanceID;
			std::string consensusValue = _oLearner.GetConsensusResult();

			Reset();

			if (pCtx != nullptr) {
				pCtx->Commit(consensusValue);

				_pState->oRWLock.lock();
				if (pCtx->GetCommitStatus() == CommitCtx::Success) {
					_pState->uLastAcceptInstanceID = doneInstanceID;
				}
				else {
					_pState->uLastRejectInstanceID = doneInstanceID;
				}
				_pState->oRWLock.unlock();
			}

			_pGroup->InstanceDoneConsensus(doneInstanceID, consensusValue);

			if (pCtx != nullptr) {
				pCtx->Notify();
			}
		}
	}

	void Instance::IOLoopOnTimeout(const uint32_t uTimerID, const int iTimerType) {
		switch (iTimerType) {
		case PaxosTimerPrepare:
			_oProposer.OnPrepareTimeout();
			break;
		case PaxosTimerAccept:
			_oProposer.OnAcceptTimeout();
			break;
		default:
			assert(0);
		}
	}

	// After instance finish consensus
	void Instance::Reset() {
		Dprintf(InstanceLevel, "%s[Reset]\n", InstanceInfo().c_str());

		_pCommitCtx = nullptr;

		_oIOLoop.SetAcceptInstanceID(0);
		_oIOLoop.Clear();

		_oConfig.uInstanceID = 0;
		
		_oProposer.Reset();
		_oAcceptor.Reset();
		_oLearner.Reset();
	}

	std::string Instance::InstanceInfo() const {
		return ToString("{I:Node:", _pState->uNodeID, ",Ins:", _oConfig.uInstanceID, ",MaxOther:", _pState->uMaxOtherProposalID, "}");
	}
}