#pragma once
#include "slab.h"
#include <stdio.h>
#include <string.h>
#define CRT_SECURE_NO_WARNINGS
void kmem_init(void* space, int block_num) {
	num_of_blocks = block_num;
	startAdr = space;
	head = 0;
	tail = 0;
}

kmem_cache_t* kmem_cache_create(const char* name, size_t size,
	void (*ctor)(void*), void (*dtor)(void*)) {

	size_t startPom = 0;
	kmem_cache_t* curr;
	if (!head) {
		curr = (char*)startAdr + startPom;
		head = curr;
		tail = head;
	} else {
		curr = (char*)tail + (((kmem_cache_t*)tail)->header.num_of_slabs * BLOCK_SIZE);
		((kmem_cache_t*)tail)->header.next = tail = curr;
	}
	curr->header.cache_name = name;
	curr->header.cache_size = size;
	curr->header.cache_ctor = ctor;
	curr->header.cache_dtor = dtor;
	curr->header.next = NULL;
	curr->header.num_of_slabs = 2;
	curr->head = 0;
	curr->tail = 0;
	curr->num_of_obj = 0;
	return curr;
}

int kmem_cache_shrink(kmem_cache_t* cachep) { return 0; }

void* kmem_cache_alloc(kmem_cache_t* cachep) {
	if (!cachep)
		return NULL;
	cachep->num_of_obj++;
	kmem_cache_t_obj* ret = NULL;
	if (!cachep->head) {
		cachep->head = (char*)cachep + BLOCK_SIZE;
		ret = cachep->head;
		ret->next = NULL;
		cachep->tail = ret;
	} else {
		kmem_cache_t_obj* last = cachep->tail;
		ret = (char*)last + cachep->header.cache_size + sizeof(void*);
		last->next = cachep->tail = ret;
	}
	ret = ret + sizeof(void*);
	void (*ctor)(void*) = cachep->header.cache_ctor;
	ctor(ret);
	return ret;
}

void kmem_cache_free(kmem_cache_t* cachep, void* objp) {}

void* kmalloc(size_t size) {

}

void kfree(const void* objp) {}

void kmem_cache_destroy(kmem_cache_t* cachep) {}

void kmem_cache_info(kmem_cache_t* cachep) {
	kmem_cache_t_header* h = &cachep->header;
	printf("ime: %s\nvelicine: %d\n", h->cache_name, h->cache_size);
}

int kmem_cache_error(kmem_cache_t* cachep) { return 0; }
