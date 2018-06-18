#ifndef TPAXOS_TEST_CONFIG_H
#define TPAXOS_TEST_CONFIG_H

#include "paxos/instance.h"
#include "util/util.h"
#include "util/network.h"

template <typename T>
class Config {
public:
	Config(int nodeNum) : iNodeNum(nodeNum), oTransfer(nodeNum) {
		// Initialize network and receiver(SGroup)
		for (int i = 0; i < iNodeNum; i++) {
			oInfoMap[i] = tPaxos::NodeInfo(i, "", 0);
		}
		for (int i = 0; i < iNodeNum; i++) {
			oGroups.push_back(new T(i));
		}
		for (int i = 0; i < iNodeNum; i++) {
			oNetworks.emplace_back(tPaxos::QueueNetwork(i, &oTransfer, oGroups[i]));
			oNetworks[i].Init(oInfoMap);
		}
		for (int i = 0; i < iNodeNum; i++) {
			oNetMap[i] = &oNetworks[i];
		}

		// Initialize network tranfer
		oTransfer.Init(oNetMap);
		oTransfer.Run();

		// Initialize Group
		for (int i = 0; i < iNodeNum; i++) {
			oGroups[i]->Init(&oNetworks[i]);
		}
	}

	~Config() {
		oTransfer.Stop();

		for (int i = 0; i < iNodeNum; i++) {
			delete oGroups[i];
		}
	}


	int iNodeNum;
	std::map<uint32_t, tPaxos::NodeInfo> oInfoMap;
	std::map<uint32_t, tPaxos::NetworkClient*> oNetMap;
	tPaxos::QueueTransfer oTransfer;
	std::vector<T*> oGroups;
	std::vector<tPaxos::QueueNetwork> oNetworks;
};

#endif
