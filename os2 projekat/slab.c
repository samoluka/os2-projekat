#pragma once
#include "slab.h"
#include <stdio.h>
#include <string.h>
#define CRT_SECURE_NO_WARNINGS
void kmem_init(void* space, int block_num) {
	num_of_blocks = block_num;
	startAdr = space;
}

kmem_cache_t* kmem_cache_create(const char* name, size_t size,
	void (*ctor)(void*), void (*dtor)(void*)) {

	unsigned int startPom = 0;
	kmem_cache_t* curr = (char*)startAdr + startPom;
	curr->header.cache_name = name;
	curr->header.cache_size = size;
	curr->header.cache_ctor = ctor;
	curr->header.cache_dtor = dtor;
	return curr;
}

int kmem_cache_shrink(kmem_cache_t* cachep) { return 0; }

void* kmem_cache_alloc(kmem_cache_t* cachep) { return NULL; }

void kmem_cache_free(kmem_cache_t* cachep, void* objp) {}

void* kmalloc(size_t size) { return NULL; }

void kfree(const void* objp) {}

void kmem_cache_destroy(kmem_cache_t* cachep) {}

void kmem_cache_info(kmem_cache_t* cachep) {
	kmem_cache_t_header* h = &cachep->header;
	printf("ime: %s\nvelicine: %d", h->cache_name, h->cache_size);
}

int kmem_cache_error(kmem_cache_t* cachep) { return 0; }
