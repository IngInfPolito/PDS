#include "ResultFile.h"
#include <system_error>

using namespace std;

// Mutex class for RAII idiom.
class Mutex {
private:
	HANDLE mutex;

public:
	Mutex(HANDLE mutex) {
		this->mutex = mutex;
		if (WaitForSingleObject(mutex, INFINITE) != WAIT_OBJECT_0)
			throw system_error(-1, system_category());
	}

	virtual ~Mutex() {
		if (!ReleaseMutex(mutex))
			throw system_error(-1, system_category());
	}
};

ResultFile::ResultFile(const TCHAR* fileName) {
	of = new tofstream(fileName);
	if (!of->is_open()) throw tofstream::failure(string("bad file"));
	mutex = CreateMutex(NULL, FALSE, NULL);
	if (mutex == NULL) throw system_error(-1, system_category());
}

void ResultFile::syncWrite(const TCHAR* s) {
	Mutex m(mutex);
	(*of) << s;
}

void ResultFile::syncWriteLine(const TCHAR* s) {
	Mutex m(mutex);
	(*of) << s << endl;
}

BOOL ResultFile::fail(void) {
	return of->fail();
}

ResultFile::~ResultFile() {
	of->close();
	delete of;
}
