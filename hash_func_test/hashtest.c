#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include "hash.h"
#include "md5.h"

struct hash_key {
	unsigned char *key;
	struct hash_key *next; 
};

struct hash_counter_entry {
	unsigned int hit_count;
	unsigned int entry_count;
	struct hash_key *keys;
};

#define BLOCK_LEN	4096

static int backet_len = 10240;
static int hash_call_count = 0;
static struct hash_counter_entry *hlist = NULL;
unsigned int (*hash_func)(char *str);

void choose_hash_func(char *hash_func_name)
{
	if (0 == strcmp(hash_func_name, "simple_hash"))
		hash_func = simple_hash;
	else if (0 == strcmp(hash_func_name, "RS_hash"))
		hash_func = RS_hash;
	else if (0 == strcmp(hash_func_name, "JS_hash"))
		hash_func = JS_hash;
	else if (0 == strcmp(hash_func_name, "PJW_hash"))
		hash_func = PJW_hash;
	else if (0 == strcmp(hash_func_name, "ELF_hash"))
		hash_func = ELF_hash;
	else if (0 == strcmp(hash_func_name, "BKDR_hash"))
		hash_func = BKDR_hash;
	else if (0 == strcmp(hash_func_name, "SDBM_hash"))
		hash_func = SDBM_hash;
	else if (0 == strcmp(hash_func_name, "DJB_hash"))
		hash_func = DJB_hash;
	else if (0 == strcmp(hash_func_name, "AP_hash"))
		hash_func = AP_hash;
	else if (0 == strcmp(hash_func_name, "CRC_hash"))
		hash_func = CRC_hash;
	else
		hash_func = NULL;
}

void insert_hash_entry(unsigned char *key, struct hash_counter_entry *hlist)
{
	unsigned int hash_value = hash_func(key) % backet_len;
	struct hash_key *p;

	p = hlist[hash_value].keys;
	while(p) {
		if (0 == strcmp(key, p->key))
			break;
		p = p->next;
	}
	if (p == NULL)
	{
		p = (struct hash_key *)malloc(sizeof(struct hash_key));
		if (p == NULL) 
		{
			perror("malloc in insert_hash_entry");
			return;
		}
		p->key = strdup(key);
		p->next = hlist[hash_value].keys;
		hlist[hash_value].keys = p;
		hlist[hash_value].entry_count++;
	}
	hlist[hash_value].hit_count++;
}

void hashtest_init()
{
	int i;
	
	hash_call_count = 0;
	hlist = (struct hash_counter_entry *) malloc (sizeof(struct hash_counter_entry) *  backet_len);
	if (NULL == hlist)
	{
		perror("malloc in hashtest_init");
		return;
	}
	for (i = 0; i < backet_len; i++)
	{
		hlist[i].hit_count = 0;
		hlist[i].entry_count = 0;
		hlist[i].keys = NULL;
	}
}

void hashtest_clean()
{
	int i;
	struct hash_key *pentry, *p;

	if (NULL == hlist)
		return;

	for (i = 0; i < backet_len; ++i)
	{
		pentry = hlist[i].keys;
		while(pentry)
		{
			p = pentry->next;
			if (pentry->key) free(pentry->key);
			free(pentry);
			pentry = p;
		}
	}
	free(hlist);
}

void show_hashtest_result()
{
	int i, backet = 0, max_link = 0, sum = 0;
	int conflict_count = 0, hit_count = 0;
	float avg_link, backet_usage;

	for(i = 0; i < backet_len; i++)
	{
		if (hlist[i].hit_count > 0) 
		{
			backet++;
			sum += hlist[i].entry_count;
			if (hlist[i].entry_count > max_link)
			{
				max_link = hlist[i].entry_count;
			}
			if (hlist[i].entry_count > 1)
			{
				conflict_count++;
			}
			hit_count += hlist[i].hit_count;
		}
	}

	backet_usage = backet/1.0/backet_len * 100;;
	avg_link = sum/1.0/backet;

	printf("backet_len = %d/n", backet_len);
	printf("hash_call_count = %d/n", hash_call_count);
	printf("hit_count = %d/n", hit_count);
	printf("conflict count = %d/n", conflict_count);
	printf("longest hash entry = %d/n", max_link);
	printf("average hash entry length = %.2f/n", avg_link);
	printf("backet usage = %.2f%/n", backet_usage);
}

void usage()
{
	printf("Usage: hashtest filename hash_func_name [backet_len]/n");
	printf("hash_func_name:/n");
	printf("/tsimple_hash/n");
	printf("/tRS_hash/n");
	printf("/tJS_hash/n");
	printf("/tPJW_hash/n");
	printf("/tELF_hash/n");
	printf("/tBKDR_hash/n");
	printf("/tSDBM_hash/n");
	printf("/tDJB_hash/n");
	printf("/tAP_hash/n");
	printf("/tCRC_hash/n");
}

void md5_to_32(unsigned char *md5_16, unsigned char *md5_32)
{
	int i;

	for (i = 0; i < 16; ++i)
	{
		sprintf(md5_32 + i * 2, "%02x", md5_16[i]);
	}
}

int main(int argc, char *argv[])
{
	int fd = -1, rwsize = 0;
	unsigned char md5_checksum[16 + 1] = {0};
	unsigned char buf[BLOCK_LEN] = {0};

	if (argc < 3) 
	{
		usage();
		return -1;
	}

	if (-1 == (fd = open(argv[1], O_RDONLY)))
	{
		perror("open source file");
		return errno;
	}

	if (argc == 4)
	{
		backet_len = atoi(argv[3]);
	}

	hashtest_init();
	choose_hash_func(argv[2]);
	while (rwsize = read(fd, buf, BLOCK_LEN))
	{
		md5(buf, rwsize, md5_checksum);
		insert_hash_entry(md5_checksum, hlist);
		hash_call_count++;
		memset(buf, 0, BLOCK_LEN);
		memset(md5_checksum, 0, 16 + 1);
	}
	close(fd);

	show_hashtest_result();
	hashtest_clean();
	return 0;
}
