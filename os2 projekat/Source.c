#include "slab.h"
#include "buddy.h"

typedef struct myStruct {
	int a;
	int b;
	int c;
}myStruct;


typedef struct myStruct2 {
	int a[10];
}myStruct2;
int x = 0;
void constructor(void* data) {
	myStruct* c = (myStruct*)data;
	c->a = x++;
	c->b = x++;
	c->c = x++;
}
void constructor2(void* data) {
	myStruct2* c = (myStruct2*)data;
	for (int i = 0; i < 10; i++) {
		c->a[i] = x++;
	}
}

int main() {
	void* metaspace = malloc(7*2*sizeof(void*));
	void* space = malloc(BLOCK_SIZE * 1024);
	//kmem_init(space, 1024);
//	kmem_cache_t* shared = kmem_cache_create("prvi kes", sizeof(myStruct), constructor, NULL);
//	kmem_cache_t* shared2 = kmem_cache_create("prvi kes", sizeof(myStruct2), constructor2, NULL);
//	kmem_cache_info(shared);
//	kmem_cache_info(shared2);
//	myStruct* obj1 = kmem_cache_alloc(shared);
//	myStruct* obj2 = kmem_cache_alloc(shared);
//	myStruct2* obj3 = kmem_cache_alloc(shared2);
//	printf("objekat 1: %d %d %d\nobjekat 2: %d %d %d\nobjekat 3:", obj1->a, obj1->b, obj1->c, obj2->a, obj2->b, obj2->c);
//	for (int i = 0; i < 10; i++) {
//		printf("%d ", obj3->a[i]);
//	}
//	printf("\n");
	buddy_init(metaspace, 4,10,space);
	printf("%d\n", (char*)buddy_malloc(32) - space);
	printf("%d\n", (char*)buddy_malloc(64) - space);
	printf("%d\n", (char*)buddy_malloc(64) - space);
	printf("%d\n", (char*)buddy_malloc(32) - space);
	printf("%d\n", (char*)buddy_malloc(128) - space);
	buddy_free((char*)space+12);
	buddy_free((char*)space + 76);
	printf("%d\n", (char*)buddy_malloc(89) - space);
	printf("%d\n", (char*)buddy_malloc(89) - space);
	printf("%d\n", (char*)buddy_malloc(89) - space);
	printf("%d\n", (char*)buddy_malloc(89) - space);
}
