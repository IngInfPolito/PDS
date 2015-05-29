#ifndef __LIBRO_H
#define __LIBRO_H

#include "Contenitore.h"
#include <string>

class Libro : public Contenitore {
private:
	const std::string titolo;

public:
	Libro(char* titolo);
	Libro(std::string titolo);

	virtual const std::string& getTitolo();

	virtual Contenitore* aggiungi(Testo* t);

};

#endif