#include "dhash.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define  SIZE  128

static elem_t copy_elem_func(elem_t elem)
{
	int *copy = malloc(sizeof(int));
	*copy = *(int *)elem;
	return copy;
}

static int elem_equal_func(elem_t *a, elem_t *b)
{
	return (*(int *)a) == (*(int *)b);
}

static void free_elem_func(elem_t elem)
{
	free(elem);
}

static int hash_func(elem_t elem)
{
	return *(int *)elem;
}

static void show_elem_func(elem_t elem)
{
	printf("%d\t", *(int *)elem);
}

int main()
{
	dhash_t *dhash;
	int elem[SIZE];
	int i;

	dhash = dhash_init(0);
	if (!dhash)
		return -1;
	set_hash_func(dhash, hash_func);
	set_free_elem_func(dhash, free_elem_func);
	set_elem_equal_func(dhash, elem_equal_func);
	set_copy_elem_func(dhash, copy_elem_func);
	set_show_elem_func(dhash, show_elem_func);
	srand(time(NULL));
	for (i = 0; i < SIZE; i++) {
		elem[i] = rand();
		if (dhash_insert(dhash, &elem[i]) < 0)
			printf("insert %d failed\n", elem[i]);
	}
	printf("hash table:\n");
	dhash_show(dhash);
	for (i = 0; i < SIZE; i++) {
		assert(NULL != dhash_query(dhash, &elem[i]));
		dhash_delete(dhash, &elem[i]);
	}
	dhash_destroy(dhash);
	return 0;
}

