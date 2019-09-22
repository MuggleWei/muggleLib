/*
 *	author: muggle wei <mugglewei@gmail.com>
 *
 *	Use of this source code is governed by the MIT license that can be
 *	found in the LICENSE file.
 */

#ifndef MUGGLE_C_MEMORY_POOL_H_
#define MUGGLE_C_MEMORY_POOL_H_

#include "muggle/c/base/macro.h"
#include <stdbool.h>

EXTERN_C_BEGIN

// memory pool flag
#define MUGGLE_MEMORY_POOL_CONSTANT_SIZE	0x01

typedef struct MuggleMemoryPool_tag
{
	void**			memory_pool_data_bufs;  // data buffer array
	void**			memory_pool_ptr_buf;    // pointer buffer

	unsigned int	alloc_index;            // next time, alloc pointer index in pointer buffer
	unsigned int	free_index;             // next time, free pointer index in pointer buffer

	unsigned int	capacity;               // current memory pool capacity
	unsigned int	used;                   // how many block in use

	unsigned int	block_size;             // size of single block
	unsigned int	num_buf;                // the number of data buffer

	unsigned int	flag;					// flags

#if MUGGLE_DEBUG
	unsigned int	peak;                   // record max number of block in use
#endif
}MuggleMemoryPool;

MUGGLE_CC_EXPORT
bool MuggleMemoryPoolInit(MuggleMemoryPool* pool, unsigned int init_capacity, unsigned int block_size);

MUGGLE_CC_EXPORT
void MuggleMemoryPoolDestroy(MuggleMemoryPool* pool);

MUGGLE_CC_EXPORT 
void* MuggleMemoryPoolAlloc(MuggleMemoryPool* pool);

MUGGLE_CC_EXPORT 
void MuggleMemoryPoolFree(MuggleMemoryPool* pool, void* p_data);

MUGGLE_CC_EXPORT 
bool MuggleMemoryPoolEnsureSpace(MuggleMemoryPool* pool, unsigned int capacity);

MUGGLE_CC_EXPORT
unsigned int MuggleMemoryPoolGetFlag(MuggleMemoryPool* pool);

MUGGLE_CC_EXPORT
void MuggleMemoryPoolSetFlag(MuggleMemoryPool* pool, unsigned int flag);

EXTERN_C_END

#endif