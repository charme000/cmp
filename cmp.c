#include "cmp.h"

MemPool gMemPool;

EXPORT_F void 
cmp_init_mempool(uint init_poolsize, uint chunk_size, uint mini_mem_to_alloc, bool init_mem_data)
{
	if ((0 == init_poolsize)
		&& (0 == chunk_size)
		&& (0 == mini_mem_to_alloc)
		)
	{
		set_pool(&gMemPool);
		alloc_mem(DEFAULT_MEMORY_POOL_SIZE);
	}
	else
	{
		gMemPool.first_chunk = NULL;
		gMemPool.last_chunk = NULL;
		gMemPool.current_chunk = NULL;

		gMemPool.totle_mempool_size = 0;
		gMemPool.used_mempool_size = 0;
		gMemPool.free_mempool_size = 0;

		gMemPool.chunk_size = chunk_size;
		gMemPool.chunk_count = 0;
		gMemPool.ref = 0;

		gMemPool.init_memdata = init_mem_data;
		gMemPool.mini_mem_to_alloc = mini_mem_to_alloc;

		alloc_mem(init_poolsize);
	}	
}

EXPORT_F void 
cmp_destroy_mempool()
{
	deallocate_all_memblock();
	deallocate_all_chunks();

	assert((gMemPool.ref == 0) && "WARNING : Memory-Leak : You have not freed all allocated Memory");
}

EXPORT_F void 
*cmp_malloc(const uint memsize)
{
	uint size = calc_mem_block_size(memsize);
	MemChunk *chunk = NULL;

	while (!chunk)
	{
		chunk = find_suit_chunk(size);
		if (!chunk)
		{
			size = max_value(size, calc_mem_block_size(gMemPool.mini_mem_to_alloc));

			alloc_mem(size);
		}
	}

	gMemPool.used_mempool_size += size;
	gMemPool.free_mempool_size -= size;
	gMemPool.ref++;

	update_chunk(chunk, size);

	return ((void *) chunk->data);
}

EXPORT_F void 
cmp_free(void *mem_block)
{
	MemChunk *chunk = find_mem_chunk_point(mem_block);

	if (chunk == gMemPool.last_chunk)
	{
		chunk = gMemPool.first_chunk;
	}
	else if (chunk)
	{
		free_chunks(chunk);
	}
	else
	{
		assert(false && "ERROR : Requested Pointer not in Memory Pool");
	}
	assert((gMemPool.ref > 0) && "ERROR : Request to delete more Memory then allocated.");

	gMemPool.ref--;
}

EXPORT_F bool 
gen_mem_dump(const schar *file)
{
	bool success = false;
	int write_byte = 0;

	int fd = open(file, O_WRONLY | O_CREAT | O_APPEND);

	MemChunk *current_chunk = gMemPool.first_chunk;

	if (fd == (-1))
		return false;

	while (current_chunk)
	{
		write_byte = write(fd, ((schar *)current_chunk->data), gMemPool.chunk_size);

		if (write_byte == gMemPool.chunk_size)
		{
			success = true;
		}

		current_chunk = current_chunk->next;
	}
	close(fd);

	return success;
}

EXPORT_F bool 
is_valid_point(void *point)
{
	MemChunk *chunk = gMemPool.first_chunk;

	while (chunk)
	{
		if (chunk->data == ((uchar *)point))
		{
			return true;
		}
		chunk = chunk->next;
	}
	
	return false;
}

INTERNAL_F void 
free_chunks(MemChunk *chunk)
{
	MemChunk *current_chunk = chunk;
	uint chunk_count = calc_needed_chunks(current_chunk->used_size);

	for (uint i = 0; i < chunk_count; ++i)
	{
		if (current_chunk)
		{
			if (gMemPool.init_memdata)
			{
				memset(((void *)current_chunk->data), FREEED_MEMORY, gMemPool.chunk_size);
			}

			current_chunk->used_size = 0;

			gMemPool.free_mempool_size += gMemPool.chunk_size;
			gMemPool.used_mempool_size -= gMemPool.chunk_size;

			current_chunk = current_chunk->next;
		}
	}
}

INTERNAL_F MemChunk *
find_mem_chunk_point(void *mem_block)
{
	MemChunk *chunk_tmp = gMemPool.first_chunk;

	while (chunk_tmp)
	{
		if (chunk_tmp->data == ((uchar*)mem_block))
		{
			break;
		}
		chunk_tmp = chunk_tmp->next;
	}

	return chunk_tmp;
}

INTERNAL_F void 
update_chunk(MemChunk *chunk, uint memsize)
{
	if (chunk)
	{
		chunk->used_size = memsize;
	}
	else
	{
		assert(false && "Error : Invalid NULL-Pointer passed");
	}
}

INTERNAL_F MemChunk *
find_suit_chunk(uint memsize)
{
	uint chunk_skip = 0;
	bool search = true;

	MemChunk *chunk = gMemPool.current_chunk;
	MemChunk *chunk_tmp = NULL;
	uint can_use = 0;

	for (uint i = 0; i < gMemPool.chunk_count; ++i)
	{
		if ((chunk) && (true == search))
		{
			if (chunk == gMemPool.last_chunk)
			{
				chunk = gMemPool.first_chunk;
			}

			if (chunk->data_size >= memsize)
			{
				if (chunk->used_size == 0)
				{
					if ((chunk->data == gMemPool.first_chunk->data)
						&& (gMemPool.chunk_size < memsize)
						)
					{
						can_use = gMemPool.chunk_size;
						chunk_tmp = chunk;

						while (chunk_tmp->next)
						{
							chunk_tmp = chunk_tmp->next;

							if (chunk_tmp->used_size == 0)
							{
								can_use += gMemPool.chunk_size;

								if (can_use >= memsize) break;
							}
							else break;
						}
						if (can_use >= memsize)
						{
							gMemPool.current_chunk = chunk;
							return chunk;
						}
					}
					else
					{
						gMemPool.current_chunk = chunk;
						return chunk;
					}
				}
			}

			chunk_skip = calc_needed_chunks(chunk->used_size);

			if (chunk_skip == 0) chunk_skip = 1;

			chunk = skip_chunk(chunk, chunk_skip);
		}
		else
		{
			search = false;
		}
	}

	return NULL;
}

INTERNAL_F MemChunk *
skip_chunk(MemChunk *chunk, uint skip)
{
	MemChunk *current_chunk = chunk;

	for (uint i = 0; i < skip; ++i)
	{
		if (current_chunk == gMemPool.last_chunk)
		{
			current_chunk = gMemPool.first_chunk;
		}
		else if (current_chunk)
		{
			current_chunk = current_chunk->next;
		}
		else
		{
			assert(false && "Error : Chunk == NULL was not expected.");
			break;
		}
	}

	return current_chunk;
}

INTERNAL_F void 
deallocate_all_memblock()
{
	MemChunk *p_chunk = gMemPool.first_chunk;

	while (p_chunk)
	{
		if (p_chunk->is_alloc_chunk)
		{
			free((void*)(p_chunk->data));
		}
		p_chunk = p_chunk->next;
	}
}

INTERNAL_F void 
deallocate_all_chunks()
{
	MemChunk *p_chunk = gMemPool.first_chunk;
	MemChunk *p_chunk_del = NULL;

	while (p_chunk)
	{
		if (p_chunk->is_alloc_chunk)
		{
			if (p_chunk_del)
			{
				free((void*)p_chunk_del);
			}
			p_chunk_del = p_chunk;
		}
		p_chunk = p_chunk->next;
	}

	if (p_chunk_del)
	{
		free(((void *)p_chunk_del));
		p_chunk_del = NULL;
	}
}

INTERNAL_F bool 
alloc_mem(const uint mem_block_size)
{
	uint needed_chunk = calc_needed_chunks(mem_block_size);
	uint block_size = calc_mem_block_size(mem_block_size);

	uchar *p_new_mem_block = (uchar*)malloc(block_size);
	MemChunk *p_new_chunk = (MemChunk*)malloc(needed_chunk * sizeof(MemChunk));

	assert(((p_new_mem_block) && (p_new_chunk)) && "Error : System ran out of Memory");

	gMemPool.totle_mempool_size += block_size;
	gMemPool.free_mempool_size += block_size;
	gMemPool.chunk_count += needed_chunk;

	if (true == gMemPool.init_memdata)
	{
		memset((void *)p_new_mem_block, ALLOCATED_MEMORY, block_size);
	}

	return bind_chunk_to_data(p_new_chunk, needed_chunk, p_new_mem_block);
}

INTERNAL_F uint 
calc_needed_chunks(const uint mem_block_size)
{
	float chunk_count = (float)(((float)mem_block_size) / ((float)gMemPool.chunk_size));

	return ((uint)ceil(chunk_count));
}

INTERNAL_F uint 
calc_mem_block_size(const uint mem_block_size)
{
	uint needed_chunk = calc_needed_chunks(mem_block_size);

	return (uint)(needed_chunk * gMemPool.chunk_size);
}

INTERNAL_F bool 
bind_chunk_to_data(MemChunk* chunk, uint chunk_count, uchar *p_mem_block)
{
	MemChunk *p_new_chunk = NULL;
	uint mem_off = 0;
	bool assign = false;

	for (uint i = 0; i < chunk_count; ++i)
	{
		if (!(gMemPool.first_chunk))
		{
			gMemPool.first_chunk = set_chunk(&(chunk[0]));
			gMemPool.last_chunk = gMemPool.first_chunk;
			gMemPool.current_chunk = gMemPool.first_chunk;
		}
		else
		{
			p_new_chunk = set_chunk(&(chunk[i]));
			gMemPool.last_chunk->next = p_new_chunk;
			gMemPool.last_chunk = p_new_chunk;
		}

		mem_off = (i * ((uint) gMemPool.chunk_size));
		gMemPool.last_chunk->data = &(p_mem_block[mem_off]);

		if (!assign)
		{
			gMemPool.last_chunk->is_alloc_chunk = true;
			assign = true;
		}
	}

	return re_calc_mem_block_size(gMemPool.first_chunk, chunk_count);
}

INTERNAL_F bool 
re_calc_mem_block_size(MemChunk *chunk, uint chunk_count)
{
	uint mem_off = 0;

	for (uint i = 0; i < chunk_count; ++i)
	{
		if (chunk)
		{
			mem_off = (i * ((uint) gMemPool.chunk_size));
			chunk->data_size = (((uint)gMemPool.totle_mempool_size) - mem_off);

			chunk = chunk->next;
		}
		else
		{
			assert(false && "Error : ptrChunk == NULL");
			return false;
		}
	}

	return true;
}

INTERNAL_F MemPool* 
set_pool(MemPool *pool)
{
	if (pool)
	{
		pool->chunk_count = 0;
		pool->chunk_size = DEFAULT_MEMORY_CHUNK_SIZE;
		pool->current_chunk = NULL;
		pool->first_chunk = NULL;
		pool->last_chunk = NULL;
		pool->free_mempool_size = 0;
		pool->init_memdata = DEFAULT_MEMORY_INITIALIZE;
		pool->mini_mem_to_alloc = DEFAULT_MEMORY_SIZE_TO_ALLOCATE;
		pool->ref = 0;
		pool->totle_mempool_size = 0;
		pool->used_mempool_size = 0;
	}

	return pool;
}

INTERNAL_F MemChunk* 
set_chunk(MemChunk *chunk)
{
	if (chunk)
	{
		chunk->data = NULL;
		chunk->data_size = 0;
		chunk->used_size = 0;
		chunk->is_alloc_chunk = false;
		chunk->next = NULL;
	}

	return chunk;
}