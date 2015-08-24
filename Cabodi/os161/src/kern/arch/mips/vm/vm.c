/*
 * Copyright (c) 2000, 2001, 2002, 2003, 2004, 2005, 2008, 2009
 *	The President and Fellows of Harvard College.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE UNIVERSITY AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE UNIVERSITY OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <types.h>
#include <kern/errno.h>
#include <lib.h>
#include <spl.h>
#include <spinlock.h>
#include <thread.h>
#include <current.h>
#include <mips/tlb.h>
#include <addrspace.h>
#include <vm.h>

static memory_t mem;
node_t* vm_space;

/*
 * Wrap ram_stealmem in a spinlock.
 */
static struct spinlock stealmem_lock = SPINLOCK_INITIALIZER;

static paddr_t getppages(unsigned long npages) {
	paddr_t addr;

	spinlock_acquire(&stealmem_lock);

	addr = ram_stealmem(npages);
	
	spinlock_release(&stealmem_lock);
	return addr;
}

/* 
 * Nodes are allocated into the reserved vm_space.
 * This space is viewed as a raw array of node "containers",
 * each one of size equal to sizeof(node_t).
 *
 * A node container is free iff the address contained is 0, otherwise it is allocated.
 */
static node_t* allocNode(paddr_t addr, size_t pages) {
	size_t i;
	node_t* n;

	if (addr == 0) return NULL;
	
	for (i = 0; i < MAX_NODES; i++) {
		n = vm_space + i;
		if (n->addr == 0) {
			n->addr = addr; // Allocate
			n->pages = pages;
			n->next = NULL;
			return n;
		}
	}

	return NULL; // No more space
}

static void freeNode(paddr_t addr) {
	size_t i;
	node_t* n;

	if (addr == 0) return;
	
	for (i = 0; i < MAX_NODES; i++) {
		n = vm_space + i;
		if (n->addr == addr) {
			n->addr = 0; // Deallocate
			n->pages = 0;
			n->next = NULL;
			return;
		}
	}

	return; // Node not found
}

void vm_bootstrap(void) {
	/*
	 * Initialize VM structure.
	 */
	vm_space = (void*)PADDR_TO_KVADDR(getppages(VM_PAGES));
	bzero(vm_space, VM_PAGES * PAGE_SIZE);

	ram_getsize(&mem.start, &mem.end);
	mem.pages = (mem.end - mem.start) / PAGE_SIZE;
	mem.free_pages = mem.pages;
	mem.free_blks = allocNode(mem.start, mem.pages);
	if (mem.free_blks == NULL) {
		panic("Not enough memory for VM system!\n");
	}
	mem.used_blks = NULL;
	mem.policy = first_fit;
}

/* Allocate/free physical pages */
paddr_t alloc_ppages(int npages) {
	node_t* node = mem.free_blks;
	node_t* prev = NULL;
	node_t* selected = NULL;
	node_t* prevSelected;
	node_t* used;
	size_t selectedPages;

	if (npages == 0) return 0;
	if (mem.free_pages < npages || mem.free_blks == NULL) return 0; // Block not available
	switch (mem.policy) {
		case first_fit:
			while (node != NULL) {
				if (node->pages >= npages) {
					prevSelected = prev;
					selected = node;
					break;
				}
				prev = node;
				node = node->next;
			}
			break;
		case best_fit:
			selectedPages = -1; // Maximum value
			while (node != NULL) {
				if (node->pages >= npages && node->pages < selectedPages) {
					prevSelected = prev;
					selectedPages = node->pages;
					selected = node;
				}
				prev = node;
				node = node->next;
			}
			break;
		case worst_fit:
			selectedPages = 0; // Minimum value
			while (node != NULL) {
				if (node->pages >= npages && node->pages > selectedPages) {
					prevSelected = prev;
					selectedPages = node->pages;
					selected = node;
				}
				prev = node;
				node = node->next;
			}
			break;
	}
	if (selected == NULL) return 0; // Not found
	if (selected->pages == npages) { // Use same node for used list
		if (prevSelected == NULL) {
			mem.free_blks = selected->next;
		} else {
			prevSelected->next = selected->next;
		}
		used = selected;
	} else { // Create new node for used list and update free node
		used = allocNode(selected->addr, npages);
		if (used == NULL) panic("Not enough memory for VM system!\n");
		selected->addr += npages * PAGE_SIZE;
		selected->pages -= npages;
	}
	// Insert node into used list
	node = mem.used_blks;
	prev = NULL;
	while (node != NULL && node->addr < used->addr) {
		prev = node;
		node = node->next;
	}
	// Insert between prev and node
	used->next = node;
	if (prev == NULL) {
		mem.used_blks = used;
	} else {
		prev->next = used;
	}
	mem.free_pages -= used->pages;
	return used->addr;
}

void free_ppages(paddr_t addr) {
	if (addr == 0) return;

	node_t* node = mem.used_blks;
	node_t* prev = NULL;
	node_t* free_n;
	
	// Search for block in used list
	while (node != NULL && node->addr != addr) {
		prev = node;
		node = node->next;
	}
	if (node == NULL) return;

	// Remove node from used list
	if (prev == NULL) {
		mem.used_blks = node->next;
	} else {
		prev->next = node->next;
	}
	// Insert node into free list
	free_n = node;
	node = mem.free_blks;
	prev = NULL;
	while (node != NULL && node->addr < free_n->addr) {
		prev = node;
		node = node->next;
	}
	// Insert between prev and node
	free_n->next = node;
	if (prev == NULL) {
		mem.free_blks = free_n;
	} else {
		prev->next = free_n;
	}
	mem.free_pages += free_n->pages;

	// Compact free list
	if (prev != NULL && (free_n->addr == prev->addr + prev->pages * PAGE_SIZE)) {
		// Join prev with free_n
		prev->pages += free_n->pages;
		prev->next = free_n->next;
		freeNode(free_n);
		free_n = prev;
	}
	if (node != NULL && (node->addr == free_n->addr + free_n->pages * PAGE_SIZE)) {
		// Join free_n with node
		free_n->pages += node->pages;
		free_n->next = node->next;
		freeNode(node);
	}
}

/* Allocate/free some kernel-space virtual pages */
vaddr_t alloc_kpages(int npages) {
	return PADDR_TO_KVADDR(alloc_ppages(npages));
}

void free_kpages(vaddr_t vaddr) {
	free_ppages(KVADDR_TO_PADDR(vaddr));
}

void vm_tlbshootdown_all(void) {
	panic("Tried to do tlb shootdown?!\n");
}

void vm_tlbshootdown(const struct tlbshootdown *ts) {
	(void)ts;
	panic("Tried to do tlb shootdown?!\n");
}

int vm_fault(int faulttype, vaddr_t faultaddress) {
	vaddr_t vbase1, vtop1, vbase2, vtop2, stackbase, stacktop;
	paddr_t paddr;
	int i;
	uint32_t ehi, elo;
	struct addrspace *as;
	int spl;

	faultaddress &= PAGE_FRAME;

	DEBUG(DB_VM, "dumbvm: fault: 0x%x\n", faultaddress);

	switch (faulttype) {
	    case VM_FAULT_READONLY:
		/* We always create pages read-write, so we can't get this */
		panic("dumbvm: got VM_FAULT_READONLY\n");
	    case VM_FAULT_READ:
	    case VM_FAULT_WRITE:
		break;
	    default:
		return EINVAL;
	}

	as = curthread->t_addrspace;
	if (as == NULL) {
		/*
		 * No address space set up. This is probably a kernel
		 * fault early in boot. Return EFAULT so as to panic
		 * instead of getting into an infinite faulting loop.
		 */
		return EFAULT;
	}

	/* Assert that the address space has been set up properly. */
	KASSERT(as->as_vbase1 != 0);
	KASSERT(as->as_pbase1 != 0);
	KASSERT(as->as_npages1 != 0);
	KASSERT(as->as_vbase2 != 0);
	KASSERT(as->as_pbase2 != 0);
	KASSERT(as->as_npages2 != 0);
	KASSERT(as->as_stackpbase != 0);
	KASSERT((as->as_vbase1 & PAGE_FRAME) == as->as_vbase1);
	KASSERT((as->as_pbase1 & PAGE_FRAME) == as->as_pbase1);
	KASSERT((as->as_vbase2 & PAGE_FRAME) == as->as_vbase2);
	KASSERT((as->as_pbase2 & PAGE_FRAME) == as->as_pbase2);
	KASSERT((as->as_stackpbase & PAGE_FRAME) == as->as_stackpbase);

	vbase1 = as->as_vbase1;
	vtop1 = vbase1 + as->as_npages1 * PAGE_SIZE;
	vbase2 = as->as_vbase2;
	vtop2 = vbase2 + as->as_npages2 * PAGE_SIZE;
	stackbase = USERSTACK - VM_STACKPAGES * PAGE_SIZE;
	stacktop = USERSTACK;

	if (faultaddress >= vbase1 && faultaddress < vtop1) {
		paddr = (faultaddress - vbase1) + as->as_pbase1;
	}
	else if (faultaddress >= vbase2 && faultaddress < vtop2) {
		paddr = (faultaddress - vbase2) + as->as_pbase2;
	}
	else if (faultaddress >= stackbase && faultaddress < stacktop) {
		paddr = (faultaddress - stackbase) + as->as_stackpbase;
	}
	else {
		return EFAULT;
	}

	/* make sure it's page-aligned */
	KASSERT((paddr & PAGE_FRAME) == paddr);

	/* Disable interrupts on this CPU while frobbing the TLB. */
	spl = splhigh();

	for (i=0; i<NUM_TLB; i++) {
		tlb_read(&ehi, &elo, i);
		if (elo & TLBLO_VALID) {
			continue;
		}
		ehi = faultaddress;
		elo = paddr | TLBLO_DIRTY | TLBLO_VALID;
		DEBUG(DB_VM, "dumbvm: 0x%x -> 0x%x\n", faultaddress, paddr);
		tlb_write(ehi, elo, i);
		splx(spl);
		return 0;
	}

	kprintf("dumbvm: Ran out of TLB entries - cannot handle page fault\n");
	splx(spl);
	return EFAULT;
}

