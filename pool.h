#ifndef __POOL_HEADER__
#define __POOL_HEADER__

#include "defines.h"
#include "chunk.h"

static const sint FREEED_MEMORY = 0xFF;
static const sint ALLOCATED_MEMORY = 0xAA;

static const uint DEFAULT_MEMORY_POOL_SIZE = 1000;
static const uint DEFAULT_MEMORY_CHUNK_SIZE = 128;
static const uint DEFAULT_MEMORY_SIZE_TO_ALLOCATE = 128 * 2;
static const bool DEFAULT_MEMORY_INITIALIZE = true;

typedef struct MemPool
{
	MemChunk *first_chunk;
	MemChunk *last_chunk;
	MemChunk *current_chunk;
	uint totle_mempool_size;
	uint used_mempool_size;
	uint free_mempool_size;
	uint chunk_size;
	uint chunk_count;
	uint ref;
	bool init_memdata;
	uint mini_mem_to_alloc;
}MemPool;


#endif