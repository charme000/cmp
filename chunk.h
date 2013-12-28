#ifndef __CHUNK_HEADER__
#define __CHUNK_HEADER__

#include "defines.h"

typedef struct MemChunk
{
	uchar *data;
	uint data_size;
	uint used_size;
	bool is_alloc_chunk;
	struct MemChunk *next;
}MemChunk;

#endif