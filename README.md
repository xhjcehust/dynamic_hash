# dynamic_hash
This project is a C implementation of dynamic hash.

# 接口说明

dhash_t *dhash_init(int block_len)

初始化构造dhash_t结构体，用于表示动态hash表，初始化成功返回dhash_t实例，否则返回NULL。

void dhash_destroy(dhash_t *dhash)

销毁动态hash表，释放hash表内存，执行一些清理工作。

void set_hash_func(dhash_t *dhash, hash_func_t func)

void set_free_elem_func(dhash_t *dhash, free_elem_func_t func)

void set_elem_equal_func(dhash_t *dhash, elem_equal_func_t func)

void set_copy_elem_func(dhash_t *dhash, copy_elem_func_t func)

void set_show_elem_func(dhash_t *dhash, show_elem_func_t func)

分别为动态hash表设置hash函数指针、元素释放指针、判断元素相等函数指针、拷贝元素函数指针、打印hash表函数指针。

elem_t* dhash_query(dhash_t *dhash, elem_t elem)

在hash表中查找指定元素，存在则返回元素指针，否则返回NULL。

int dhash_insert(dhash_t *dhash, elem_t elem)

在hash表中插入指定元素，成功返回0，否则返回-1。

int dhash_delete(dhash_t *dhash, elem_t elem)

在hash表中删除指定元素，成功返回0，否则返回-1。

void dhash_show(dhash_t *dhash)

打印hash表中所有的元素。


$ make

Then you will get two test files: int_hash_test and str_hash_test.

For more informations, see http://blog.csdn.net/xhjcehust/article/details/51645418 