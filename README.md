# dynamic_hash
This project is a C implementation of dynamic hash.

# �ӿ�˵��

dhash_t *dhash_init(int block_len)

��ʼ������dhash_t�ṹ�壬���ڱ�ʾ��̬hash����ʼ���ɹ�����dhash_tʵ�������򷵻�NULL��

void dhash_destroy(dhash_t *dhash)

���ٶ�̬hash���ͷ�hash���ڴ棬ִ��һЩ��������

void set_hash_func(dhash_t *dhash, hash_func_t func)

void set_free_elem_func(dhash_t *dhash, free_elem_func_t func)

void set_elem_equal_func(dhash_t *dhash, elem_equal_func_t func)

void set_copy_elem_func(dhash_t *dhash, copy_elem_func_t func)

void set_show_elem_func(dhash_t *dhash, show_elem_func_t func)

�ֱ�Ϊ��̬hash������hash����ָ�롢Ԫ���ͷ�ָ�롢�ж�Ԫ����Ⱥ���ָ�롢����Ԫ�غ���ָ�롢��ӡhash����ָ�롣

elem_t* dhash_query(dhash_t *dhash, elem_t elem)

��hash���в���ָ��Ԫ�أ������򷵻�Ԫ��ָ�룬���򷵻�NULL��

int dhash_insert(dhash_t *dhash, elem_t elem)

��hash���в���ָ��Ԫ�أ��ɹ�����0�����򷵻�-1��

int dhash_delete(dhash_t *dhash, elem_t elem)

��hash����ɾ��ָ��Ԫ�أ��ɹ�����0�����򷵻�-1��

void dhash_show(dhash_t *dhash)

��ӡhash�������е�Ԫ�ء�


$ make

Then you will get two test files: int_hash_test and str_hash_test.

For more informations, see http://blog.csdn.net/xhjcehust/article/details/51645418 