all:int_hash_test str_hash_test

int_hash_test:dhash.c dhash.h int_hash_test.c
	gcc -g -o int_hash_test dhash.c int_hash_test.c

str_hash_test:dhash.c dhash.h str_hash_test.c
	gcc -g -o str_hash_test dhash.c str_hash_test.c
	
clean:
	-rm int_hash_test str_hash_test

.PHONY:clean