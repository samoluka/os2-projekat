#pragma once
// File: slab.h
#include <stdlib.h>

int num_of_blocks;
void* startAdr;
void* head;
void* tail;
typedef struct kmem_cache_t_header {
	const char* cache_name;
	size_t cache_size;
	void* cache_ctor;
	void* cache_dtor;
	void* next;
	unsigned int num_of_slabs;
}kmem_cache_t_header;
typedef struct kmem_cache_t_obj {
	void* next;
}kmem_cache_t_obj;
typedef struct kmem_cache_s {
	kmem_cache_t_header header;
	kmem_cache_t_obj* head;
	kmem_cache_t_obj* tail;
	unsigned int num_of_obj;
} kmem_cache_t;
#define BLOCK_SIZE (4096)
#define CACHE_L1_LINE_SIZE (64)
#define MAX_NAME_SIZE (16)
void kmem_init(void* space, int block_num);

kmem_cache_t* kmem_cache_create(const char* name, size_t size, void (*ctor)(void*), void (*dtor)(void*)); // Allocate cache
int kmem_cache_shrink(kmem_cache_t* cachep); // Shrink cache
void* kmem_cache_alloc(kmem_cache_t* cachep); // Allocate one object from cache
void kmem_cache_free(kmem_cache_t* cachep, void* objp); // Deallocate one object from cache
void* kmalloc(size_t size); // Alloacate one small memory buffer
void kfree(const void* objp); // Deallocate one small memory buffer
void kmem_cache_destroy(kmem_cache_t* cachep); // Deallocate cache
void kmem_cache_info(kmem_cache_t* cachep); // Print cache info
int kmem_cache_error(kmem_cache_t* cachep); // Print error message