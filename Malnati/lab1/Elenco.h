#ifndef __ELENCO_H
#define __ELENCO_H

#include "Persona.h"

class Elenco {
private:
	typedef struct node {
		Persona p;
		struct node* next;
	} Pnode;

	Pnode* head;
	Pnode* tail;
	int s;

	void copyList(const Pnode* pn);

public:
	Elenco();
	Elenco(const Elenco& e);

	int size();
	void add(Persona p);
	Persona get(int pos);
	Persona remove(int pos);
	void clear();

	Elenco& operator=(const Elenco& e);

	~Elenco();
};

#endif