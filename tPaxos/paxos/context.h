#ifndef TPAXOS_CONTEXT_H
#define TPAXOS_CONTEXT_H

#include <stdint.h>
#include <string>
#include <mutex>
#include <condition_variable>

namespace tPaxos {
	class CommitCtx {
	public:

		enum CommitStatus {
			Success,
			Fail,
			UnDone,
		};

		CommitCtx(const std::string & sValue) : _uInstanceID(static_cast<uint32_t>(-1)), _bCommitDone(false),\
			_oStatus(Success), _sProposalValue(sValue) {}

		bool IsNewCommit() {
			return _uInstanceID == static_cast<uint32_t>(-1);
		}

		void StartCommit(const uint32_t uInstanceID) {
			_uInstanceID = uInstanceID;
		}

		void Wait() {
			_oLock.lock();

			for (; !_bCommitDone;) {
				_oCond.wait(_oLock);
			}

			_oLock.unlock();
		}

		uint32_t GetCommitInstanceID() const {
			return _uInstanceID;
		}

		const std::string & GetProposalValue() const {
			return _sProposalValue;
		}

		CommitStatus GetCommitStatus() const {
			return _oStatus;
		}

		void Commit(const std::string & sConsensus) {
			if (sConsensus == _sProposalValue) {
				_oStatus = Success;
			}
			else {
				_oStatus = Fail;
			}
		}

		void Notify() {
			_oLock.lock();
			_bCommitDone = true;
			_oCond.notify_one();
			_oLock.unlock();
		}

		void ResetStatus() {
			_uInstanceID = 0;
			_bCommitDone = false;
			_oStatus = UnDone;
		}

	private:
		uint32_t _uInstanceID;
		bool _bCommitDone;
		CommitStatus _oStatus;

		std::string _sProposalValue;

		std::mutex _oLock;
		std::condition_variable_any _oCond;
	};
}

#endif
