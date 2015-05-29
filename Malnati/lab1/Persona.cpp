#include "Persona.h"
#include <string.h>
#include <algorithm>

Persona::Persona() : nome(nullptr), cognome(nullptr) { }

Persona::Persona(const char* nome, const char* cognome) {
	copyName(nome, cognome);
}

Persona::Persona(const Persona& p) {
	copyName(p.nome, p.cognome);
}

void Persona::copyName(const char* nome, const char* cognome) {
	int ln = strlen(nome), lc = strlen(cognome);
	this->nome = new char[ln + 1];
	this->cognome = new char[lc + 1];
	std::copy(nome, nome + ln + 1, this->nome);
	std::copy(cognome, cognome + lc + 1, this->cognome);
}

const char* Persona::getNome() {
	return nome;
}

const char* Persona::getCognome() {
	return cognome;
}

Persona& Persona::operator=(const Persona& p) {
	delete[] nome;
	delete[] cognome;
	copyName(p.nome, p.cognome);
	return *this;
}

Persona::~Persona() {
	delete[] nome;
	delete[] cognome;
}
