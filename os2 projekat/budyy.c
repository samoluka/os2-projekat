#pragma once
#include "buddy.h"
#include "buddy_list.h"


typedef unsigned int uint;
typedef unsigned char uint8;

#define header_size (sizeof(List_Node));

List_Node* remove_last(List_Node* elem);
List_Node* all_Lists;
uint num_of_Lists;
uint min_Alloc;
uint max_Alloc;
uint min_power;
uint max_power;
uint lower_limit;
uint8* splits;
void* start_Adr, * max;



int find(size_t request) {
	uint curr = min_Alloc;
	int ret = num_of_Lists - 1;
	while (curr < request && ret >= 0) {
		curr *= 2;
		ret--;
	}
	return ret;
}

void* index_to_ptr(uint index_of_Tree, uint index_of_List) {
	return (char*)start_Adr +
		((index_of_Tree - (1 << index_of_List) + 1) <<
			(max_power - index_of_List));
}
uint ptr_to_index(void* ptr, uint index_of_List) {
	return (((char*)ptr - start_Adr) >> (max_power - index_of_List)) +
		(1 << index_of_List) - 1;
}
int is_split(uint index) {
	return (splits[index / 8] >> (index % 8)) & 1;
}
void invers_split(uint index) {
	splits[index / 8] ^= 1 << (index % 8);
}
//*-Move to parent : index = (index - 1) / 2;
//*-Move to left child : index = index * 2 + 1;
//*-Move to right child : index = index * 2 + 2;
//*-Move to sibling : index = ((index - 1) ^ 1) + 1;
void lower_bucket_limit(uint entry) {
	while (entry < lower_limit) {
		uint root_index = ptr_to_index(start_Adr, lower_limit);
		uint parent = (root_index - 1) / 2;
		if (!is_split(parent)) {
			lower_limit--;
			remove((List_Node*)start_Adr);
			initNode(&all_Lists[lower_limit]);
			add(&all_Lists[lower_limit], (List_Node*)start_Adr);
			invers_split(parent);
			continue;
		}
		void* right_ptr = index_to_ptr(root_index + 1, lower_limit);
		max = (char*)right_ptr + header_size;
		add(&all_Lists[lower_limit], (List_Node*)right_ptr);
		initNode(&all_Lists[--lower_limit]);
		root_index = (root_index - 1) / 2;
		if (root_index)
			invers_split((root_index - 1) / 2);
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
		lower_bucket_limit(entry);
		ret = remove_last(&all_Lists[entry]);
		if (!ret) {
			if (entry != lower_limit || !entry) {
				entry--;
				continue;
			}
			lower_bucket_limit(entry - 1);
			ret = remove_last(&all_Lists[entry]);
		}
		max = (char*)ret + real_request;
		int ret_index = ptr_to_index(ret, entry);
		if (ret_index) {
			invers_split((ret_index - 1) / 2);
		}
		while (entry < original) {
			ret_index = ret_index * 2 + 1;
			entry++;
			invers_split((ret_index - 1) / 2);
			add(&all_Lists[entry], (List_Node*)index_to_ptr(ret_index + 1, entry));
		}
		((List_Node*)ret)->size = real_request;
		return (char*)ret + header_size;
	}
	return NULL;
};
void buddy_free(void* ptr) {
	if (!ptr)
		return;
	ptr = (char*)ptr - header_size;
	uint real_request = ((List_Node*)ptr)->size;
	uint entry = find(real_request);
	uint index = ptr_to_index(ptr, entry);
	uint parent = (index - 1) / 2;
	while (index) {
		invers_split(parent);
		if (is_split(parent) || entry == lower_limit)
			break;
		remove(index_to_ptr(((index - 1) ^ 1) + 1, entry));
		index = parent;
		parent = (index - 1) / 2;
		entry--;
	}
	add(&all_Lists[entry], index_to_ptr(index, entry));
};
void buddy_init(void* metaSpace, uint minP, uint maxP, void* start) {
	all_Lists = (List_Node*)metaSpace;
	min_power = minP;
	min_Alloc = 1 << min_power;
	max_power = maxP;
	max_Alloc = 1 << max_power;
	start_Adr = start;
	num_of_Lists = max_power - min_power + 1;
	for (int i = 0; i < num_of_Lists; i++) {
		all_Lists[i] = *initNode(&all_Lists[i]);
	}
	max = NULL;
	add(&all_Lists[num_of_Lists - 1], (List_Node*)start_Adr);
	lower_limit = num_of_Lists - 1;
	splits = &all_Lists[num_of_Lists];
	for (int i = 0; i < ((1 << num_of_Lists - 1) / 8);i++)
		splits[i] = 0;
};