#pragma once
#include "slab.h"
#include "buddy.h"
#include <math.h>
#include <string.h>
#include <Windows.h>
#include <stdio.h>

typedef unsigned int uint;
typedef unsigned char uint8;
typedef struct kmem_cache_t_slab kmem_cache_t_slab;
typedef struct kmem_cache_t_lists kmem_cache_t_lists;
typedef struct kmem_cache_t_object_info kmem_cache_t_object_info;
typedef struct kmem_cache_t_header kmem_cache_t_header;
typedef struct kmem_cache_t_list_elem kmem_cache_t_list_elem;
typedef struct kmem_cache_t_obj_header kmem_cache_t_obj_header;
#define log2_block_size (12)
#define max_small_buff_pow (17)
#define min_small_buff_pow (5)
#define max_small_buff_size (max_small_buff_pow - min_small_buff_pow)

void* kmem_cache_alloc(kmem_cache_t* cachep);
int kmem_cache_expand(kmem_cache_t* cachep);

int num_of_blocks;
void* startAdr;
kmem_cache_t** smallBuff;
kmem_cache_t* caches;
HANDLE mutex;

typedef struct kmem_cache_t_obj_header {
	kmem_cache_t_obj_header* next;
	kmem_cache_t_slab* mySlab;
} kmem_cache_t_obj_header;
typedef struct kmem_cache_t_list_elem {
	kmem_cache_t_slab* head;
	kmem_cache_t_slab* tail;
} kmem_cache_t_list_elem;
typedef struct kmem_cache_t_slab {
	void* next;
	void* prev;
	kmem_cache_t_obj_header* free;
	uint curr_objects;
	uint object_start_offset;
} kmem_cache_t_slab;
typedef struct kmem_cache_t_lists {
	kmem_cache_t_list_elem free, half, full;
}kmem_cache_t_lists;

typedef struct kmem_cache_t_object_info {
	size_t object_size;
	void* object_ctor;
	void* object_dtor;
}kmem_cache_t_object_info;

typedef struct kmem_cache_t_header {
	char cache_name[MAX_NAME_SIZE];
	kmem_cache_t_object_info obj_info;
	uint slab_block_size;
	uint num_of_slabs;
	uint obj_per_slab;
}kmem_cache_t_header;
typedef struct kmem_cache_s {
	kmem_cache_t_header header;
	kmem_cache_t_lists lists;
	uint num_of_obj;
	uint unused_space;
	uint curr_offset;
	int error_code;
} kmem_cache_t;

uint calculate_blocks_need(size_t space_needed) {
	WaitForSingleObject(mutex, INFINITE);
	int ret = 1;
	while (!(ret * BLOCK_SIZE > space_needed)) {
		ret *= 2;
	}
	ReleaseMutex(mutex);
	return ret;
}
void addFromList(kmem_cache_t_list_elem* list, kmem_cache_t_slab* elem) {
	WaitForSingleObject(mutex, INFINITE);
	if (list->tail) {
		elem->prev = list->tail;
		list->tail = list->tail->next = elem;
	} else {
		elem->prev = elem->next = elem;
		list->head = list->tail = elem;
	}
	ReleaseMutex(mutex);
}
kmem_cache_t_slab* removeFromList(kmem_cache_t_list_elem* list) {
	if (!list->head || !list->tail)
		return NULL;
	WaitForSingleObject(mutex, INFINITE);
	kmem_cache_t_slab* ret;
	if (!list->head->prev) {
		ret = list->head;
		list->head = NULL;
		list->tail = NULL;
		ReleaseMutex(mutex);
		return ret;
	}
	if (list->head->prev == list->head) {
		ret = list->head;
		list->head = list->tail = NULL;
	} else {
		ret = list->tail;
		list->tail = list->head->prev = list->tail->prev;
		list->tail->next = list->head;
	}
	ret->next = NULL;
	ret->prev = NULL;
	ReleaseMutex(mutex);
	return ret;
}
void addObjList(kmem_cache_t_obj_header** l, kmem_cache_t_obj_header* elem) {
	WaitForSingleObject(mutex, INFINITE);
	elem->next = *l;
	*l = elem;
	ReleaseMutex(mutex);
}
void* removeObjList(kmem_cache_t_obj_header* l) {
	WaitForSingleObject(mutex, INFINITE);
	void* ret = l;
	l = l->next;
	ReleaseMutex(mutex);
	return ret;
}
void init_start_cache(kmem_cache_t* cache) {
	cache->header.obj_info.object_ctor = NULL;
	cache->header.obj_info.object_dtor = NULL;
	strcpy(cache->header.cache_name, "kes kesova");
	cache->header.obj_info.object_size = sizeof(kmem_cache_t) + sizeof(kmem_cache_t_obj_header);
	cache->header.num_of_slabs = 0;
	cache->header.slab_block_size = calculate_blocks_need(cache->header.obj_info.object_size);
	cache->num_of_obj = 0;
	cache->header.obj_per_slab = cache->header.slab_block_size * BLOCK_SIZE / cache->header.obj_info.object_size;
	cache->lists.free.head = 0;
	cache->lists.half.head = 0;
	cache->lists.full.head = 0;
	cache->lists.free.tail = 0;
	cache->lists.half.tail = 0;
	cache->lists.full.tail = 0;
	kmem_cache_expand(cache);
}
void kmem_init(void* space, int block_num) {
	num_of_blocks = block_num;
	startAdr = space;
	int log_num_of_blocks = log2(num_of_blocks);
	buddy_init(space, log2_block_size, log2_block_size + log_num_of_blocks+1,
		(char*)space + 3 * BLOCK_SIZE, (char*)space + block_num * BLOCK_SIZE);
	caches = (char*)space + 2 * BLOCK_SIZE;
	init_start_cache(caches);
	smallBuff = (char*)space + 2 * BLOCK_SIZE + sizeof(kmem_cache_t);
	for (int i = 0; i < max_small_buff_size; i++) {
		smallBuff[i] = NULL;
	}
	mutex = CreateMutex(0, 0, 0);
}

kmem_cache_t* kmem_cache_create(const char* name, size_t size,
	void (*ctor)(void*), void (*dtor)(void*)) {
	WaitForSingleObject(mutex, INFINITE);
	kmem_cache_t* ret = kmem_cache_alloc(caches);
	strcpy(ret->header.cache_name, name);
	ret->header.obj_info.object_ctor = ctor;
	ret->header.obj_info.object_dtor = dtor;
	ret->header.obj_info.object_size = size + sizeof(kmem_cache_t_obj_header);
	ret->header.num_of_slabs = 0;
	ret->header.slab_block_size = calculate_blocks_need(ret->header.obj_info.object_size);
	ret->num_of_obj = 0;
	ret->header.obj_per_slab = (ret->header.slab_block_size * BLOCK_SIZE - sizeof(kmem_cache_t_slab)) / ret->header.obj_info.object_size;
	ret->unused_space = ret->header.slab_block_size * BLOCK_SIZE - sizeof(kmem_cache_t_slab) - ret->header.obj_per_slab * ret->header.obj_info.object_size;
	ret->curr_offset = 0;
	ret->lists.free.head = 0;
	ret->lists.half.head = 0;
	ret->lists.full.head = 0;
	ret->lists.free.tail = 0;
	ret->lists.half.tail = 0;
	ret->lists.full.tail = 0;
	ret->error_code = 0;
	if (kmem_cache_expand(ret) == -1) {
		ReleaseMutex(mutex);
		return NULL;
	}
	ReleaseMutex(mutex);
	return ret;
}

int kmem_cache_expand(kmem_cache_t* cachep) {
	WaitForSingleObject(mutex, INFINITE);
	kmem_cache_t_slab* curr = buddy_malloc(cachep->header.slab_block_size * BLOCK_SIZE + sizeof(kmem_cache_t_obj_header));
	if (curr == NULL) {
		cachep->error_code = 1;
		ReleaseMutex(mutex);
		return -1;
	}
	kmem_cache_t_list_elem* l = &cachep->lists.free;
	addFromList(l, curr);
	cachep->header.num_of_slabs++;
	if (cachep->curr_offset >= cachep->unused_space) {
		cachep->curr_offset = 0;
	}
	curr->object_start_offset = cachep->curr_offset;
	cachep->curr_offset += CACHE_L1_LINE_SIZE;
	curr->curr_objects = 0;
	curr->free = 0;
	ReleaseMutex(mutex);
	return 1;
}

int kmem_cache_shrink(kmem_cache_t* cachep) {
	WaitForSingleObject(mutex, INFINITE);
	while (cachep->lists.free.head) {
		buddy_free(removeFromList(&cachep->lists.free));
		cachep->header.num_of_slabs--;
	}
	//cachep->lists.free.head = 0;
	//cachep->lists.free.tail = 0;
	ReleaseMutex(mutex);
	return 1;
}

void* kmem_cache_alloc(kmem_cache_t* cachep) {
	if (!cachep)
		return NULL;
	WaitForSingleObject(mutex, INFINITE);
	kmem_cache_t_list_elem* l = 0;
	if (cachep->lists.half.head) {
		l = &cachep->lists.half;
	} else if (cachep->lists.free.head) {
		l = &cachep->lists.free;
	}
	if (!l) {
		if (kmem_cache_expand(cachep) == -1) {
			//printf("greska\n");
			ReleaseMutex(mutex);
			return NULL;
		}
		l = &cachep->lists.free;
	}
	kmem_cache_t_slab* slab_for_obj = removeFromList(l);
	void* ret;
	if (!slab_for_obj->free)
		ret = (char*)slab_for_obj + slab_for_obj->object_start_offset + sizeof(kmem_cache_t_slab) + slab_for_obj->curr_objects * cachep->header.obj_info.object_size;
	else
		ret = removeObjList(slab_for_obj->free);
	((kmem_cache_t_obj_header*)ret)->mySlab = slab_for_obj;
	ret = (char*)ret + sizeof(kmem_cache_t_obj_header);
	void (*ctor)(void*) = cachep->header.obj_info.object_ctor;
	if (ctor)
		ctor(ret);
	slab_for_obj->curr_objects++;
	if (slab_for_obj->curr_objects == cachep->header.obj_per_slab) {
		addFromList(&cachep->lists.full, slab_for_obj);
	} else {
		addFromList(&cachep->lists.half, slab_for_obj);
	}
	cachep->num_of_obj++;
	ReleaseMutex(mutex);
	return ret;
}

void kmem_cache_free(kmem_cache_t* cachep, void* objp) {
	WaitForSingleObject(mutex, INFINITE);
	cachep->num_of_obj--;
	kmem_cache_t_obj_header* h = (char*)objp - sizeof(kmem_cache_t_obj_header);
	kmem_cache_t_slab* cur = h->mySlab;
	if (cur->curr_objects == 1) {
		addFromList(&cachep->lists.free, cur);
	} else {
		addFromList(&cachep->lists.half, cur);
	}
	cur->curr_objects--;
	addObjList(&cur->free, h);
	ReleaseMutex(mutex);
}

void* kmalloc(size_t size) {
	WaitForSingleObject(mutex, INFINITE);
	uint ind = log2(size) - min_small_buff_pow;
	if (!smallBuff[ind]) {
		smallBuff[ind] = kmem_cache_create("mali bafer", size + sizeof(uint), NULL, NULL);
	}
	void* ret = kmem_cache_alloc(smallBuff[ind]);
	*(uint*)ret = ind;
	ReleaseMutex(mutex);
	return (char*)ret + sizeof(uint);
}

void kfree(const void* objp) {
	WaitForSingleObject(mutex, INFINITE);
	uint ind = *(uint*)((char*)objp - sizeof(uint));
	kmem_cache_free(smallBuff[ind], (char*)objp - sizeof(uint));
	ReleaseMutex(mutex);
}

void kmem_cache_destroy(kmem_cache_t* cachep) {
	WaitForSingleObject(mutex, INFINITE);
	if (cachep->num_of_obj) {
		cachep->error_code = 2;
		ReleaseMutex(mutex);
		return;
	}
	while (cachep->lists.free.head)
		buddy_free(removeFromList(&cachep->lists.free));
	while (cachep->lists.full.head)
		buddy_free(removeFromList(&cachep->lists.full));
	while (cachep->lists.half.head)
		buddy_free(removeFromList(&cachep->lists.half));
	kmem_cache_free(caches, cachep);
	ReleaseMutex(mutex);
}


/*informacije koje se stampaju za jedan kes su ime, velicina jednog podatka izrazena u
bajtovima, velicina celog kesa izrazenog u broju blokova, broj ploca, broj objekata u jednoj
ploci i procentualna popunjenost kesa. */
void kmem_cache_info(kmem_cache_t* cachep) {
	WaitForSingleObject(mutex, INFINITE);
	printf("//////////////////////////\n");
	printf("Ime kesa: %s\n", cachep->header.cache_name);
	printf("Velicina jednog podatka: %d\n", cachep->header.obj_info.object_size);
	printf("Velicina kesa u blokovima: %d\n", cachep->header.num_of_slabs * cachep->header.slab_block_size);
	printf("Broj ploca: %d\n", cachep->header.num_of_slabs);
	printf("Broj objekat po ploci: %d\n", cachep->header.obj_per_slab);
	printf("Popunjenost kesa: %f\n", (float)(cachep->header.obj_info.object_size * cachep->num_of_obj) / (float)(cachep->header.num_of_slabs * cachep->header.slab_block_size * BLOCK_SIZE) * 100.0);
	printf("//////////////////////////\n");
	ReleaseMutex(mutex);
}

int kmem_cache_error(kmem_cache_t* cachep) {
	WaitForSingleObject(mutex,INFINITE);
	switch (cachep->error_code) {
	case 1:
		printf("Neusesno prosirenje kesa\n");
		return 1;
	case 2:
		printf("Pokusaj destroy dok postoje objekti u kesu\n");
		break;
	case 0:
		printf("Nije bilo nikakve greske\n");
		break;
	default:
		break;
	}
	ReleaseMutex(mutex);
	return cachep->error_code;
}
