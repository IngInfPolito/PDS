#ifndef __FGREP_H
#define __FGREP_H

#include "SearchPool.h"

class FGrep {
private:
	SearchPool* sp;
	int execute(const TCHAR*, const TCHAR*, int);

public:
	FGrep(int, ResultFile*);

	int execute(const TCHAR*, const TCHAR*);

	virtual ~FGrep();
};

#endif
