This is a simple C hashmap, using strings for the keys.

Originally based on code by Eliot Back at http://elliottback.com/wp/hashmap-implementation-in-c/

Reworked by Pete Warden - http://petewarden.typepad.com/searchbrowser/2010/01/c-hashmap.html

Zaks Wang
        1.fix bug that put same key the map value will increase

        2.add feature that you can change hash function

main.c contains an example that tests the functionality of the hashmap module.

To compile it, run something like this on your system:

gcc main.c hashmap.c hash.c -o hashmaptest

There are no restrictions on how you reuse this code.

hash_func_test
##############

        一个字符串hash函数的评测,原文http://blog.csdn.net/liuben/article/details/5050697
hashMap
#######

        cheungmine修改版hashmap http://blog.csdn.net/cheungmine/article/details/7704686


