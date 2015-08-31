#include "FGrep.h"
#include <strsafe.h>
#include <system_error>

using namespace std;

FGrep::FGrep(int nThreads, ResultFile* rf) {
	sp = new SearchPool(nThreads, rf);
}

int FGrep::execute(const TCHAR* dirName, const TCHAR* text, int level) {
	TCHAR dir[MAX_PATH];
	size_t dirLen;
	WIN32_FIND_DATA d;
	StringCchLength(dirName, MAX_PATH, &dirLen);
	if (dirLen > MAX_PATH - 3) return -1;
	StringCchCopy(dir, MAX_PATH, dirName);
	StringCchCat(dir, MAX_PATH, TEXT("\\*"));
	HANDLE h = FindFirstFile(dir, &d);
	if (h == INVALID_HANDLE_VALUE) return -1;
	do {
		if (_tcscmp(d.cFileName, TEXT(".")) == 0 ||
			_tcscmp(d.cFileName, TEXT("..")) == 0) continue; // Skip '.' and '..' directories
		TCHAR file[MAX_PATH];
		StringCchCopy(file, MAX_PATH, dirName);
		StringCchCat(file, MAX_PATH, TEXT("\\"));
		StringCchCat(file, MAX_PATH, d.cFileName);
		if (d.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			// Recursion on subdirectory
			if (this->execute(file, text)) return -1;
		} else {
			try {
				sp->addSearch(file, text);
			} catch (...) {
				return -1;
			}
		}
	} while (FindNextFile(h, &d));
	if (GetLastError() != ERROR_NO_MORE_FILES) return -1;
	if (level == 0) sp->waitForSearchCompletion();
	return 0;
}

int FGrep::execute(const TCHAR* dirName, const TCHAR* text) {
	return this->execute(dirName, text, 0);
}

FGrep::~FGrep() {
	delete sp;
}
