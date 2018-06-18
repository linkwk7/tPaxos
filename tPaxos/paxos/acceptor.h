#ifndef TPAXOS_ACCEPTOR_H
#define TPAXOS_ACCEPTOR_H

#include <string>

#include "common.h"
#include "util/message.pb.h"
#include "util/network.h"
#include "util/util.h"

namespace tPaxos {
	class Acceptor {
	public:

		Acceptor(Config * pConfig, State * pState, NetworkClient * pNetwork);

		void OnPrepare(const PaxosMsg * pMsg);
		void OnAccept(const PaxosMsg * pMsg);

		void Reset();

	private:
		std::string AcceptorInfo() const;

	private:
		BallotSequence _oPromiseBallot;
		BallotSequence _oAcceptBallot;
		std::string _sValue;

		Config * _pConfig;
		State * _pState;

		NetworkClient * _pNetwork;
	};
}

#endif