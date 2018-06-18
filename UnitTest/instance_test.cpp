#include "pch.h"

#include <vector>
#include <map>
#include <string>
#include "gtest/gtest.h"

#include "test_config.h"
#include "paxos/instance.h"
#include "util/util.h"
#include "util/network.h"

// Simulate a group with one instance
class SGroup : public tPaxos::GroupReceiver, public tPaxos::NetworkReceiver {
public:
	SGroup(const uint32_t id) : uInstanceID(1), pInstance(nullptr), pNetwork(nullptr) {
		oState.uMaxOtherProposalID = 0;
		oState.uLastAcceptInstanceID = 0;
		oState.uLastRejectInstanceID = 0;
		oState.uNodeID = id;
		oState.uMajority = 2;
		oState.uAll = 3;
	}

	~SGroup() {
		delete pInstance;
	}

	void Init(tPaxos::NetworkClient * pNet) {
		pNetwork = pNet;

		pInstance = new tPaxos::Instance(this, &oState, pNet);
	}
	
	void InstanceDoneConsensus(const uint32_t uIID, const std::string & sValue) override {
		tPaxos::Dprintf(tPaxos::UtilLevel, "GReceiver%d[DoneConsensus]:%lu\n", oState.uNodeID, uIID);

		pInstance->InitForNewInstance(uInstanceID+1);

		uInstanceID++;
	}

	void OnReceiveNewMessage(const std::string & sMsg) override {

		tPaxos::PaxosMsg oMsg;
		oMsg.ParseFromString(sMsg);

		if (oMsg.instanceid() == uInstanceID) {
			//tPaxos::Dprintf("Group%d receive %s for %d\n", oState.uNodeID, tPaxos::ToString(oMsg).c_str(), uInstanceID);

			pInstance->OnReceiveNewMessage(oMsg);
		}
	}

	uint32_t uInstanceID;
	tPaxos::State oState;
	tPaxos::Instance * pInstance;
	tPaxos::NetworkClient * pNetwork;
};

TEST(InstanceTest, ConsensusOne) {
	Config<SGroup> conf(3);

	for (int j = 0; j < conf.iNodeNum; j++) {
		conf.oGroups[j]->pInstance->InitForNewInstance(1);
	}

	tPaxos::CommitCtx ctx("Content");

	ctx.StartCommit(conf.oGroups[0]->uInstanceID);
	conf.oGroups[0]->pInstance->NewPropose(&ctx);

	ctx.Wait();
}

TEST(InstanceTest, ConsensusMultiple) {
	Config<SGroup> conf(3);

	for (int j = 0; j < conf.iNodeNum; j++) {
		conf.oGroups[j]->pInstance->InitForNewInstance(1);
	}

	// Group should make sure new propose is handled in a brand new instance
	for (int i = 0; i < 100000; i++) {
		tPaxos::CommitCtx ctx("Content");

		while (conf.oGroups[0]->uInstanceID != (i+1));

		ctx.StartCommit(conf.oGroups[0]->uInstanceID);
		conf.oGroups[0]->pInstance->NewPropose(&ctx);
		
		ctx.Wait();
	}
}

void ConsensusMultiMasterAux(Config<SGroup> * conf, int index) {
	int commitInstanceID = 1;

	for (int i = 0; i < 5; i++) {
		tPaxos::CommitCtx ctx(tPaxos::ToString(index,"-",i));
		do {
			while (conf->oGroups[0]->uInstanceID != commitInstanceID);
			
			ctx.StartCommit(commitInstanceID);
			conf->oGroups[index]->pInstance->NewPropose(&ctx);

			ctx.Wait();

			commitInstanceID++;
		} while (ctx.GetCommitStatus() != tPaxos::CommitCtx::Success);
	}
}