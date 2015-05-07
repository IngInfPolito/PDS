#include <stdexcept>

#include "Elenco.h"

Elenco::Elenco() : head(nullptr), tail(nullptr), s(0) { }

Elenco::Elenco(const Elenco& e) : head(nullptr), tail(nullptr), s(0) {
	copyList(e.head);
}

void Elenco::copyList(const Pnode* pn) {
	Pnode* pncopy;
	while (pn != nullptr) {
		if (head == nullptr) {
			head = new Pnode;
			pncopy = head;
		} else {
			pncopy->next = new Pnode;
			pncopy = pncopy->next;
		}
		pncopy->p = pn->p;
		pncopy->next = nullptr;
		pn = pn->next;
		s++;
	}
	tail = pncopy;
}

int Elenco::size() {
	return s;
}

void Elenco::add(Persona p) {
	if (head == nullptr) {
		head = new Pnode;
		tail = head;
	} else {
		tail->next = new Pnode;
		tail = tail->next;
	}
	tail->p = p;
	tail->next = nullptr;
	s++;
}

Persona Elenco::get(int pos) {
	if (pos < 0 || pos >= s) throw std::out_of_range("pos out of range");
	int i = 0;
	Pnode* pn = head;
	while (i < pos) {
		pn = pn->next;
		i++;
	}
	return pn->p;
}

Persona Elenco::remove(int pos) {
	if (pos < 0 || pos >= s) throw std::out_of_range("pos out of range");
	int i = 0;
	Pnode* pn = head;
	Pnode* prev = nullptr;
	while (i < pos) {
		prev = pn;
		pn = pn->next;
		i++;
	}
	Persona p = pn->p;
	if (prev == nullptr) {
		head->next = pn->next;
	} else {
		prev->next = pn->next;
	}
	if (pn == tail) tail = prev;
	delete pn;
	s--;
	return p;
}

void Elenco::clear() {
	Pnode* tmp;
	while (head != nullptr) {
		tmp = head;
		head = head->next;
		delete tmp;
	}
	tail = nullptr;
	s = 0;
}

Elenco& Elenco::operator=(const Elenco& e) {
	clear();
	copyList(e.head);
	return *this;
}

Elenco::~Elenco() {
	clear();
}