#include "FileSearcher.h"

using namespace std;

typedef struct param_s {
	const tstring* fileName;
	const tstring* text;
	ResultFile* rf;
} thr_param;

DWORD WINAPI doSearch(LPVOID p);
void freeMemory(thr_param* params);

HANDLE CreateFileSearcher(const TCHAR* fileName, const TCHAR* text, ResultFile* rf) {
	thr_param* params = new thr_param;
	params->fileName = new tstring(fileName);
	params->text = new tstring(text);
	params->rf = rf;
	return CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)doSearch, (LPVOID)params, 0, NULL);
}

DWORD WINAPI doSearch(LPVOID p) {
	thr_param* params = (thr_param*)p;
	tstring line;
	tifstream in(*params->fileName);
	if (in.is_open()) {
		while (getline(in, line)) {
			if (line.find(*params->text) != string::npos) {
				params->rf->syncWriteLine(params->fileName->c_str());
				in.close();
				freeMemory(params);
				params = nullptr;
				return TEXT_FOUND;
			}
		}
		in.close();
	}
	freeMemory(params);
	params = nullptr;
	return TEXT_NOT_FOUND;
}

void freeMemory(thr_param* params) {
	delete params->fileName;
	delete params->text;
	delete params;
}
