#pragma warning(disable : 4996)

#include<string.h>
#include<stdint.h>
#include<assert.h>
#include<stdio.h>

#define u8 uint8_t 
#define u16 uint16_t
#define STACK_SIZE 32
#define HEAP_SIZE STACK_SIZE * 4
#define HEADER 4

static u16 IN_USE;

typedef struct virtual_memory
{
	u8 stack[STACK_SIZE];
	char** unmapped;
	u8 heap[HEAP_SIZE];

	struct
	{
		char** data;
		char** bss;
		char* text;
	}data_t;

}virtual_memory_t;

typedef struct entity
{
	u8* ptr;
	u16 size;
}entity_t;

entity_t LIST[40];

void LOG()
{
	printf("OUR LIST\n");
	for (unsigned i = 0; i < IN_USE; i++)
	{
		printf("Data + Header.(%p). Memory of our heap free[%u]:\n",LIST[i].ptr,LIST[i].size);
	}
	printf("Entity in use:[%d]", (sizeof(LIST) / sizeof(LIST[0]) - IN_USE));
}

entity_t* new_entity(size_t size)
{
	if (LIST[0].ptr == NULL && LIST[0].size == 0)
	{
		static virtual_memory_t vm;
		LIST[0].ptr = vm.heap;
		LIST[0].size = HEAP_SIZE;
		IN_USE++;
		LOG();
	}
	entity_t* best = LIST;

	for (unsigned i = 0; i < IN_USE; i++)
	{
		if (LIST[i].size >= size && LIST[i].size < best->size)
			best = &LIST[i];
	}

	return best;
}

void* k_malloc(size_t size)
{
	assert(size <= HEAP_SIZE);

	size += HEADER;

	entity_t* e = new_entity(size); 

	u8* start = e->ptr;
	u8* user_ptr = start + HEADER;

	*start = size;

	e->ptr += size;
	e->size -= size;

	assert(e->size >= 0);
	LOG();

	return user_ptr;
}

void k_free(void* ptr)
{
	u8* start = (u8*)ptr - HEADER;

	LIST[IN_USE].ptr = start;
	LIST[IN_USE].size = *start;
	IN_USE++;
	LOG();
}

void test()
{
	typedef struct foo
	{
		int a;
		int b;
	}foo_t;

	foo_t* foo;

	char* bar;

	int* bazz;

	foo = (foo_t*)k_malloc(sizeof(foo_t));
	bar = (char*)k_malloc(sizeof(5));
	bazz = (int*)k_malloc(sizeof(int));

	foo->a = 5;
	foo->b = 10;

	strcpy(bar, "bar");
	memcpy(bazz, &foo->a, sizeof(int));

	printf("Address: [%p], data: [%d] [%d]\n", foo, foo->a, foo->b);
	printf("Address: [%p], data: [%s]\n", bar, bar);
	printf("Address: [%p], data: [%d]\n", bazz,*bazz);

	k_free(foo);
	k_free(bazz);
	
}