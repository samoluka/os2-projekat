#pragma once
#include "slab.h"
#include <stdio.h>
#include <string.h>
#include "buddy.h"
#include <math.h>
#define CRT_SECURE_NO_WARNINGS

typedef unsigned int uint;
typedef unsigned char uint8;

#define log2_block_size (12)

int num_of_blocks;
void* startAdr;
//void* head;
//void* tail;
kmem_cache_t* caches;

typedef struct kmem_cache_t_slab {
	kmem_cache_t_slab* next;
	kmem_cache_t_slab* prev;
	uint curr_objects;
} kmem_cache_t_slab;

typedef struct kmem_cache_t_lists {
	kmem_cache_t_slab* free, * half, * full;
}kmem_cache_t_lists;

typedef struct kmem_cache_t_object_info {
	size_t cache_size;
	void* cache_ctor;
	void* cache_dtor;
}kmem_cache_t_object_info;

typedef struct kmem_cache_t_header {
	const char* cache_name;
	kmem_cache_t_object_info obj_info;
	uint slab_block_size;
	uint num_of_slabs;
	uint obj_per_slab;
}kmem_cache_t_header;
typedef struct kmem_cache_s {
	kmem_cache_t_header header;
	kmem_cache_t_lists heads, tails;
	uint num_of_obj;
} kmem_cache_t;

uint calculate_blocks_need(size_t space_needed) {
	return 1;
}
void add(kmem_cache_t_slab* head, kmem_cache_t_slab* tail, kmem_cache_t_slab* elem) {
	if (tail) {
		elem->prev = tail;
		tail = tail->next = elem;
	} else {
		elem->prev = elem->next = elem;
		head = tail = elem;
	}
}
kmem_cache_t_slab* remove(kmem_cache_t_slab* head, kmem_cache_t_slab* tail) {
	if (!head || !tail)
		return NULL;
	kmem_cache_t_slab* ret;
	if (head->prev == head) {
		ret = head;
		head = tail = NULL;
	} else {
		ret = tail;
		tail = head->prev = tail->prev;
		tail->next = head;
	}
	ret->next = NULL;
	ret->prev = NULL;
	return ret;
}
void init_start_cache(kmem_cache_t* cache) {
	cache->header.obj_info.cache_ctor = NULL;
	cache->header.obj_info.cache_dtor = NULL;
	cache->header.obj_info.cache_size = sizeof(kmem_cache_t);
	cache->header.num_of_slabs = 0;
	cache->header.slab_block_size = calculate_blocks_need(cache->header.obj_info.cache_size);
	cache->num_of_obj = 0;
	cache->heads.free = cache->heads.full = cache->heads.half = NULL;
	cache->tails.free = cache->tails.full = cache->tails.half = NULL;
	kmem_cache_shrink(cache);
}
void kmem_init(void* space, int block_num) {
	num_of_blocks = block_num;
	startAdr = space;
	int log_num_of_blocks = log2(num_of_blocks);
	buddy_init(space, log2_block_size, log2_block_size + log_num_of_blocks,
		(char*)space + 3 * BLOCK_SIZE, (char*)space + block_num * BLOCK_SIZE);
	caches = (char*)space + 2 * BLOCK_SIZE;
	init_start_cache(caches);
}

kmem_cache_t* kmem_cache_create(const char* name, size_t size,
	void (*ctor)(void*), void (*dtor)(void*)) {
	kmem_cache_t* ret = kmem_cache_alloc(caches);
	ret->header.cache_name = name;
	ret->header.obj_info.cache_ctor = ctor;
	ret->header.obj_info.cache_dtor = dtor;
	ret->header.obj_info.cache_size = size;
	ret->header.num_of_slabs = 0;
	ret->header.slab_block_size = calculate_blocks_need(size);
	ret->num_of_obj = 0;
	ret->heads.free = ret->heads.full = ret->heads.half = NULL;
	ret->tails.free = ret->tails.full = ret->tails.half = NULL;
	kmem_cache_shrink(ret);
	return ret;
}

int kmem_cache_shrink(kmem_cache_t* cachep) {
	kmem_cache_t_slab* curr = buddy_malloc(cachep->header.slab_block_size * BLOCK_SIZE);
	kmem_cache_t_slab* head = cachep->heads.free;
	kmem_cache_t_slab* tail = cachep->tails.free;
	add(head, tail, curr);
	cachep->header.num_of_slabs++;
}

void* kmem_cache_alloc(kmem_cache_t* cachep) {
	if (!cachep)
		return NULL;
	kmem_cache_t_slab* head = NULL;
	kmem_cache_t_slab* tail = NULL;
	if (cachep->tails.half) {
		head = cachep->heads.half;
		tail = cachep->tails.half;
	} else if (cachep->tails.free) {
		head = cachep->heads.free;
		tail = cachep->tails.free;
	}
	if (!head || !tail) {
		kmem_cache_shrink(cachep);
		head = cachep->heads.free;
		tail = cachep->tails.free;
	}
	kmem_cache_t_slab* slab_for_obj = remove(head, tail);
	void* ret = (char*)slab_for_obj + sizeof(kmem_cache_t_slab) + slab_for_obj->curr_objects * cachep->header.obj_info.cache_size;
	void (*ctor)(void*) = cachep->header.obj_info.cache_ctor;
	ctor(ret);
	slab_for_obj->curr_objects++;
	if (slab_for_obj->curr_objects == cachep->header.obj_per_slab) {
		add(cachep->heads.full, cachep->tails.full, slab_for_obj);
	} else {
		add(cachep->heads.half, cachep->tails.half, slab_for_obj);
	}
	return ret;
}

void kmem_cache_free(kmem_cache_t* cachep, void* objp) {}

void* kmalloc(size_t size) {

}

void kfree(const void* objp) {}

void kmem_cache_destroy(kmem_cache_t* cachep) {}

void kmem_cache_info(kmem_cache_t* cachep) {
	//kmem_cache_t_header* h = &cachep->header;
	//printf("ime: %s\nvelicine: %d\n", h->cache_name, h->cache_size);
}

int kmem_cache_error(kmem_cache_t* cachep) { return 0; }
