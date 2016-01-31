#include "BlockingQueue.h"

using namespace std;

template <class T> BlockingQueue<T>::BlockingQueue(int capacity) : capacity(capacity) {
	if (capacity <= 0) throw domain_error("capacity must be greater than zero");
	queue = new T[capacity];
	open = true;
	size = 0;
	in = 0;
	out = 0;
}

template <class T> bool BlockingQueue<T>::vuoto() {
	return size == 0;
}

template <class T> bool BlockingQueue<T>::pieno() {
	return size == capacity;
}

template <class T> bool BlockingQueue<T>::preleva(T& res) {
	if (open == false) return false; // Only for performance reasons
	unique_lock<mutex> lock(m);
	while (open == true && vuoto()) {
		c_vuoto.wait(lock);
	}
	if (open == false) return false;
	res = queue[out];
	out = (out + 1) % capacity;
	size--;
	c_pieno.notify_one();
	return true;
}

template <class T> void BlockingQueue<T>::inserisci(T val) {
	unique_lock<mutex> lock(m);
	while (open == true && pieno()) {
		c_pieno.wait(lock);
	}
	if (open == false) throw logic_error("coda chiusa");
	queue[in] = val;
	in = (in + 1) % capacity;
	size++;
	c_vuoto.notify_one();
}

template <class T> int BlockingQueue<T>::conteggio() {
	unique_lock<mutex> lock(m);
	return size;
}

template <class T> void BlockingQueue<T>::chiudi() {
	unique_lock<mutex> lock(m);
	open = false;
	c_vuoto.notify_all();
	c_pieno.notify_all();
}

template <class T> BlockingQueue<T>::~BlockingQueue() {
	delete[] queue;
}
