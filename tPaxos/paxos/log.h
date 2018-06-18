#ifndef TPAXOS_LOG_H
#define TPAXOS_LOG_H

#include <stdint.h>
#include <map>
#include <mutex>
#include <string>
#include "util/util.h"
#include "util/error.h"

namespace tPaxos {
	// TODO: 1. use log to record instance or group's state for fail recover.
	//		 2. persist log
	class Log {
	public:
		Log();

		Error SetConsensusResult(const uint32_t uInstanceID, const std::string & sValue);
		Error GetConsensusResult(const uint32_t uInstanceID, std::string & sValue);

		void SetContinuousLogID(const uint32_t uID);
		uint32_t GetContinuousLogID();

		bool EntryNotExist(const uint32_t uID);

	private:
		uint32_t _uInstanceID;
		std::map<uint32_t, std::string> _oConsensusResult;
		std::mutex _oLock;
	};
}

#endif
