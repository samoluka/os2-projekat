#pragma once
#include <stdlib.h>

typedef struct List_Node {
	struct List_Node* prev, * next;
	size_t size;
}List_Node;

List_Node* initNode(List_Node* elem);
void add(List_Node* List, List_Node* elem);
void add_first(List_Node* List, List_Node* elem);
void removeMy(List_Node* elem);
List_Node* remove_last(List_Node* elem);