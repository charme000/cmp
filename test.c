#include <time.h>
#include <stdlib.h>

#include "cmp.h"

unsigned int ts_count = 50000;
unsigned int ts_array_size = 1000;

void ts_cmp()
{
	unsigned int i = 0;

	printf("enter ts_cmp\n");
	for (i = 0; i < ts_count; ++i)
	{
		char* p = (char *)cmp_malloc(ts_array_size);
		cmp_free((void *)p);
	}
	printf("leave ts_cmp\n");
}

void ts_heap()
{
	unsigned int i = 0;

	printf("enter ts_heap\n");
	for (i = 0; i < ts_count; ++i)
	{
		char* p = (char *)malloc(ts_array_size);
		free((void *)p);
	}
	printf("leave ts_heap\n");
}

void main()
{
	clock_t start, finish;
	double duration = 0.0;

	cmp_init_mempool(0, 0, 0, true);

	start = clock();
	ts_cmp();
	finish = clock();
	duration = (double)(finish - start) / CLOCKS_PER_SEC;
	printf("alloc time for cmp : %f\n", duration);

	start = clock();
	ts_heap();
	finish = clock();
	duration = (double)(finish - start) / CLOCKS_PER_SEC;
	printf("alloc time for heap : %f\n", duration);

	cmp_destroy_mempool();

	getc(stdin);
	return;
}
