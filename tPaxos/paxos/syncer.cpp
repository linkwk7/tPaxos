#include "syncer.h"

namespace tPaxos {
	Syncer::Syncer(State * pState, Log * pLog) : _oIOLoop(this), _pState(pState), \
		_pLog(pLog) {}

	Syncer::~Syncer() {
		_oIOLoop.Stop();
	}

	void Syncer::Init(NetworkClient * pNetwork) {
		_pNetwork = pNetwork;

		_oIOLoop.Run();
	}

	void Syncer::AddTask(SyncerTaskType oType, const uint32_t uNodeID, const uint32_t uInstanceID) {
		SyncerTask task;
		task.oType = oType;
		task.uNodeID = uNodeID;
		task.uInstanceID = uInstanceID;

		_oIOLoop.AddTask(task);
	}

	void Syncer::IOLoopOnTimeout() {
		PaxosMsg msg;
		msg.set_gltype(GroupMsgProbe);
		msg.set_nodeid(_pState->uNodeID);

		_pNetwork->Broadcast(false, msg.SerializeAsString());
	
		// This is reset every timeout to prevent peer didn't got enough log
		_uLastRequestInstance = _pLog->GetContinuousLogID();

		Dprintf(GroupLevel, "%s[Probe]:%u\n", SyncerInfo().c_str(), _uLastRequestInstance);
	}

	void Syncer::IOLoopOnTask(const SyncerTask & task) {
		switch (task.oType) {
		case SyncerTaskProbeReply:
			OnProbeReply(task);
			break;
		case SyncerTaskUpdatePeer:
			OnSendUpdate(task);
			break;
		default:
			assert(0);
		}
	}

	void Syncer::OnProbeReply(const SyncerTask & task) {
		for (uint32_t i = _pLog->GetContinuousLogID(); i < task.uInstanceID; i++) {
			if (_pLog->EntryNotExist(i)) {
				PaxosMsg msg;
				msg.set_gltype(GroupMsgLookup);
				msg.set_instanceid(i);
				msg.set_nodeid(_pState->uNodeID);

				_pNetwork->Send(task.uNodeID, msg.SerializeAsString());

				Dprintf(GroupLevel, "%s[Lookup]:%u\n", SyncerInfo().c_str(), i);
			}
		}
		//_uLastRequestInstance = (_uLastRequestInstance > task.uInstanceID ? _uLastRequestInstance: task.uInstanceID);
	}

	void Syncer::OnSendUpdate(const SyncerTask & task) {
		std::string res;
		Error err = _pLog->GetConsensusResult(task.uInstanceID, res);
		if (err.Errno != 0) {
			Dprintf(GroupLevel, "[AssertFail]%u didn't exist\n", task.uInstanceID);
			assert(0);
		}

		PaxosMsg msg;
		msg.set_gltype(GroupMsgPaxos);
		msg.set_instanceid(task.uInstanceID);
		msg.set_nodeid(_pState->uNodeID);
		msg.set_iltype(PaxosMsgTypeLearnValue);
		msg.set_value(res);

		_pNetwork->Send(task.uNodeID, msg.SerializeAsString());

		Dprintf(GroupLevel, "%s[SendUpdate]: to %u @ %u\n", SyncerInfo().c_str(), task.uNodeID, task.uInstanceID);
	}

	std::string Syncer::SyncerInfo() {
		return ToString("{S:Node:", _pState->uNodeID,/* ",Min:", _pLog->GetContinuousLogID(), */"}");
	}
}