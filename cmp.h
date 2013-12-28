#ifndef __CMP_HEADER__
#define __CMP_HEADER__

#include "defines.h"
#include "chunk.h"
#include "pool.h"


EXPORT_F void cmp_init_mempool(uint init_poolsize, uint chunk_size, uint mini_mem_to_alloc, bool init_mem_data);
EXPORT_F void cmp_destroy_mempool();
EXPORT_F void *cmp_malloc(const uint memsize);
EXPORT_F void cmp_free(void *mem_block);
EXPORT_F bool gen_mem_dump(const schar *file);
EXPORT_F bool is_valid_point(void *point);

INTERNAL_F bool alloc_mem(const uint mem_block_size);
INTERNAL_F uint calc_needed_chunks(const uint mem_block_size);
INTERNAL_F uint calc_mem_block_size(const uint mem_block_size);
INTERNAL_F bool re_calc_mem_block_size(MemChunk *chunk, uint chunk_count);
INTERNAL_F bool bind_chunk_to_data(MemChunk* chunk, uint chunk_count, uchar *p_mem_block);
INTERNAL_F MemPool* set_pool(MemPool *pool);
INTERNAL_F MemChunk* set_chunk(MemChunk *chunk);
INTERNAL_F void deallocate_all_memblock();
INTERNAL_F void deallocate_all_chunks();
INTERNAL_F void free_chunks(MemChunk *chunk);
INTERNAL_F void update_chunk(MemChunk *chunk, uint memsize);
INTERNAL_F MemChunk *find_suit_chunk(uint memsize);
INTERNAL_F MemChunk *find_mem_chunk_point(void *mem_block);
INTERNAL_F MemChunk *skip_chunk(MemChunk *chunk, uint skip);


#endif