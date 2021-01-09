#include "slab.h"
#include "buddy.h"
#include "buddy_list.h"
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
	void* metaspace = malloc(1024 * sizeof(List_Node));
	void* space = malloc(1024);
	buddy_init(metaspace, 1, 10, space,(char*)space+1024);
	printf("11 %d\n", (char*)buddy_malloc(480) - space);
	printf("12 %d\n", (char*)buddy_malloc(480) - space);
	//printf("21 %d\n", (char*)buddy_malloc(200) - space);
	//printf("22 %d\n", (char*)buddy_malloc(200) - space);
	//printf("31 %d\n", (char*)buddy_malloc(100) - space);
	//printf("32 %d\n", (char*)buddy_malloc(100) - space);
	//printf("41 %d\n", (char*)buddy_malloc(40) - space);
	//printf("42 %d\n", (char*)buddy_malloc(40) - space);
	//printf("41 %d\n", (char*)buddy_malloc(10) - space);
	//printf("41 %d\n", (char*)buddy_malloc(10) - space);
	buddy_free((char*)space - 12, 0);
	printf("43 %d\n", (char*)buddy_malloc(250) - space);
	free(metaspace);
	free(space);
	//	kmem_init(space, 1000);
	////	kmem_cache_t* shared = kmem_cache_create("prvi kes", sizeof(myStruct), constructor, NULL);
	////	kmem_cache_t* shared2 = kmem_cache_create("prvi kes", sizeof(myStruct2), constructor2, NULL);
	////	kmem_cache_info(shared);
	////	kmem_cache_info(shared2);
	////	myStruct* obj1 = kmem_cache_alloc(shared);
	////	myStruct* obj2 = kmem_cache_alloc(shared);
	////	myStruct2* obj3 = kmem_cache_alloc(shared2);
	////	printf("objekat 1: %d %d %d\nobjekat 2: %d %d %d\nobjekat 3:", obj1->a, obj1->b, obj1->c, obj2->a, obj2->b, obj2->c);
	////	for (int i = 0; i < 10; i++) {
	////		printf("%d ", obj3->a[i]);
	////	}
	////	printf("\n");
	//	buddy_init(metaspace, 4,10,space);
	//	void* f = buddy_malloc(32);
	//	printf("%d\n", (char*)f - space);
	//	void* f1 = buddy_malloc(6);
	//	void* f2 = buddy_malloc(6);
	//	printf("%d\n", (char*)f1 - space);
	//	//printf("%d\n", (char*)f2 - space);
	//	printf("%d\n", (char*)buddy_malloc(100) - space);
	//	printf("%d\n", (char*)buddy_malloc(230) - space);
	//	printf("%d\n", (char*)buddy_malloc(470) - space);
	//	buddy_free(f1);
	//	buddy_free(f2);
	//	buddy_free(f);
	//	printf("%d\n", (char*)buddy_malloc(62) - space);
	//	free(metaspace);
	//	free(space);
	//	//printf("%d\n", (char*)buddy_malloc(32) - space);
}
