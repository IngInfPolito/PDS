#ifndef __CONTENITORE_H
#define __CONTENITORE_H

#include "Testo.h"

class Contenitore :	public Testo {
private:
	typedef struct node {
		Testo* t;
		struct node* next;
	} Tnode;

	Tnode* elenco;
	Tnode* coda;

protected:
	void add(Testo* t);

public:
	Contenitore();

	virtual std::string trova(std::string s);
	virtual Contenitore* aggiungi(Testo*) = 0;

	virtual ~Contenitore();
};

#endif
