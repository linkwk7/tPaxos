#ifndef TPAXOS_QUEUE_H
#define TPAXOS_QUEUE_H

#include <deque>
#include <mutex>
#include <condition_variable>

namespace tPaxos {
	template <typename T>
	class Queue {
	public:
		Queue() {}

		bool peek(T &t, const int timeoutMS) {
			while (empty()) {
				if (_oCond.wait_for(_oLock, std::chrono::milliseconds(timeoutMS)) == std::cv_status::timeout) {
					return false;
				}
			}
			t = _oStorage.front();
			return true;
		}

		T& peek() {
			while (empty()) {
				_oCond.wait(_oLock);
			}
			return _oStorage.front();
		}

		void pop() {
			_oStorage.pop_front();
		}

		void add(const T & elem, bool back = true) {
			if (back) {
				_oStorage.push_back(elem);
			}
			else {
				_oStorage.push_front(elem);
			}

			_oCond.notify_one();
		}

		bool empty() const {
			return _oStorage.empty();
		}

		size_t size() const {
			return _oStorage.size();
		}

		void clear() {
			_oStorage.clear();
		}

		void signalOne() {
			_oCond.notify_one();
		}

		void signalAll() {
			_oCond.notify_all();
		}

		void lock() {
			_oLock.lock();
		}

		void unlock() {
			_oLock.unlock();
		}

	private:
		std::mutex _oLock;
		std::condition_variable_any _oCond;
		std::deque<T> _oStorage;
	};
}

#endif
