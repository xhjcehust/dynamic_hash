#include "dhash.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define  SIZE  1280

static elem_t copy_elem_func(elem_t elem)
{
	char *copy = strdup(elem);
	return copy;
}

static int elem_equal_func(elem_t *a, elem_t *b)
{
	return strcmp((const char *)a, (const char *)b) == 0;
}

static void free_elem_func(elem_t elem)
{
	free(elem);
}

static int hash_func(elem_t elem)
{
	int result = 0;
	char *str = elem;
	
	while (*str++) {
		result = result * 31 + *str;
	}
	return result;
}

static void show_elem_func(elem_t elem)
{
	printf("%s\t", (char *)elem);
}

static char *generate_random_string()
{
	int i, len = rand() % 32;
	char *str;

	str = malloc((len + 1) * sizeof(char));
	if (!str)
		return NULL;
	for (i = 0; i < len; i++) {
		str[i] = 'a' + rand() % ('z' - 'a');
	}
	str[i] = '\0';
	return str;
}

int main()
{
	dhash_t *dhash;
	char *elem[SIZE];
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
		elem[i] = generate_random_string();
		if (dhash_insert(dhash, elem[i]) < 0)
			printf("insert %s failed\n", elem[i]);
	}
	printf("hash table:\n");
	dhash_show(dhash);
	for (i = 0; i < SIZE; i++) {
		assert(NULL != dhash_query(dhash, elem[i]));
		dhash_delete(dhash, elem[i]);
	}
	dhash_destroy(dhash);
	for (i = 0; i < SIZE; i++)
		free(elem[i]);
	return 0;
}

