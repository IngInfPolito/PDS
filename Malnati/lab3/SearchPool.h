#ifndef __SEARCH_POOL_H
#define __SEARCH_POOL_H

#include "FileSearcher.h"
#include <Windows.h>
#include <tchar.h>

class SearchPool {
private:
	int maxSize, size;
	HANDLE* handles;
	ResultFile* rf;

	SearchPool(const SearchPool&);

public:
	SearchPool(ResultFile*);
	SearchPool(int size, ResultFile*);

	void addSearch(const TCHAR*, const TCHAR*);

	void waitForSearchCompletion(void);

	virtual ~SearchPool();
};

#endif
