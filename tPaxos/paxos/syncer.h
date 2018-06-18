#ifndef TPAXOS_SYNCER_H
#define TPAXOS_SYNCER_H

#include <thread>
#include <chrono>
#include <map>
#include "common.h"
#include "log.h"
#include "ioloop.h"
#include "util/network.h"

namespace tPaxos {
	class Syncer : public SyncerIOLoopReceiver {
	public:
		Syncer(State * pState, Log * pLog);

		~Syncer();

		void Init(NetworkClient * pNetwork);

		void AddTask(SyncerTaskType oType, const uint32_t uNodeID, const uint32_t uInstanceID);

		void IOLoopOnTimeout() override;
		void IOLoopOnTask(const SyncerTask & task) override;

	private:
		void OnProbeReply(const SyncerTask & task);

		void OnSendUpdate(const SyncerTask & task);

		std::string SyncerInfo();

	private:

		SyncerIOLoop _oIOLoop;

		uint32_t _uLastRequestInstance;
		std::map<uint32_t, uint32_t> _oMinInstanceMap;

		State * _pState;
		Log * _pLog;

		NetworkClient * _pNetwork;
	};
}

#endif
