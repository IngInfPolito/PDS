#include <tchar.h>
#include <iostream>

#include "Libro.h"
#include "Capitolo.h"
#include "Paragrafo.h"
#include "bad_hierarchy.h"

using namespace std;

int main(int argc, _TCHAR* argv[]) {
	Libro* l = new Libro("l");
	Capitolo* c1 = new Capitolo();
	Capitolo* c2 = new Capitolo();
	Paragrafo* alfa = new Paragrafo("alfa");
	Paragrafo* beta = new Paragrafo("beta");
	Paragrafo* gamma = new Paragrafo("gamma");
	Paragrafo* delta = new Paragrafo("delta");
	l->aggiungi(c1)->aggiungi(c2);
	c1->aggiungi(alfa)->aggiungi(beta);
	c2->aggiungi(gamma)->aggiungi(delta);
	cout << "String \"eta\": " << l->trova(std::string("eta")) << endl;
	cout << "String \"ma\": " << l->trova(std::string("ma")) << endl;
	delete l;
	return 0;
}