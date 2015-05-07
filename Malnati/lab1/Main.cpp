#include <tchar.h>
#include <iostream>

#include "Persona.h"
#include "Elenco.h"

using namespace std;

int main(int argc, _TCHAR* argv[]) {
	Elenco e1;
	e1.add(Persona("a", "b"));
	e1.add(Persona("c", "d"));
	e1.add(Persona("e", "f"));
	Elenco e2 = e1;
	e1.remove(1);
	e2.remove(2);
	for (int i = 0; i<e1.size(); i++)
		cout << i << ": " << e1.get(i).getNome() << " " << e1.get(i).getCognome() << endl;
	cout << endl;
	for (int i = 0; i<e2.size(); i++)
		cout << i << ": " << e2.get(i).getNome() << " " << e2.get(i).getCognome() << endl;
	cout << endl;
	e1.clear();
	cout << e1.size() << endl;

	return 0;
}