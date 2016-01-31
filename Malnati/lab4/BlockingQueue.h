#ifndef __BLOCKING_QUEUE_H
#define __BLOCKING_QUEUE_H

#include <mutex>
#include <condition_variable>

template <class T> class BlockingQueue {
private:
	T* queue;
	unsigned int in, out;
	int size;
	int capacity;
	bool open;
	std::mutex m;
	std::condition_variable c_vuoto;
	std::condition_variable c_pieno;

	bool vuoto();
	bool pieno();

public:
	BlockingQueue(int);

	bool preleva(T&);

	void inserisci(T);

	int conteggio();

	void chiudi();

	~BlockingQueue();
};

// Needed for template class
#include "BlockingQueue.hpp"

#endif
