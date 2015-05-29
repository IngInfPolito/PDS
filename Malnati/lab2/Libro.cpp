#include "Libro.h"
#include "Capitolo.h"
#include "bad_hierarchy.h"

Libro::Libro(char* titolo) : titolo(titolo) { }
Libro::Libro(std::string titolo) : titolo(titolo) { }

const std::string& Libro::getTitolo() {
	return titolo;
}

Contenitore* Libro::aggiungi(Testo* t) {
	Capitolo* c = dynamic_cast<Capitolo*>(t);
	if (c == nullptr) throw bad_hierarchy();
	add(t);
	return this;
}