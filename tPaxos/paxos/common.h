#ifndef TPAXOS_COMMON_H
#define TPAXOS_COMMON_H

#include <stdint.h>
#include <shared_mutex>

namespace tPaxos {
	enum GroupMsgType {
		GroupMsgPaxos = 0,
		GroupMsgInternal,
		GroupMsgProbe,
		GroupMsgProbeReply,
		GroupMsgLookup,
	};

	enum PaxosMsgType {
		PaxosMsgTypeNewProposal = 0,
		PaxosMsgTypePrepare,
		PaxosMsgTypePrepareReply,
		PaxosMsgTypeAccept,
		PaxosMsgTypeAcceptReply,
		PaxosMsgTypeLearnValue,
	};

	enum TimerType {
		PaxosTimerPrepare,
		PaxosTimerAccept,
		SyncerTimerProbe,
	};

	enum SyncerTaskType {
		SyncerTaskProbeReply = 0,
		SyncerTaskUpdatePeer,
	};

	class SyncerTask {
	public:
		SyncerTaskType oType;
		uint32_t uNodeID;
		uint32_t uInstanceID;
	};

	class State {
	public:
		State() = default;
		State(const uint32_t uNid, const uint32_t uMaj, const uint32_t uAl) : State(0, 0, 0, uNid, uMaj, uAl) {}
		State(const uint32_t uPid, const uint32_t uAid, const uint32_t uRid, const uint32_t uNid, \
			const uint32_t uMaj, const uint32_t uAl) : uMaxOtherProposalID(uPid), uLastAcceptInstanceID(uAid), \
			uLastRejectInstanceID(uRid), uNodeID(uNid), uMajority(uMaj), uAll(uAl) {}

		std::shared_mutex oRWLock;
		uint32_t uMaxOtherProposalID;
		uint32_t uLastAcceptInstanceID;
		uint32_t uLastRejectInstanceID;

		uint32_t uNodeID;
		uint32_t uMajority;
		uint32_t uAll;
	};

	class Config {
	public:
		Config() : uInstanceID(0) {}

		uint32_t uInstanceID;
	};

	class BallotSequence {
	public:
		BallotSequence() : BallotSequence(0, 0) {}
		BallotSequence(const uint32_t uProposal, const uint32_t uNode) :uProposalID(uProposal), uNodeID(uNode) {}

		void Reset() {
			uProposalID = 0;
			uNodeID = 0;
		}

		bool operator<(const BallotSequence & rhs) const {
			return uProposalID < rhs.uProposalID || (uProposalID == rhs.uProposalID && uNodeID < rhs.uNodeID);
		}

		bool operator==(const BallotSequence & rhs) const {
			return uProposalID == rhs.uProposalID && uNodeID == rhs.uNodeID;
		}

		bool operator>(const BallotSequence & rhs) const {
			return uProposalID > rhs.uProposalID || (uProposalID == rhs.uProposalID && uNodeID > rhs.uNodeID);
		}

		uint32_t uProposalID;
		uint32_t uNodeID;
	};
}

#endif
