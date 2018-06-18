#include "timer.h"

namespace tPaxos {
	uint64_t Timers::GetSteadyClockMS() {
		auto currentTime = std::chrono::steady_clock::now();
		uint64_t now = (std::chrono::duration_cast<std::chrono::milliseconds>(currentTime.time_since_epoch())).count();
		return now;
	}

	void Timers::AddTimer(const uint64_t absTime, const int type, uint32_t & timerID) {
		timerID = _uNextTimerID++;

		_oTimerQueue.emplace(Timer(timerID, absTime, type));
	}

	int Timers::GetNextTimeout() const {
		if (_oTimerQueue.empty()) {
			return -1;
		}

		Timer topTimer = _oTimerQueue.top();
		uint64_t now = GetSteadyClockMS();
		if (topTimer.uAbsTimeout > now) {
			return static_cast<int> (topTimer.uAbsTimeout - now);
		}
		return 0;
	}

	bool Timers::PopTimeout(uint32_t & timerID, int & type) {
		if (_oTimerQueue.empty()) {
			return false;
		}

		Timer topTimer = _oTimerQueue.top();
		uint64_t now = GetSteadyClockMS();
		if (topTimer.uAbsTimeout > now) {
			return false;
		}

		timerID = topTimer.uTimerID;
		type = topTimer.iType;
		_oTimerQueue.pop();
		return true;
	}

	void Timers::Clear() {
		_oTimerQueue = std::priority_queue<Timer>();
	}
}