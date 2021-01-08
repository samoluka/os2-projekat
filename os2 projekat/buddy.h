#pragma once
#include <stdlib.h>




void* buddy_malloc(size_t request);
void buddy_free(void* bucket);
void buddy_init(void* metaSpace, unsigned int min, unsigned int max, void* start);