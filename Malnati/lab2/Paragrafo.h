#ifndef __PARAGRAFO_H
#define __PARAGRAFO_H

#include "Testo.h"
#include <string>

class Paragrafo : public Testo {
private:
	const std::string testo;

public:
	Paragrafo(std::string testo);

	virtual const std::string& getTesto();

	virtual std::string trova(std::string s);
};

#endif