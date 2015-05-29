#include "Capitolo.h"
#include "Libro.h"
#include "bad_hierarchy.h"

Contenitore* Capitolo::aggiungi(Testo* t) {
	Libro* l = dynamic_cast<Libro*>(t);
	if (l != nullptr) throw bad_hierarchy();
	add(t);
	return this;
}