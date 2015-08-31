#ifndef __UNICODE_TYPES_H
#define __UNICODE_TYPES_H

#include <string>
#include <fstream>

namespace std {
#ifdef _UNICODE
	typedef wstring tstring;
#else
	typedef string tstring;
#endif

#ifdef _UNICODE
	typedef wifstream tifstream;
#else
	typedef ifstream tifstream;
#endif

#ifdef _UNICODE
	typedef wofstream tofstream;
#else
	typedef ofstream tofstream;
#endif
};

#endif
