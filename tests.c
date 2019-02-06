#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "hashmap.h"

/*
 * This test cover problem reported at https://github.com/petewarden/c_hashmap/issues/6 .
 */
void test_key_duplication_issue()
{
	int status;
	map_t hmap;
	unsigned int colliding_keys_hash;
	char *colliding_key1 = "123";
	char *colliding_key2 = "157";
	char *res;

	printf("Running test_key_duplication_issue...");

	hmap = hashmap_new();

	/* 1. Preparation: */
	/* - Check if two provided keys are colliding (same hash value). */
	colliding_keys_hash = hashmap_hash_int(hmap, colliding_key1);

	/* - If assertion below is failing, this mean that hash function */
	/*   has beed updated and keys are not collidinig any more. */
	/*   Provided keys colliding_key1, colliding_key2 have to be updated, */
	/*   so they collide again. */
	assert(hashmap_hash_int(hmap, colliding_key2) == colliding_keys_hash);

	/* 2. Setup data in hash map: */
	/* - Put test data into map. Two colliding keys (same hash value). */
	status = hashmap_put(hmap, colliding_key1, colliding_key1);
	assert(status==MAP_OK);

	status = hashmap_put(hmap, colliding_key2, colliding_key2);
	assert(status==MAP_OK);

	/* - Check if keys exists in hash map. */
	assert(hashmap_get(hmap, colliding_key1, (void **)(&res)) == MAP_OK);
	assert(res == colliding_key1);

	assert(hashmap_get(hmap, colliding_key2, (void **)(&res)) == MAP_OK);
	assert(res == colliding_key2);


	/* 3. Remove first key - colliding_key1. */
	assert(hashmap_remove(hmap, colliding_key1) == MAP_OK);

	/* - Check if colliding_key2 is present. */
	assert(hashmap_get(hmap, colliding_key2, (void **)(&res)) == MAP_OK);

	/* 4. Put again colliding_key2. If problem occur then in hash map there */
	/*    can be placed key duplicate. */
	assert(hashmap_put(hmap, colliding_key2, colliding_key2) == MAP_OK);

	/* 5. Remove colliding_key2 from hash map. Only first instance will be removed. */
	assert(hashmap_remove(hmap, colliding_key2) == MAP_OK);

	/* 6. Check if colliding_key2 exists in hash map. */
	/*    After calling hashmap_remove() user expect not to find key. */
	/*    Should be removed, but still present in map. */
	assert(hashmap_get(hmap, colliding_key2, (void **)(&res)) == MAP_MISSING);

	/* Cleanup. */
	hashmap_free(hmap);

	printf("ok\n");
}

/*
 * This test cover problem reported at https://github.com/petewarden/c_hashmap/issues/8 .
 */
void test_map_size_increase_on_entry_overwrite_issue()
{
	int status;
	map_t hmap;
	char *test_key = "123";

	printf("Running test_map_size_increase_on_entry_overwrite_issue...");

	/* 1. Check initial size of hashmap. */
	hmap = hashmap_new();
	assert(hashmap_length(hmap) == 0);

	/* 2. Add single element and check map size. */
	status = hashmap_put(hmap, test_key, test_key);
	assert(status==MAP_OK);
	assert(hashmap_length(hmap) == 1);

	/* 3. Add again same element. */
	status = hashmap_put(hmap, test_key, test_key);
	assert(status==MAP_OK);

	/* 4. No change in size is expected on same key insert. */
	assert(hashmap_length(hmap) == 1);

	/* Cleanup. */
	hashmap_free(hmap);

	printf("ok\n");
}

/*
 * For quick run all tests call in command line:
 *   gcc tests.c hashmap.c -o tests && ./tests
 */
int main(char* argv, int argc)
{
	test_key_duplication_issue();
	test_map_size_increase_on_entry_overwrite_issue();
}