#pragma once
#include "buddy_list.h"


List_Node* initNode(List_Node* elem) {
	elem->prev = elem;
	elem->next = elem;
	return elem;
}

void add(List_Node* List, List_Node* elem) {
	if (!elem)
		return;
	if (!List) {
		return NULL;
	}
	List_Node* tail = List->prev;
	List->prev = elem;
	tail->next = elem;
	elem->next = List;
	elem->prev = tail;
}

void add_first(List_Node* List, List_Node* elem) {
	if (!elem)
		return;
	if (!List) {
		return NULL;
	}
	List_Node* head = List->next;
	List->prev = elem;
	head->next = elem;
	elem->next = List;
	elem->prev = head;
}

void removeMy(List_Node* elem) {
	elem->prev->next = elem->next;
	elem->next->prev = elem->prev;
}
List_Node* remove_last(List_Node* elem) {
	if (!elem)
		return NULL;
	if (elem->prev == elem)
		return NULL;
	List_Node* ret = elem->prev;
	removeMy(ret);
	return ret;
}