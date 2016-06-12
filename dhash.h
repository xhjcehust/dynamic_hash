#ifndef __DHASH_H__
#define __DHASH_H__

#define DEFAULT_BUCKET_LEN 4
#define INIT_HASH_TABLE_PREFIX 0

typedef void * elem_t;
typedef struct block_s block_t;

struct block_s{
	elem_t* data;
	block_t *next; //overflow block
};

typedef struct {
	int ref_count;
	int hash_prefix;
	block_t block;
} item_t;

typedef int (*hash_func_t)(elem_t elem);
typedef void (*free_elem_func_t)(elem_t elem);
/*
** return 1 if a equals to b else return 0.
*/
typedef int (*elem_equal_func_t)(elem_t *a, elem_t *b);
typedef elem_t (*copy_elem_func_t)(elem_t elem);
typedef void (*show_elem_func_t)(elem_t elem);

typedef struct {
	item_t **table;
	int block_len;
	int hash_prefix; //start from 1 and hash table size is 1 << (hash_prefix - 1)
	hash_func_t hash_func;
	free_elem_func_t free_elem_func;
	elem_equal_func_t elem_equal_func;
	copy_elem_func_t copy_elem_func;
	show_elem_func_t show_elem_func;
} dhash_t;

dhash_t *dhash_init(int block_len);
void dhash_destroy(dhash_t *dhash);
void set_hash_func(dhash_t *dhash, hash_func_t func);
void set_free_elem_func(dhash_t *dhash, free_elem_func_t func);
void set_elem_equal_func(dhash_t *dhash, elem_equal_func_t func);
void set_copy_elem_func(dhash_t *dhash, copy_elem_func_t func);
void set_show_elem_func(dhash_t *dhash, show_elem_func_t func);
elem_t* dhash_query(dhash_t *dhash, elem_t elem);
int dhash_insert(dhash_t *dhash, elem_t elem);
int dhash_delete(dhash_t *dhash, elem_t elem);
void dhash_show(dhash_t *dhash);

#endif

