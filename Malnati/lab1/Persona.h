#ifndef __PERSONA_H
#define __PERSONA_H

class Persona {
private:
	char* nome;
	char* cognome;

	void copyName(const char* nome, const char* cognome);

public:
	Persona();
	Persona(const char* nome, const char* cognome);
	Persona(const Persona& p);

	const char* getNome();
	const char* getCognome();

	Persona& operator=(const Persona& p);

	~Persona();
};

#endif