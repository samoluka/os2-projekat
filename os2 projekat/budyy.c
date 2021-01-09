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
static void* fake = NULL;

#define header_size (sizeof(List_Node))
#define parent(x) ((x-1)/2)
#define left_child(x) ((x*2)+1)
#define right_child(x) ((x*2)+2)
#define buddy(x) (((x-1)^1)+1)

inline void* index_to_ptr(uint index_of_Tree, uint index_of_List) {
	return (char*)start_Adr +
		((index_of_Tree - (1 << index_of_List) + 1) <<
			(max_power - index_of_List));
}
inline uint ptr_to_index(void* ptr, uint index_of_List) {
	return (((char*)ptr - start_Adr) >> (max_power - index_of_List)) +
		(1 << index_of_List) - 1;
}
inline int is_split(uint index) {
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

void build_tree(uint entry) {
	while (entry < lower_limit) {
		uint root_index = ptr_to_index(start_Adr, lower_limit);
		uint parent = parent(root_index);
		if (!is_split(parent)) {
			lower_limit--;
			remove((List_Node*)start_Adr);
			initNode(&all_Lists[lower_limit]);
			add(&all_Lists[lower_limit], (List_Node*)start_Adr);
			invers_split(parent);
		} else {
			void* right_ptr = index_to_ptr(root_index + 1, lower_limit);
			add(&all_Lists[lower_limit], (List_Node*)right_ptr);
			initNode(&all_Lists[--lower_limit]);
			root_index = parent(root_index);
			if (root_index)
				invers_split(parent(root_index));
		}
	}
}
void* buddy_malloc(size_t request) {
	size_t real_request = request + header_size;
	void* ret = NULL;
	if (real_request > max_Alloc) {
		return ret;
	}
	int entry = find(real_request);
	int original = entry;
	if (entry < 0)
		return ret;
	while (entry >= 0) {
		build_tree(entry);
		ret = remove_last(&all_Lists[entry]);
		if (fake && ret == fake)
			continue;
		if (!ret) {
			if (entry != lower_limit || !entry) {
				entry--;
				continue;
			}
			build_tree(entry - 1);
			ret = remove_last(&all_Lists[entry]);
		}
		int ret_index = ptr_to_index(ret, entry);
		if (ret_index) {
			invers_split(parent(ret_index));
		}
		while (entry < original) {
			ret_index = left_child(ret_index);
			entry++;
			invers_split(parent(ret_index));
			add(&all_Lists[entry], (List_Node*)index_to_ptr(ret_index + 1, entry));
		}
		((List_Node*)ret)->size = real_request;
		//if ((char*)ret + (1 << (num_of_Lists - original)) > last_Adr) {
		//	buddy_free((char*)ret + header_size, 1);
		//	fake = ret;
		//	//remove(ret);
		//	return NULL;
		//}
		return (char*)ret + header_size;
	}
	return NULL;
};
void buddy_free(void* ptr, int a) {
	if (!ptr)
		return;
	ptr = (char*)ptr - header_size;
	uint real_request = ((List_Node*)ptr)->size;
	uint entry = find(real_request);
	uint index = ptr_to_index(ptr, entry);
	uint parent = parent(index);
	while (index) {
		invers_split(parent);
		if (is_split(parent) || entry == lower_limit)
			break;
		remove(index_to_ptr(buddy(index), entry));
		index = parent;
		parent = parent(index);
		entry--;
	}
	add(&all_Lists[entry], index_to_ptr(index, entry));
};
void buddy_init(void* metaSpace, uint minP, uint maxP, void* start, void* end) {
	all_Lists = (List_Node*)metaSpace;
	min_power = minP;
	min_Alloc = 1 << min_power;
	max_power = maxP;
	max_Alloc = 1 << max_power;
	start_Adr = start;
	last_Adr = end;
	num_of_Lists = max_power - min_power + 1;
	initNode(&all_Lists[num_of_Lists - 1]);
	add(&all_Lists[num_of_Lists - 1], (List_Node*)start_Adr);
	lower_limit = num_of_Lists - 1;
	splits = &all_Lists[num_of_Lists];
	for (int i = 0; i < ((1 << num_of_Lists - 1) / 8);i++)
		splits[i] = 0;
};