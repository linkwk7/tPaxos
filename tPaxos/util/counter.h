#ifndef TPAXOS_COUNTER_H
#define TPAXOS_COUNTER_H

#include <set>

namespace tPaxos {
	class Counter {
	public:

		void Accept(uint32_t uNodeID) {
			if (_oReceived.find(uNodeID) == _oReceived.end()) {
				_oReceived.insert(uNodeID);
				_oAccepted.insert(uNodeID);
			}
		}

		void Reject(uint32_t uNodeID) {
			if (_oReceived.find(uNodeID) == _oReceived.end()) {
				_oReceived.insert(uNodeID);
				_oRejected.insert(uNodeID);
			}
		}

		bool AcceptReach(int n) const {
			return _oAccepted.size() >= n;
		}

		bool RejectReach(int n) const {
			return _oRejected.size() >= n;
		}

		bool ReceiveReach(int n) const {
			return _oReceived.size() >= n;
		}

		void Reset() {
			_oReceived.clear();
			_oAccepted.clear();
			_oRejected.clear();
		}
	private:
		std::set<uint32_t> _oReceived;
		std::set<uint32_t> _oAccepted;
		std::set<uint32_t> _oRejected;
	};
}

#endif
