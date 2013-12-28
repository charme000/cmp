#include <time.h>
#include <stdlib.h>

#include "cmp.h"

unsigned int ts_count = 50000;
unsigned int ts_array_size = 1000;

void malloc_cmp()
{
	printf("enter ts_cmp\n");
	for (unsigned int i = 0; i < ts_count; ++i)
	{
		char* p = (char *)cmp_malloc(ts_array_size);
		cmp_free((void *)p);
	}
	printf("leave ts_cmp\n");
}

void malloc_heap()
{
	printf("leave ts_heap\n");
	for (unsigned int i = 0; i < ts_count; ++i)
	{
		char* p = (char *)malloc(ts_array_size);
		free((void *)p);
	}
	printf("leave ts_heap\n");
}

void ts_speed()
{
	clock_t start, finish;
	double duration = 0.0;

	start = clock();
	malloc_cmp();
	finish = clock();
	duration = (double)(finish - start) / CLOCKS_PER_SEC;
	printf("alloc time for cmp : %f\n", duration);

	start = clock();
	malloc_heap();
	finish = clock();
	duration = (double)(finish - start) / CLOCKS_PER_SEC;
	printf("alloc time for heap : %f\n", duration);
}

void ts_mulity()
{
	char* p1 = (char*)cmp_malloc(512);
	printf("p1 addr = 0x%08x\n", p1);

	char* p2 = (char*)cmp_malloc(512);
	printf("p2 addr = 0x%08x\n", p2);

	cmp_free(p1);
	printf("p1 is freed\n");

	strcpy(p1, "charme");

	// through p2 is not free, but you can malloc the p3
	char* p3 = (char*)cmp_malloc(100);
	printf("p3 addr = 0x%08x\n", p3);

	cmp_free(p2);
	printf("p2 is freed\n",p2);

	cmp_free(p3);
	printf("p3 is freed\n");
}

void ts_normal()
{
	char* p1 = (char*)cmp_malloc(512);

	strcpy(p1, "charme is best!");
	printf("write string:%s\n", p1);
	cmp_free(p1);
}

void main()
{
	// alloc memory pool
	cmp_init_mempool(0, 0, 0, true);

	// speed test
	printf("****test speed****\n");
	ts_speed();
	printf("\n");

	// mulity malloc
	printf("****test mulity alloc****\n");
	ts_mulity();
	printf("\n");

	// normal malloc
	printf("****test normal alloc****\n");
	ts_normal();
	printf("\n");

	// destroy memory pool
	cmp_destroy_mempool();

	getc(stdin);
	return;
}