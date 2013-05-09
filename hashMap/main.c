/**
 * main.c
 *
 * Detecting memory leaks only for windows .  
 * Place the following snippet where leak to be tested: 
 * #if defined(_CRTDBG_MAP_ALLOC) 
 *    _CrtDumpMemoryLeaks(); 
 * #endif 
 */  
#if defined(WIN32) && defined(_DEBUG)  
  #ifndef   _CRTDBG_MAP_ALLOC  
    #pragma message( __FILE__": _CRTDBG_MAP_ALLOC defined only for DEBUG on Win32." )   
    #define _CRTDBG_MAP_ALLOC  
    #include<stdlib.h>   
    #include<crtdbg.h>  
  #endif  
#endif  

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "hashmap.h"

typedef struct userelem_t {
  char   key[20];
  char  *value;
} userelem;

typedef struct userdata_t {
  char   name[20];
  hmap_t map;  /* userelem map */
} userdata;

static int iter_elem(void* elem, void *arg) {
  userelem *el = (userelem *) elem;
  printf("key=%s; value=%s\n", el->key, el->value);
  return 0;
}

static int free_elem(void* elem, void *arg) {
  userelem *el = (userelem *) elem;
  free(el->value);
  free(el);
  return 0;
}

static int free_data(void* data, void *arg) {
  userdata *dat = (userdata *) data;
  /* 删除整个子 map */
  hashmap_destroy(dat->map, free_elem, 0);
  free(dat);
  return 0;
}

int main(int argc, char* argv[])
{
  hmap_t map;
  userdata  *dat;
  userelem  *el;
  int ret, i, j;

  /* 创建 hashmap */
  map = hashmap_create();

  /* 插入 hashmap 元素 */
  for (i=0; i<100; i++) {
    dat = (userdata *)malloc(sizeof(userdata));

    /* 创建子 hashmap */
    dat->map = hashmap_create();

    /* 插入子 hashmap 元素 */
    for (j=0; j<10; j++) {
      el = (userelem *)malloc(sizeof(userelem));
      sprintf(el->key, "%d", j);

      el->value = (char*) malloc(30);
      sprintf(el->value, "%d", j+1000);
      ret = hashmap_put(dat->map, el->key, el);
      assert(ret==HMAP_S_OK);
    }

    sprintf(dat->name, "%d", i);
    ret = hashmap_put(map, dat->name, dat);
    assert(ret==HMAP_S_OK);
  }

  printf("hashmap_size: %d\n", hashmap_size(map));

  /* 删除指定元素: key="10" */
  ret = hashmap_remove(map, "10", &dat);
  assert(ret==HMAP_S_OK);
  printf("hashmap_remove: name=%s. size=%d\n", dat->name, hashmap_size(map));
  hashmap_iterate(dat->map, iter_elem, 0);
  free_data(dat, 0);

  /* 删除指定元素: key="11" */
  ret = hashmap_remove(map, "11", &dat);
  assert(ret==HMAP_S_OK);
  printf("hashmap_remove: name=%s. size=%d\n", dat->name, hashmap_size(map));
  hashmap_iterate(dat->map, iter_elem, 0);
  free_data(dat, 0);

  /* 查询元素: key="99" */
  ret = hashmap_get(map, "99", &dat);
  assert(ret==HMAP_S_OK);
  printf("hashmap_get: name=%s. size=%d\n", dat->name, hashmap_size(map));
  hashmap_iterate(dat->map, iter_elem, 0);

  /* 删除整个 map */
  hashmap_destroy(map, free_data, 0);

  _CrtDumpMemoryLeaks();
  return 0;
}

