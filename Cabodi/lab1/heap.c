#include "heap.h"
#include <stdlib.h>
#include <string.h>

// Size of blocks will be a multiple of ALIGNMENT (power of 2)
#define ALIGNMENT 8
#define ALIGN(x) (((x) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))

typedef struct node_s node_t;

struct node_s {
	void* start;
	size_t size;
	node_t* next;
};

struct heap_s {
	void* start;
	size_t size;
	size_t free_size;
	node_t* free_blks;
	node_t* used_blks;
	node_t* dfrg_blks;
	heap_policy_t policy;
};

void listDestroy(node_t*);
node_t* listCopy(node_t*);

/*
 * Creates a new heap
 * Returns a valid heap_t* if successful, NULL elsewhere
 */
heap_t* heapCreate(size_t heapSize) {
	// Allocating heap structure
	heap_t* newHeap = (heap_t*)malloc(sizeof(heap_t));
	if (newHeap == NULL) return NULL;

	// Allocating heap space
	newHeap->start = malloc(heapSize);
	if (newHeap->start == NULL) {
		free(newHeap);
		return NULL;
	}

	// Allocating free_blks first node and initialize other fields
	newHeap->size = heapSize;
	newHeap->free_size = heapSize;
	newHeap->free_blks = (node_t*)malloc(sizeof(node_t));
	if (newHeap->free_blks == NULL) {
		free(newHeap->start);
		free(newHeap);
		return NULL;
	}
	// Only one contiguous free block
	newHeap->free_blks->start = newHeap->start;
	newHeap->free_blks->size = heapSize;
	newHeap->free_blks->next = NULL;
	newHeap->used_blks = NULL; // No used blocks
	newHeap->dfrg_blks = NULL;
	newHeap->policy = first_fit; // Default policy

	return newHeap;
}

/*
 * Destroys space and data structures of heap h
 */
void heapDestroy(heap_t* h) {
	listDestroy(h->free_blks);
	listDestroy(h->used_blks);
	h->free_blks = NULL;
	h->used_blks = NULL;
	free(h->start);
	free(h);
}

/*
 * Destroys a list pointed by node, until the end of the list is reached
 */
void listDestroy(node_t* node) {
	node_t* tmpNode = NULL;
	while (node != NULL) {
		tmpNode = node->next;
		free(node);
		node = tmpNode;
	}
}

/*
 * Allocates a block of size bytes inside the heap h
 * Returns a pointer to the block if successful, NULL elsewhere
 */
void* heapAlloc(heap_t* h, size_t size) {
	node_t* node = h->free_blks;
	node_t* prev = NULL;
	node_t* selected = NULL;
	node_t* prevSelected;
	node_t* used;
	size_t selectedSize;

	if (size == 0) return NULL;
	size = ALIGN(size);
	if (h->free_size < size || h->free_blks == NULL) return NULL; // Block not available, searching is useless
	switch(h->policy) {
		case first_fit:
			while(node != NULL) {
				if (node->size >= size) {
					prevSelected = prev;
					selected = node;
					break;
				}
				prev = node;
				node = node->next;
			}
			break;
		case best_fit:
			selectedSize = -1; // Maximum value
			while (node != NULL) {
				if (node->size >= size && node->size < selectedSize) {
					prevSelected = prev;
					selectedSize = node->size;
					selected = node;
				}
				prev = node;
				node = node->next;
			}
			break;
		case worst_fit:
			selectedSize = 0; // Minimum value
			while (node != NULL) {
				if (node->size >= size && node->size > selectedSize) {
					prevSelected = prev;
					selectedSize = node->size;
					selected = node;
				}
				prev = node;
				node = node->next;
			}
			break;
	}
	if (selected == NULL) return NULL; // Not found
	if (selected->size == size) { // Use same node for used list
		if (prevSelected == NULL) {
			h->free_blks = selected->next;
		} else {
			prevSelected->next = selected->next;
		}
		used = selected;
	} else { // Create new node for used list and update free node
		used = (node_t*)malloc(sizeof(node_t));
		used->start = selected->start;
		used->size = size;
		selected->start += size;
		selected->size -= size;
	}
	// Insert node into used list
	node = h->used_blks;
	prev = NULL;
	while (node != NULL && node->start < used->start) {
		prev = node;
		node = node->next;
	}
	// Insert between prev and node
	used->next = node;
	if (prev == NULL) {
		h->used_blks = used;
	} else {
		prev->next = used;
	}
	h->free_size -= used->size;
	return used->start;
}

/*
 * Free a block of heap h which starts on address p
 * If p is not valid nor it points to a valid block, does nothing
 */
void heapFree(heap_t* h, void* p) {
	if (p == NULL) return;
	node_t* node = h->used_blks;
	node_t* prev = NULL;
	node_t* freeNode;
	
	// Search for block in used list
	while (node != NULL && node->start != p) {
		prev = node;
		node = node->next;
	}
	if (node == NULL) return;
	// Remove node from used list
	if (prev == NULL) {
		h->used_blks = node->next;
	} else {
		prev->next = node->next;
	}
	// Insert node into free list
	freeNode = node;
	node = h->free_blks;
	prev = NULL;
	while (node != NULL && node->start < freeNode->start) {
		prev = node;
		node = node->next;
	}
	// Insert between prev and node
	freeNode->next = node;
	if (prev == NULL) {
		h->free_blks = freeNode;
	} else {
		prev->next = freeNode;
	}
	h->free_size += freeNode->size;
	// Compact free list
	if (prev != NULL && (freeNode->start == prev->start + prev->size)) {
		prev->size += freeNode->size;
		prev->next = freeNode->next;
		free(freeNode);
		freeNode = prev;
	}
	if (node != NULL && (node->start == freeNode->start + freeNode->size)) {
		freeNode->size += node->size;
		freeNode->next = node->next;
		free(node);
	}
}

void heapSetPolicy(heap_t* h, heap_policy_t policy) {
	h->policy = policy;
}

void heapDefrag(heap_t* h) {
	node_t* node;
	void* prev;
	if (h->used_blks == NULL) return;
	// Copy and adjust used list moving blocks in memory
	node = h->used_blks;
	prev = h->start;
	h->dfrg_blks = listCopy(h->used_blks);
	while (node != NULL) {
		if (prev != node->start) {
			node->start = memmove(prev, node->start, node->size);
		}
		prev = node->start + node->size;
		node = node->next;
	}
	// Adjust free list (one block at the end of the heap, if any)
	if (h->free_blks != NULL) {
		h->free_blks->start = h->start + h->size - h->free_size;
		h->free_blks->size = h->free_size;
		listDestroy(h->free_blks->next);
		h->free_blks->next = NULL;
	}
}

node_t* listCopy(node_t* list) {
	node_t* copy = NULL;
	node_t* node;
	while (list != NULL) {
		if (copy == NULL) {
			copy = (node_t*)malloc(sizeof(node_t));
			node = copy;
		} else {
			node->next = (node_t*)malloc(sizeof(node_t));
			node = node->next;
		}
		node->start = list->start;
		node->size = list->size;
		node->next = NULL;
		list = list->next;
	}
	return copy;
}

void* heapNewPointer(heap_t* h, void* p) {
	node_t* node = h->dfrg_blks;
	node_t* newNode = h->used_blks;
	while(node != NULL) {
		if (node->start == p) return newNode->start;
		node = node->next;
		newNode = newNode->next;
	}
	return NULL;
}

void heapDefragClose(heap_t* h) {
	listDestroy(h->dfrg_blks);
	h->dfrg_blks = NULL;
}
