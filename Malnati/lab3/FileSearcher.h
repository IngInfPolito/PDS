#ifndef __FILE_SEARCHER_H
#define __FILE_SEARCHER_H

#include "ResultFile.h"
#include <Windows.h>
#include <string>
#include <tchar.h>

#define TEXT_NOT_FOUND 0
#define TEXT_FOUND 1

HANDLE CreateFileSearcher(const TCHAR*, const TCHAR*, ResultFile*);

#endif
