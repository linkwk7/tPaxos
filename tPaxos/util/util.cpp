#include "util.h"

namespace tPaxos {
	static FILE* logFile = fopen("log.txt", "w+");

	static std::string GLTypeStr[] = {
		"Paxos",
		"GInternal",
		"IInternal",
	};

	static std::string ILTypeStr[] = {
		"NewProposal",
		"Prepare",
		"PrepareReply",
		"Accept",
		"AcceptReply",
		"LearnValue",
	};
	
	void Dprintf(DebugLevel level, const char * format, ...) {
		if (Debug && (level&Level != 0 || Level==ALL)) {
			va_list args = NULL;
			va_start(args, format);
			vfprintf(logFile, format, args);
			va_end(args);

			fflush(logFile);
		}
	}

	void Dprintf(DebugLevel level, const std::string & str) {
		if (Debug && (level&Level != 0 || Level == ALL)) {
			fprintf(logFile, "%s", str.c_str());
			fflush(logFile);
		}
	}

	template <>
	void add2SS<PaxosMsg>(std::stringstream & ss, const PaxosMsg & arg) {
		ss << "[GType:" << GLTypeStr[arg.gltype()] << ",InsID:" << arg.instanceid() << ",Node:" << arg.nodeid() << ",IType:" << \
			ILTypeStr[arg.iltype()] << ",Proposal:" << arg.proposalid() << ",Value:" << arg.value() << ",AccProposal:" << \
			arg.acceptedproposalid() << ",AccProNode:" << arg.acceptednodeid() << ",Rejct:" << arg.reject() << ",RejProposal:" << \
			arg.promisedproposalid() << "]";
	}

	uint32_t XorShift32() {
		static uint32_t y = 2463534242UL;
		y ^= (y << 13); y ^= (y >> 17); return (y ^= (y << 15));
	}

	uint32_t XorShift64() {
		static uint32_t x = 123456789, y = 362436069;
		uint32_t t = (x ^ (x << 10)); x = y; return y = (y ^ (y >> 10)) ^ (t ^ (t >> 13));
	}

	uint32_t XorShift96() {
		static uint32_t x = 123456789, y = 362436069, z = 521288629;
		uint32_t t = (x ^ (x << 10)); x = y; y = z; return z = (z ^ (z >> 26)) ^ (t ^ (t >> 5));
	}

	uint32_t XorShift128() {
		static uint32_t x = 123456789, y = 362436069, z = 521288629,
			w = 88675123;
		uint32_t t = (x ^ (x << 11)); x = y; y = z; z = w; return w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
	}

	uint32_t XorShift160() {
		static uint32_t x = 123456789, y = 362436069, z = 521288629,
			w = 88675123, v = 5783321;
		uint32_t t = (x ^ (x << 2)); x = y; y = z; z = w; w = v; return v = (v ^ (v >> 4)) ^ (t ^ (t >> 1));
	}

	uint32_t XorShiftwow() {
		static uint32_t x = 123456789, y = 362436069, z = 521288629,
			w = 88675123, v = 5783321, d = 6615241;
		uint32_t t = (x ^ (x >> 2)); x = y; y = z; z = w; w = v; v = (v ^ (v << 4)) ^ (t ^ (t << 1)); return (d += 362437) + v;
	}

	uint32_t XorShift() {
		return XorShift64();
	}
}