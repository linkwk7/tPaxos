#ifndef TPAXOS_TIMER_H
#define TPAXOS_TIMER_H

#include <stdint.h>
#include <chrono>
#include <queue>

namespace tPaxos {
	class Timers {
	public:
		Timers() : _uNextTimerID(1) {}

		static uint64_t GetSteadyClockMS();

		void AddTimer(const uint64_t absTime, const int type, uint32_t & timerID);
		// Return time util next timeout, -1 if didn't exist any
		int GetNextTimeout() const;
		bool PopTimeout(uint32_t & timerID, int & type);
		void Clear();
		
	private:
		class Timer {
		public:
			Timer() = default;
			Timer(const uint32_t id, const uint64_t timeout, const int type) : uTimerID(id),
				uAbsTimeout(timeout), iType(type) {}

			bool operator < (const Timer & t) const {
				if (uAbsTimeout == t.uAbsTimeout) {
					return t.uTimerID < uTimerID;
				}
				return t.uAbsTimeout < uAbsTimeout;
			}

			uint32_t uTimerID;
			uint64_t uAbsTimeout;
			int iType;
		};

		uint32_t _uNextTimerID;
		std::priority_queue<Timer> _oTimerQueue;
	};
}

#endif