#include "group.h"

namespace tPaxos {
	Group::Group(const uint32_t uID) : _oIOLoop(this), _uMinInstanceID(1), _uMaxInstanceID(1), \
		_oSyncer(&_oState, &_oLog), _pNetwork(nullptr) {
		_oState.uMaxOtherProposalID = 0;
		_oState.uLastAcceptInstanceID = 0;
		_oState.uLastRejectInstanceID = 0;
		_oState.uNodeID = uID;
		_oState.uMajority = 2;
		_oState.uAll = 3;

		for (int i = 0; i < MAXINSTANCENUM; i++) {
			_oIdleInstance.push_back(i);
		}
		for (int i = 0; i < MAXINSTANCENUM; i++) {
			_oDoneInstanceID[i] = false;
		}
	}

	Group::~Group() {
		_oIOLoop.Stop();

		std::vector<std::thread> exitThreads;
		for (int i = 0; i < MAXINSTANCENUM; i++) {
			exitThreads.push_back(std::thread(
				[&](int ind) {delete _oInstances[ind]; }, i));
		}
		for (int i = 0; i < MAXINSTANCENUM; i++) {
			exitThreads[i].join();
		}
	}

	void Group::Init(NetworkClient * pNetwork) {
		_pNetwork = pNetwork;

		for (int i = 0; i < MAXINSTANCENUM; i++) {
			_oInstances.push_back(new Instance(this, &_oState, _pNetwork));
		}

		_oIOLoop.Run();

		_oSyncer.Init(pNetwork);

		Dprintf(GroupLevel, "%s[GroupInit]\n", GroupInfo().c_str());
	}

	void Group::NewPropose(const std::string & sValue) {
		Dprintf(GroupLevel, "%s[GroupNewPropose]:Value:%s\n", GroupInfo().c_str(), sValue.c_str());

		CommitCtx ctx(sValue);

		for (int retry = 0; retry < 3; retry++) {
			ctx.ResetStatus();

			_oLock.lock();
			_oCommitCtxs.push_back(&ctx);
			_oLock.unlock();

			ctx.Wait();

			if (ctx.GetCommitStatus() == CommitCtx::Success) {
				Dprintf(GroupLevel, "%s[GroupCommitSuccess]:%s @ %u\n", GroupInfo().c_str(), ctx.GetProposalValue().c_str(), \
					ctx.GetCommitInstanceID());
				return;
			}
			else {
				Dprintf(GroupLevel, "%s[GroupCommiRetry]:%s @ %u\n", GroupInfo().c_str(), ctx.GetProposalValue().c_str(), \
					ctx.GetCommitInstanceID());
			}
		}
		Dprintf(GroupLevel, "%s[GroupCommitFail]:%s\n", GroupInfo().c_str(), ctx.GetProposalValue().c_str());
	}

	// Memory allocate here or at InstanceDoneConsensus, it will release once done processing 
	// or proving message is too old
	void Group::OnReceiveNewMessage(const std::string & sMsg) {
		PaxosMsg * msg = new PaxosMsg();
		msg->ParseFromString(sMsg);

		_oIOLoop.AddMessage(msg);
	}

	// When consensus is done, it only add a message into ioloop rather than clean instance, when 
	// group encounter this message, it will clean instance, this will prevent race condition cause 
	// by InstanceDoneConsensus'thread and IOLoop's thread
	void Group::InstanceDoneConsensus(const uint32_t uInstanceID, const std::string & sValue) {
		PaxosMsg * msg = new PaxosMsg();
		msg->set_gltype(GroupMsgInternal);
		msg->set_instanceid(uInstanceID);

		_oIOLoop.AddMessage(msg, false);

		Error err = _oLog.SetConsensusResult(uInstanceID, sValue);
		if (err.Errno != 0) {
			Dprintf(GroupLevel, "%s[AssertFail]:%s\n", GroupInfo().c_str(), err.Desc);
			assert(0);
		}

		Dprintf(GroupLevel, "%s[GroupDoneConsensus]:%u\n", GroupInfo().c_str(), uInstanceID);
	}

	bool Group::IOLoopOnMessage(PaxosMsg * pMsg) {
		switch (pMsg->gltype()) {
		case GroupMsgPaxos:
			return OnPaxosMessage(pMsg);
			break;
		case GroupMsgInternal:
			return OnInternalMessage(pMsg);
			break;
		case GroupMsgProbe:
			return OnProbeMessage(pMsg);
			break;
		case GroupMsgProbeReply:
			return OnProbeReplyMessage(pMsg);
			break;
		case GroupMsgLookup:
			return OnLookupMessage(pMsg);
			break;
		default:
			assert(0);
		}
		return false;
	}

	void Group::IOLoopNewPropose() {
		if (_oIdleInstance.size() == 0 || _oCommitCtxs.size() == 0) {
			return;
		}

		_oLock.lock();

		for (; !_oCommitCtxs.empty(); ) {
			// This may lead to waste some time to wait an old instance's execution
			if (_uMaxInstanceID < _uMinInstanceID + MAXINSTANCENUM) {
				int newInstanceIndex = -1;
				uint32_t instanceID = _uMaxInstanceID;
				if (AllocateNewInstance(instanceID, newInstanceIndex)) {
					_oCommitCtxs[0]->StartCommit(instanceID);

					_oInstances[newInstanceIndex]->InitForNewInstance(instanceID);
					_oInstances[newInstanceIndex]->NewPropose(_oCommitCtxs[0]);

					Dprintf(GroupLevel, "%s[Group.NewInstance]:%s @ %u\n", GroupInfo().c_str(), _oCommitCtxs[0]->GetProposalValue().c_str(), \
						instanceID);

					_oCommitCtxs.pop_front();
				}
				else {
					assert(0);
				}
			}
			else {
				Dprintf(GroupLevel, "%s[New] instance id too large\n", GroupInfo().c_str());
				break;
			}
		}

		Dprintf(GroupLevel, "%s[New] Context dispatch done\n", GroupInfo().c_str());

		_oLock.unlock();
	}

	bool Group::OnPaxosMessage(PaxosMsg * pMsg) {
		//Dprintf(GroupLevel, "%s[G.ReceiveMsg]:%s\n", GroupInfo().c_str(), ToString(*pMsg).c_str());

		if (pMsg->instanceid() >= _uMinInstanceID + MAXINSTANCENUM) {
			_oIOLoop.AddRetry(pMsg);
			return false;
		}

		if (pMsg->instanceid() < _uMinInstanceID || \
			_oDoneInstanceID[pMsg->instanceid() - _uMinInstanceID] == true) {
			// Old message, send notification, maybe send consensus result or checkpoint

			/*std::string consensusResult;
			Error err = _oLog.GetConsensusResult(pMsg->instanceid(), consensusResult);
			if (err.Errno == 0) {
				PaxosMsg msg;
				msg.set_gltype(GroupMsgPaxos);
				msg.set_instanceid(pMsg->instanceid());
				msg.set_nodeid(_oState.uNodeID);
				msg.set_iltype(PaxosMsgTypeLearnValue);
				msg.set_value(consensusResult);

				_pNetwork->Send(pMsg->nodeid(), msg.SerializeAsString());
			}
			else {
				Dprintf(GroupLevel, "%s[AssertFail] @ %u\n", GroupInfo().c_str(), pMsg->instanceid());
				assert(0);
			}*/
		}
		else {
			auto it = _oInstanceMap.find(pMsg->instanceid());
			if (it == _oInstanceMap.end()) {
				int index = -1;
				if (AllocateNewInstance(pMsg->instanceid(), index)) {
					_oInstances[index]->InitForNewInstance(pMsg->instanceid());
					_oInstances[index]->OnReceiveNewMessage(*pMsg);

					Dprintf(GroupLevel, "%s[Group.NewInstance]: %u\n", GroupInfo().c_str(), pMsg->instanceid());
				}
				else {
					assert(0);
				}
			}
			else {
				// instance will make a copy of this message, so it's safe to delete this message here
				_oInstances[it->second]->OnReceiveNewMessage(*pMsg);
			}
		}

		delete pMsg;
		
		return true;
	}

	bool Group::OnInternalMessage(PaxosMsg * pMsg) {
		auto it = _oInstanceMap.find(pMsg->instanceid());
		if (it == _oInstanceMap.end()) {
			assert(0);
		}

		_oIdleInstance.push_back(it->second);
		_oInstanceMap.erase(it);

		// Update instance statistics, _uMinInstanceID, _uMaxInstanceID
		UpdateMinMaxInstanceID(pMsg->instanceid());

		delete pMsg;

		return true;
	}

	bool Group::OnProbeMessage(PaxosMsg * pMsg) {
		if (pMsg->nodeid() != _oState.uNodeID) {
			PaxosMsg msg;
			msg.set_gltype(GroupMsgProbeReply);
			msg.set_instanceid(_uMinInstanceID);
			msg.set_nodeid(_oState.uNodeID);

			_pNetwork->Send(pMsg->nodeid(), msg.SerializeAsString());
		}
		return true;
	}

	bool Group::OnProbeReplyMessage(PaxosMsg * pMsg) {
		Dprintf(GroupLevel, "%s[ReceiveProbeReply] %u @ %u\n", GroupInfo().c_str(), pMsg->nodeid(), pMsg->instanceid());
		if (pMsg->nodeid() != _oState.uNodeID) {
			_oSyncer.AddTask(SyncerTaskProbeReply, pMsg->nodeid(), pMsg->instanceid());
		}
		return true;
	}

	bool Group::OnLookupMessage(PaxosMsg * pMsg) {
		if (pMsg->nodeid() != _oState.uNodeID) {
			_oSyncer.AddTask(SyncerTaskUpdatePeer, pMsg->nodeid(), pMsg->instanceid());
		}
		return true;
	}

	bool Group::AllocateNewInstance(const uint32_t uInstanceID, int & iIndex) {
		if (uInstanceID < _uMinInstanceID || uInstanceID >= _uMinInstanceID + MAXINSTANCENUM) {
			Dprintf(GroupLevel, "%s[AsserfFail.Range]:allocate:%u\n", GroupInfo().c_str(), uInstanceID);
			return false;
		}

		if (_oInstanceMap.find(uInstanceID) != _oInstanceMap.end()) {
			Dprintf(GroupLevel, "%s[AsserfFail.Existed]:allocate:%u\n", GroupInfo().c_str(), uInstanceID);
			return false;
		}

		if (_oIdleInstance.size() == 0) {
			Dprintf(GroupLevel, "%s[AsserfFail.None]:allocate:%u\n", GroupInfo().c_str(), uInstanceID);
			return false;
		}

		iIndex = _oIdleInstance.back();
		_oIdleInstance.pop_back();
		_oInstanceMap[uInstanceID] = iIndex;

		_uMaxInstanceID = (_uMaxInstanceID > uInstanceID ? _uMaxInstanceID:uInstanceID+1);

		Dprintf(GroupLevel, "%s[AllocateNewInstance] for %u @ %i\n", GroupInfo().c_str(), uInstanceID, iIndex);

		return true;
	}

	void Group::UpdateMinMaxInstanceID(const uint32_t uDone) {
		if (uDone < _uMinInstanceID || uDone > _uMinInstanceID + MAXINSTANCENUM || uDone > _uMaxInstanceID) {
			Dprintf(GroupLevel, "%s[AssertFail]: Done:%u\n", GroupInfo().c_str(), uDone, _uMinInstanceID);
			assert(0);
		}

		Dprintf(GroupLevel, "%s[Done]%u\n", GroupInfo().c_str(), uDone);

		_oDoneInstanceID[uDone - _uMinInstanceID] = true;

		if (_oDoneInstanceID[0] == true) {
			// Set to MAXINSTANCENUM to prevent all instance is idle...
			// Spend two hours trying to figure this out...
			int firstUnDone = MAXINSTANCENUM;
			for (int i = 0; i < MAXINSTANCENUM; i++) {
				if (_oDoneInstanceID[i] == false) {
					firstUnDone = i;
					break;
				}
			}

			Dprintf(GroupLevel, "%s[RecoveryInstance]:%u - %u\n", GroupInfo().c_str(), _uMinInstanceID, _uMinInstanceID + firstUnDone);

			_uMinInstanceID = _uMinInstanceID + firstUnDone;
			for (int i = firstUnDone; i < MAXINSTANCENUM + firstUnDone; i++) {
				if (i < MAXINSTANCENUM) {
					_oDoneInstanceID[i - firstUnDone] = _oDoneInstanceID[i];
				}
				else {
					_oDoneInstanceID[i - firstUnDone] = false;
				}
			}

			_oLog.SetContinuousLogID(_uMinInstanceID);
		}
	}

	std::string Group::GroupInfo() const {
		return ToString("{G:Node:",_oState.uNodeID, ",Min:", _uMinInstanceID, ",Max:", _uMaxInstanceID, ",Idle:",\
			_oIdleInstance.size(), ",Ctxs:", _oCommitCtxs.size(), "}");
	}
}