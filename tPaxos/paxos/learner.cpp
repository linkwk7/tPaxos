#include "learner.h"

namespace tPaxos {
	Learner::Learner() : _bReachConsensus(false) {}

	void Learner::LearnFromProposer(const std::string & sValue) {
		_sValue = sValue;
		_bReachConsensus = true;
	}

	void Learner::LearnFromOthers(const std::string & sValue) {
		_sValue = sValue;
		_bReachConsensus = true;
	}

	bool Learner::DoneConsensus() const {
		return _bReachConsensus;
	}

	const std::string & Learner::GetConsensusResult() {
		return _sValue;
	}

	void Learner::Reset() {
		_bReachConsensus = false;
		_sValue = std::string();
	}
}