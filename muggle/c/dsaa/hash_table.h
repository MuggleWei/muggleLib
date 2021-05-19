/*
 *	author: muggle wei <mugglewei@gmail.com>
 *
 *	Use of this source code is governed by the MIT license that can be
 *	found in the LICENSE file.
 */

#ifndef MUGGLE_C_DSAA_HASH_TABLE_H_
#define MUGGLE_C_DSAA_HASH_TABLE_H_

#include "muggle/c/dsaa/dsaa_utils.h"

EXTERN_C_BEGIN

typedef uint64_t (*hash_func)(void *data);

typedef struct muggle_hash_table_node
{
	struct muggle_hash_table_node *prev;  // prev node
	struct muggle_hash_table_node *next;  // next node
	void                          *key;   // key data of node
	void                          *value; // value data of node
}muggle_hash_table_node_t;

typedef struct muggle_hash_table
{
	muggle_hash_table_node_t *nodes;      // node array
	uint64_t                 table_size;  // size of hash table
	hash_func                hash;        // pointer to hash function
	muggle_dsaa_data_cmp     cmp;         // pointer to compare function
	muggle_memory_pool_t     *pool;       // memory pool of tree, if it's NULL, use malloc and free by default
}muggle_hash_table_t;

#define HASH_TABLE_SIZE_10007 10007

// initialize hash table
// @param p_hash_table  pointer to hash table
// @param table_size    table size
// @param hash          hash function, if it's NULL, use default hash function
// @param cmp           compare function for key
// @param capacity      init capacity for nodes memory pool, if 0, don't use memory pool
MUGGLE_CC_EXPORT
bool muggle_hash_table_init(muggle_hash_table_t *p_hash_table, size_t table_size, hash_func hash, muggle_dsaa_data_cmp cmp, size_t capacity);

// destroy hash table
// @param p_hash_table      pointer to hash table
// @param key_func_free     function for free key data, if it's NULL, do nothing for key data
// @param key_pool          the memory pool passed to key_func_free
// @param value_func_free   function for free value data, if it's NULL, do nothing for value data
// @param value_pool        the memory pool passed to value_func_free
MUGGLE_CC_EXPORT
void muggle_hash_table_destroy(muggle_hash_table_t *p_hash_table,
	muggle_dsaa_data_free key_func_free, void *key_pool,
	muggle_dsaa_data_free value_func_free, void *value_pool);

// clear hash table
// @param p_hash_table      pointer to hash table
// @param key_func_free     function for free key data, if it's NULL, do nothing for key data
// @param key_pool          the memory pool passed to key_func_free
// @param value_func_free   function for free value data, if it's NULL, do nothing for value data
// @param value_pool        the memory pool passed to value_func_free
MUGGLE_CC_EXPORT
void muggle_hash_table_clear(muggle_hash_table_t *p_hash_table,
	muggle_dsaa_data_free key_func_free, void *key_pool,
	muggle_dsaa_data_free value_func_free, void *value_pool);

// find node in hash table
// @param p_hash_table      pointer to hash table
// @param key               the key that want to found
// @return the node that found, if failed, return NULL
MUGGLE_CC_EXPORT
muggle_hash_table_node_t* muggle_hash_table_find(muggle_hash_table_t *p_hash_table, void *key);

// put data into hash table
// @param p_hash_table      pointer to hash table
// @param key               key
// @param value             value
// @return return generated node contain added data, if NULL, failed insert data
MUGGLE_CC_EXPORT
muggle_hash_table_node_t* muggle_hash_table_put(muggle_hash_table_t *p_hash_table, void *key, void *value);

// remove data in hash table
// @param p_hash_table      pointer to hash table
// @param node              node need to remove
// @param key_func_free     function for free key data, if it's NULL, do nothing for key data
// @param key_pool          the memory pool passed to key_func_free
// @param value_func_free   function for free value data, if it's NULL, do nothing for value data
// @param value_pool        the memory pool passed to value_func_free
MUGGLE_CC_EXPORT
void muggle_hash_table_remove(
	muggle_hash_table_t *p_hash_table, muggle_hash_table_node_t *node,
	muggle_dsaa_data_free key_func_free, void *key_pool,
	muggle_dsaa_data_free value_func_free, void *value_pool);

EXTERN_C_END

#endif