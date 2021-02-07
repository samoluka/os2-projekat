//#include "slab.h"
//#include "buddy.h"
//#include "buddy_list.h"
//typedef struct myStruct {
//	int a;
//	int b;
//	int c;
//	int d;
//}myStruct;
//
//
//typedef struct myStruct2 {
//	int a[10];
//}myStruct2;
//int x = 0;
//void constructor(void* data) {
//	myStruct* c = (myStruct*)data;
//	c->a = x++;
//	c->b = x++;
//	c->c = x++;
//	c->d = x++;
//}
//void constructor2(void* data) {
//	myStruct2* c = (myStruct2*)data;
//	for (int i = 0; i < 10; i++) {
//		c->a[i] = x++;
//	}
//}
//
//int main() {
//	void* space = malloc(1040 * BLOCK_SIZE);
//	kmem_init(space, 1040);
//	kmem_cache_t* shared = kmem_cache_create("prvi kes", sizeof(myStruct), constructor, NULL);
//	kmem_cache_t* shared2 = kmem_cache_create("prvi kes", sizeof(myStruct), constructor, NULL);
//	//kmem_cache_t* shared2 = kmem_cache_create("prvi kes", sizeof(myStruct2), constructor2, NULL);
//	//kmem_cache_info(shared);
//	//kmem_cache_info(shared2);
//	myStruct* obj1 = kmem_cache_alloc(shared);
//	myStruct* obj2 = kmem_cache_alloc(shared);
//	myStruct* obj3 = kmem_cache_alloc(shared);
//	myStruct* obj4 = kmem_cache_alloc(shared);
//	kmem_cache_destroy(shared);
//	shared2 = kmem_cache_create("prvi kes", sizeof(myStruct), constructor, NULL);
//
//	free(space);
//	/*printf("objekat 1: %d %d %d\nobjekat 2: %d %d %d\nobjekat 3:", obj1->a, obj1->b, obj1->c, obj2->a, obj2->b, obj2->c);
//	for (int i = 0; i < 10; i++) {
//		printf("%d ", obj3->a[i]);
//	}
//	printf("\n");*/
//}
