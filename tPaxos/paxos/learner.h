#ifndef TPAXOS_LEARNER_H
#define TPAXOS_LEARNER_H

#include <string>

namespace tPaxos {
	class Learner {
	public:

		Learner();

		void LearnFromProposer(const std::string & sValue);
		void LearnFromOthers(const std::string & sValue);

		bool DoneConsensus() const;

		const std::string & GetConsensusResult();

		void Reset();

	private:
		bool _bReachConsensus;
		std::string _sValue;
	};
}

#endif
