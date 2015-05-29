#ifndef __TESTO_H
#define __TESTO_H

#include <string>

class Testo {
private:
	Testo(const Testo& t);
	Testo& operator=(const Testo& t);

public:
	Testo() { }

	virtual std::string trova(std::string) = 0;

	virtual ~Testo() { }
};

#endif