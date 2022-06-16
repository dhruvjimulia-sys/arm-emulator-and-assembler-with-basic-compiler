#ifndef __SYMBOL_TABLE_H__
#define __SYMBOL_TABLE_H__

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

typedef struct hash_table_entry {
	char *symbol;
	uint32_t address;
	struct hash_table_entry *next;
} entry;

typedef struct {
	entry **entries;  
	unsigned int count;   //number of entries already in the hash table
	unsigned int size;
} hash_table;

//destroy hash table and free up space allocated to it
void free_hash_table(hash_table *symbol_table);

//create a new hash table
hash_table *create_hash_table(void);

//hash function for hash table entries
unsigned int hash(char *s, unsigned int size_of_table);

//create a new hash table entry
//entry contains pointer to key, associated address, pointer to the next key stored in the entry bucket
entry *create_entry(char *s, uint32_t address);

//resize hash table (currently sets new size to be 2 * previous size)
bool resize(hash_table *symbol_table);

//insert into hash table
//all the arguments are necessary as resize function uses insert with old table and new size
void insert(char *s, uint32_t address, entry **entries, unsigned int size_of_table, hash_table *symbol_table);

//look up a key in the hash table
uint32_t lookup(char *s, hash_table *symbol_table);

//rehash function used when resizing the hash table
void rehash(hash_table *symbol_table, entry **rehash_entries, unsigned int new_size);

#endif
