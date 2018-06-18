#ifndef TPAXOS_UTIL_H
#define TPAXOS_UTIL_H

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sstream>
#include <initializer_list>

#include "message.pb.h"

namespace tPaxos {

	enum DebugLevel {
		GroupLevel = 1,
		InstanceLevel = 2,
		UtilLevel = 4,
		ALL = 0xff,
	};

	const bool Debug = true;
	const DebugLevel Level = ALL;

	void Dprintf(DebugLevel level, const char * format, ...);
	void Dprintf(DebugLevel level, const std::string& msg);

	template <typename T>
	void add2SS(std::stringstream& ss, const T & arg) {
		ss << arg;
	}

	template <>
	void add2SS<PaxosMsg>(std::stringstream & ss, const PaxosMsg & arg);

	template <typename... T>
	std::string ToString(const T&... args) {
		std::stringstream ss;
		// I really don't understand how this work
		int arr[] = {(add2SS(ss, args), 0)...};

		return ss.str();
	}

	uint32_t XorShift32();
	uint32_t XorShift64();
	uint32_t XorShift96();
	uint32_t XorShift128();
	uint32_t XorShift160();
	uint32_t XorShiftwow();

	uint32_t XorShift();
}

#endif
