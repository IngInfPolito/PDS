#ifndef __RESULT_FILE_H
#define __RESULT_FILE_H

#include "UnicodeTypes.h"
#include <Windows.h>
#include <tchar.h>

class ResultFile {
private:
	std::tofstream* of;
	HANDLE mutex;

public:
	ResultFile(const TCHAR*);

	void syncWrite(const TCHAR*);

	void syncWriteLine(const TCHAR*);

	BOOL fail(void);

	virtual ~ResultFile();
};

#endif
