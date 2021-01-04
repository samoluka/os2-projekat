#include "slab.h"


int main() {
	void* space = malloc(BLOCK_SIZE * 1024);
	kmem_init(space, 1024);
	kmem_cache_t* shared = kmem_cache_create("prvi kes", (size_t)150, NULL, NULL);
	kmem_cache_info(shared);
}