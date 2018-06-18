#include "log.h"

namespace tPaxos {
	Log::Log() : _uInstanceID(1) {}

	Error Log::SetConsensusResult(const uint32_t uInstanceID, const std::string & sValue) {
		_oLock.lock();

		if (_oConsensusResult.find(uInstanceID) != _oConsensusResult.end()) {
			_oLock.unlock();
			return Error(1, "LogSetted");
		}

		_oConsensusResult[uInstanceID] = sValue;
		
		_oLock.unlock();

		return Error();
	}

	Error Log::GetConsensusResult(const uint32_t uInstanceID, std::string & sValue) {
		_oLock.lock();

		if (_oConsensusResult.find(uInstanceID) == _oConsensusResult.end()) {
			_oLock.unlock();
			return Error(2, "LogMissed");
		}

		sValue = _oConsensusResult[uInstanceID];
		
		_oLock.unlock();

		return Error();
	}

	void Log::SetContinuousLogID(const uint32_t uID) {
		_oLock.lock();

		_uInstanceID = uID;
	
		_oLock.unlock();
	}

	uint32_t Log::GetContinuousLogID() {
		_oLock.lock();

		uint32_t uID = _uInstanceID;

		_oLock.unlock();

		return uID;
	}

	bool Log::EntryNotExist(const uint32_t uID) {
		_oLock.lock();

		bool notExist = (_oConsensusResult.find(uID) == _oConsensusResult.end());

		_oLock.unlock();

		return notExist;
	}
}