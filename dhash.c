#include "dhash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MASK(n)  ((1 << (n)) - 1)

static elem_t default_copy_elem_func(elem_t elem)
{
	return elem;
}

static int default_elem_equal_func(elem_t *a, elem_t *b)
{
	return a == b;
}

static void default_free_elem_func(elem_t elem)
{}

static int default_hash_func(elem_t elem)
{
	return 0;
}

static void default_show_elem_func(elem_t elem)
{
	printf("%p\t", elem);
}

static item_t *item_alloc(int block_len)
{
	item_t *item = malloc(sizeof(item_t));
	if (!item)
		return NULL;
	memset(item, 0, sizeof(item_t));
	item->block.data = malloc(block_len * sizeof(elem_t));
	if (!item->block.data)
		return NULL;
	memset(item->block.data, 0, block_len * sizeof(elem_t));
	return item;
}

/*
** @bucket_len: number of elements which a bucket can contain, 
** 0 means default value.
** return NULL if init failed else dhash instance.
*/
dhash_t *dhash_init(int block_len)
{
	int i, size = 1 << INIT_HASH_TABLE_PREFIX;
	dhash_t *dhash = malloc(sizeof(dhash_t));
	if (!dhash)
		return NULL;
	memset(dhash, 0, sizeof(dhash_t));
	if (block_len == 0)
		block_len = DEFAULT_BLOCK_LEN;
	dhash->table = malloc(size * sizeof(item_t *));
	if (!dhash->table)
		return NULL;
	for (i = 0; i < size; i++) {
		dhash->table[i] = item_alloc(block_len);
		if (!dhash->table[i])
			return NULL;
		dhash->table[i]->hash_prefix = INIT_HASH_TABLE_PREFIX;
		dhash->table[i]->ref_count = 1;
	}
	dhash->block_len = block_len;
	dhash->hash_prefix = INIT_HASH_TABLE_PREFIX;
	dhash->elem_equal_func = default_elem_equal_func;
	dhash->copy_elem_func = default_copy_elem_func;
	dhash->free_elem_func = default_free_elem_func;
	dhash->hash_func = default_hash_func;
	dhash->show_elem_func = default_show_elem_func;
	return dhash;
}

static void dhash_destroy_block(dhash_t *dhash, block_t *block)
{
	block_t *next;
	int i;
	
	while (block) {
		for (i = 0; i < dhash->block_len; i++) {
			if (block->data[i])
				dhash->free_elem_func(block->data[i]);
		}
		free(block->data);
		next = block->next;
		free(block);
		block = next;
	}
}

void dhash_destroy(dhash_t *dhash)
{
	int i, j;
	int size = 1 << dhash->hash_prefix;
	block_t *block;
	
	for (i = 0; i < size; i++) {
		if (--dhash->table[i]->ref_count == 0) {
			block = &dhash->table[i]->block;
			for (j = 0; j < dhash->block_len; j++) {
				if (block->data[j])
					dhash->free_elem_func(block->data[j]);
			}
			free(block->data);
			dhash_destroy_block(dhash, block->next);
			free(dhash->table[i]);
		}
	}
	free(dhash->table);
	free(dhash);
}

void set_hash_func(dhash_t *dhash, hash_func_t func)	
{
	dhash->hash_func = func;
}

void set_free_elem_func(dhash_t *dhash, free_elem_func_t func)
{
	dhash->free_elem_func = func;
}

void set_elem_equal_func(dhash_t *dhash, elem_equal_func_t func)
{
	dhash->elem_equal_func = func;
}

void set_copy_elem_func(dhash_t *dhash, copy_elem_func_t func)
{
	dhash->copy_elem_func = func;
}

void set_show_elem_func(dhash_t *dhash, show_elem_func_t func)
{
	dhash->show_elem_func = func;
}

static elem_t* dhash_query_block(dhash_t *dhash, block_t *block, elem_t elem)
{
	while (block) {
		int i;
		for (i = 0; i < dhash->block_len; i++) {
			if (!block->data[i])
				continue;
			if (dhash->elem_equal_func(block->data[i], elem))
				return &block->data[i];
		}
		block = block->next;
	}
	return NULL;
}

elem_t* dhash_query(dhash_t *dhash, elem_t elem)
{
	int slot;
	
	slot = dhash->hash_func(elem) & MASK(dhash->hash_prefix);
	return dhash_query_block(dhash, &dhash->table[slot]->block, elem);
}

static int add_overflow_block(dhash_t *dhash, elem_t elem, int slot)
{
	block_t **block = &dhash->table[slot]->block.next;
	while (*block) {
		int i;
		for (i = 0; i < dhash->block_len; i++) {
 			if (!(*block)->data[i]) {
				(*block)->data[i] = dhash->copy_elem_func(elem);
				return 0;
			}
		}
		block= &(*block)->next;
	}
	*block = malloc(sizeof(block_t));
	if (!*block)
		return -1;
	memset(*block, 0, sizeof(block_t));
	(*block)->data = malloc(dhash->block_len * sizeof(elem_t));
	if (!(*block)->data)
		return -1;
	memset((*block)->data, 0, dhash->block_len * sizeof(elem_t));
	(*block)->data[0] = dhash->copy_elem_func(elem);
	return 0;
}

static int divide_bucket(dhash_t *dhash, elem_t elem, int slot)
{
	int i, table_size, table_prefix, overflow;
	int new_slot, new_size;
	int num_items, count, middle_slot;
	item_t *top_half, *bottom_half;
	item_t *item, **table;

	while (1) {
		table_prefix = dhash->hash_prefix;
		table_size = 1 << table_prefix;
		if (table_prefix == dhash->table[slot]->hash_prefix) {
			dhash->hash_prefix++;
			new_size = 1 << dhash->hash_prefix;
			table = realloc(dhash->table, new_size * sizeof(item_t *));
			if (!table)
				return -1;
			dhash->table = table;
			for (i = 0; i < table_size; i++) {
				dhash->table[i + table_size] = dhash->table[i];
				dhash->table[i]->ref_count++;
			}
			table_size = new_size;
			slot = dhash->hash_func(elem) & MASK(dhash->hash_prefix);
		}

		num_items = 1 << (dhash->hash_prefix - dhash->table[slot]->hash_prefix);
		count = 0;
		for (i = 0; i < table_size; i++) {
			if (dhash->table[i] ==  dhash->table[slot]) {
				count++;
				if (count == num_items / 2)
					break;
			}
		}
		middle_slot = i;
		
		//divide item in slot
		item = item_alloc(dhash->block_len);
		if (!item)
			return -1;
		item->hash_prefix = dhash->table[slot]->hash_prefix;
		item->ref_count = 0;
		if (dhash->table[slot]->block.next) {
			new_slot = dhash->hash_func(dhash->table[slot]->block.data[0]) & 
				MASK(dhash->hash_prefix);
			if (new_slot <= middle_slot) {
				top_half = dhash->table[slot];
				bottom_half = item;
			} else {
				top_half = item;
				bottom_half = dhash->table[slot];
			}
		} else {
			int j = 0;
			top_half = item;
			bottom_half = dhash->table[slot];
			for (i = 0; i < dhash->block_len; i++) {
				new_slot = dhash->hash_func(dhash->table[slot]->block.data[i]) & 
					MASK(dhash->hash_prefix);
				if (new_slot <= middle_slot) {
					assert(j < dhash->block_len);
					top_half->block.data[j++] = dhash->table[slot]->block.data[i];
					dhash->table[slot]->block.data[i] = NULL;
				}
			}
		}

		item = dhash->table[slot];
		top_half->ref_count = 0;
		bottom_half->ref_count = 0;
		for (i = 0; i < table_size; i++) {
			if (dhash->table[i] == item) {
				if (i <= middle_slot) {
					dhash->table[i] = top_half;
					top_half->ref_count++;
				} else {
					dhash->table[i] = bottom_half;
					bottom_half->ref_count++;
				}
			}
		}
		top_half->hash_prefix++;
		bottom_half->hash_prefix++;
		
		overflow = 1;
		for (i = 0; i < dhash->block_len; i++) {
			block_t *block = &dhash->table[slot]->block;
			if (!block->data[i]) {
				block->data[i] = dhash->copy_elem_func(elem);
				return 0;
			}
			if (dhash->hash_func(elem) != dhash->hash_func(block->data[i]))
				overflow = 0;
		}
		if (overflow) {
			return add_overflow_block(dhash, elem, slot);
		}
	}
	//can not be reached
	return 0;
}

int dhash_insert(dhash_t *dhash, elem_t elem)
{
	int slot, i, overflow = 1;
	
	slot = dhash->hash_func(elem) & MASK(dhash->hash_prefix);
	for (i = 0; i < dhash->block_len; i++) {
		block_t *block = &dhash->table[slot]->block;
		if (!block->data[i]) {
			block->data[i] = dhash->copy_elem_func(elem);
			return 0;
		}
		if (dhash->hash_func(elem) != dhash->hash_func(block->data[i]))
			overflow = 0;
	}
	if (overflow) {
		return add_overflow_block(dhash, elem, slot);
	}
	//bucket is full, do bucket division.
	return divide_bucket(dhash, elem, slot);
}

int dhash_delete(dhash_t *dhash, elem_t elem)
{
	elem_t* item = dhash_query(dhash, elem);
	
	if (!item)
		return -1;
	dhash->free_elem_func(*item);
	*item = NULL;
	return 0;
}

static void dhash_show_block(dhash_t *dhash, block_t *block)
{
	while (block) {
		int i;
		for (i = 0; i < dhash->block_len; i++) {
			if (!block->data[i])
				continue;
			dhash->show_elem_func(block->data[i]);
		}
		block = block->next;
	}
}

void dhash_show(dhash_t *dhash)
{
	int size = 1 << dhash->hash_prefix;
	int i;

	for (i = 0; i < size; i++) {
		printf("slot %d: ", i);
		dhash_show_block(dhash, &dhash->table[i]->block);
		printf("\n");
	}
}

