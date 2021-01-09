#pragma once
#include "buddy.h"
#include "buddy_list.h"
#include <math.h>

typedef unsigned int uint;
typedef unsigned char uint8;

static uint8* splits;
static void* start_Adr;
static List_Node* all_Lists;
static uint num_of_Lists;
static uint min_Alloc;
static uint max_Alloc;
static uint min_power;
static uint max_power;
static uint lower_limit;
static void* last_Adr;
static void* max = NULL;

#define header_size (sizeof(List_Node))
#define parent(x) ((x-1)/2)
#define left_child(x) ((x*2)+1)
#define right_child(x) ((x*2)+2)
#define buddy(x) (((x-1)^1)+1)

int update_max(size_t new_max) {
	if (new_max > last_Adr) {
		return 0;
	}
	max = new_max;
	return 1;
}
inline void* index_to_ptr(uint index_of_Tree, uint index_of_List) {
	return (char*)start_Adr +
		((index_of_Tree - (1 << index_of_List) + 1) <<
			(max_power - index_of_List));
}
inline uint ptr_to_index(void* ptr, uint index_of_List) {
	return (((char*)ptr - start_Adr) >> (max_power - index_of_List)) +
		(1 << index_of_List) - 1;
}
inline uint is_split(uint index) {
	return (splits[index / 8] >> (index % 8)) & 1;
}
inline void invers_split(uint index) {
	splits[index / 8] ^= 1 << (index % 8);
}





int find(size_t request) {
	uint curr = min_Alloc;
	int ret = num_of_Lists - 1;
	while (curr < request && ret >= 0) {
		curr <<= 1;
		ret--;
	}
	return ret;
	//uint l = (uint)(log(request) / log(2));
	//return  max_power - l - 1;
}
static int parent_is_split(size_t index) {
	index = (index - 1) / 2;
	return (splits[index / 8] >> (index % 8)) & 1;
}

/*
 * Given the index of a node, this flips the "is split" flag of the parent.
 */
static void flip_parent_is_split(size_t index) {
	index = (index - 1) / 2;
	splits[index / 8] ^= 1 << (index % 8);
}

int lower_bucket_limit(uint bucket) {
	while (bucket < lower_limit) {
		size_t root = ptr_to_index(start_Adr, lower_limit);
		char* right_child;
		if (!parent_is_split(root)) {
			remove((List_Node*)start_Adr);
			initNode(&all_Lists[--lower_limit]);
			add(&all_Lists[lower_limit], (List_Node*)start_Adr);
			continue;
		}
		right_child = index_to_ptr(root + 1, lower_limit);
		if (!update_max(right_child +header_size)) {
			return 0;
		}
		add(&all_Lists[lower_limit], (List_Node*)right_child);
		initNode(&all_Lists[--lower_limit]);
		root = (root - 1) / 2;
		if (root != 0) {
			flip_parent_is_split(root);
		}
	}

	return 1;
}
void* buddy_malloc(size_t request) {
	size_t original_bucket, bucket;
	if (request + header_size > max_Alloc) {
		return NULL;
	}
	bucket = find(request + header_size);
	original_bucket = bucket;
	while (bucket + 1 != 0) {
		size_t size, bytes_needed, i;
		char* ptr = NULL;
		if (!lower_bucket_limit(bucket)) {
			ptr= NULL;
		} else {
			ptr = (char*)remove_last(&all_Lists[bucket]);
		}
		if (!ptr) {

			if (bucket != lower_limit || bucket == 0) {
				bucket--;
				continue;
			}
			if (!lower_bucket_limit(bucket - 1)) {
				return NULL;
			}
			ptr = (char*)remove_last(&all_Lists[bucket]);
		}
		size = (size_t)1 << (max_power - bucket);
		bytes_needed = bucket < original_bucket ? size / 2 + header_size : size;
		if (!update_max(ptr + bytes_needed)) {
			add(&all_Lists[bucket], (List_Node*)ptr);
			bucket--;
			continue;
		}
		i = ptr_to_index(ptr, bucket);
		if (i != 0) {
			flip_parent_is_split(i);
		}
		while (bucket < original_bucket) {
			i = i * 2 + 1;
			bucket++;
			flip_parent_is_split(i);
			add(&all_Lists[bucket], (List_Node*)index_to_ptr(i + 1, bucket));
		}
		((List_Node*)ptr)->size = request;
		return ptr + header_size;
	}

	return NULL;
}

void buddy_free(void* ptr) {
	size_t bucket, i;
	if (!ptr) {
		return;
	}
	ptr = (char*)ptr - header_size;
	bucket = find(((List_Node*)ptr)->size + header_size);
	i = ptr_to_index((char*)ptr, bucket);
	while (i != 0) {
		flip_parent_is_split(i);
		if (parent_is_split(i) || bucket == lower_limit) {
			break;
		}
		remove((List_Node*)index_to_ptr(((i - 1) ^ 1) + 1, bucket));
		i = (i - 1) / 2;
		bucket--;
	}
	add(&all_Lists[bucket], (List_Node*)index_to_ptr(i, bucket));
}
void buddy_init(void* metaSpace, uint minP, uint maxP, void* start, void* end) {
	all_Lists = (List_Node*)metaSpace;
	min_power = minP;
	min_Alloc = 1 << min_power;
	max_power = maxP;
	max_Alloc = 1 << max_power;
	start_Adr = start;
	last_Adr = end;
	max = start_Adr;
	num_of_Lists = max_power - min_power + 1;
	initNode(&all_Lists[num_of_Lists - 1]);
	add(&all_Lists[num_of_Lists - 1], (List_Node*)start_Adr);
	lower_limit = num_of_Lists - 1;
	splits = &all_Lists[num_of_Lists];
	for (int i = 0; i < ((1 << num_of_Lists - 1) / 8);i++)
		splits[i] = 0;
};