#ifndef TPAXOS_ERROR_H
#define TPAXOS_ERROR_H

#include <string>

namespace tPaxos {
	class Error {
	public:
		Error() : Error(0, "") {}
		Error(const int iNO) : Error(iNO, "") {}
		Error(const int iNO, const std::string & sDes) : Errno(iNO), Desc(sDes) {}

		int Errno;
		std::string Desc;
	};
}

#endif
