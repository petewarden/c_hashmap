This is a simple C hashmap, using strings for the keys.

Originally based on code by Eliot Back at http://elliottback.com/wp/hashmap-implementation-in-c/

Reworked by Pete Warden - http://petewarden.typepad.com/searchbrowser/2010/01/c-hashmap.html

Zaks Wang

        1.fix bug that put same key the map value will increase

        2.add feature that you can change hash function

        You can chose SIMPLE_HASH RS_HASH JS_HASH PJW_HASH ELF_HASH BKDR_HASH DJB_HASH AP_HASH
        CRC_HAHS

main.c contains an example that tests the functionality of the hashmap module.

To compile it, run something like this on your system:

gcc main.c hashmap.c hash.c -o hashmaptest

There are no restrictions on how you reuse this code.

hash_func_test
##############

        一个字符串hash函数的评测,原文http://blog.csdn.net/liuben/article/details/5050697
        实际语料测试结果，BKDR_HASH远远高于其他HASH函数，其次是AP_HASH
        如果冲突，建议将MAX_CHAIN_LENGTH设置稍大

hashMap
#######

        cheungmine修改版hashmap http://blog.csdn.net/cheungmine/article/details/7704686


