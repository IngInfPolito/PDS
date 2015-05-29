#include "Contenitore.h"

Contenitore::Contenitore() : elenco(nullptr), coda(nullptr) { }

void Contenitore::add(Testo* t) {
	Tnode* tn = new Tnode;
	tn->t = t;
	tn->next = nullptr;
	if (elenco == nullptr) {
		elenco = coda = tn;
	} else {
		coda->next = tn;
		coda = tn;
	}
}

std::string Contenitore::trova(std::string s) {
	Tnode* tn = elenco;
	unsigned int pos = 1;
	while (tn != nullptr) {
		std::string res = tn->t->trova(s);
		if (!res.empty()) return "/" + std::to_string(pos) + res;
		tn = tn->next;
		pos++;
	}
	return std::string("");
}

Contenitore::~Contenitore() {
	Tnode* tn = elenco;
	Tnode* tmp;
	while (tn != nullptr) {
		delete tn->t;
		tmp = tn;
		tn = tn->next;
		delete tmp;
	}
}
