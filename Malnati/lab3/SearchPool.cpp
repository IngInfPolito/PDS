#include "SearchPool.h"
#include <stdexcept>
#include <system_error>

using namespace std;

SearchPool::SearchPool(ResultFile* rf) : SearchPool::SearchPool(10, rf) { }

SearchPool::SearchPool(int maxSize, ResultFile* rf) : maxSize(maxSize), rf(rf), size(0) {
	int i;
	if (maxSize <= 0) throw invalid_argument("maxSize must be > 0");
	handles = new HANDLE[maxSize];
	for (i = 0; i < maxSize; i++) handles[i] = NULL;
}

void SearchPool::addSearch(const TCHAR* fileName, const TCHAR* text) {
	if (size == maxSize) {
		// Maximum number of threads reached, wait for one to finish.
		DWORD h = WaitForMultipleObjects(size, handles, FALSE, INFINITE);
		if (h < WAIT_OBJECT_0 || h >= WAIT_OBJECT_0 + size) throw system_error(-1, system_category());
		h -= WAIT_OBJECT_0;
		// Thread 'h' has finished, we can delete it.
		if (!CloseHandle(handles[h])) throw system_error(-1, system_category());
		handles[h] = NULL;
		handles[h] = handles[--size];
	}
	// There is a free position pointed by 'size'
	HANDLE h = CreateFileSearcher(fileName, text, rf);
	if (h == NULL) throw system_error(-1, system_category());
	handles[size++] = h;
}

void SearchPool::waitForSearchCompletion(void) {
	DWORD h = WaitForMultipleObjects(size, handles, TRUE, INFINITE);
	if (h < WAIT_OBJECT_0 || h >= WAIT_OBJECT_0 + size) throw system_error(-1, system_category());
}

SearchPool::~SearchPool() {
	while (size > 0) {
		CloseHandle(handles[--size]);
		handles[size] = NULL;
	}
	delete[] handles;
}
