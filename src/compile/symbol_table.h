#ifndef __SYMBOL_TABLE_H__
#define __SYMBOL_TABLE_H__

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

typedef struct hash_table_entry {
	char *symbol;
	uint32_t address;
	struct hash_table_entry *next;
} entry;

typedef struct hash_table_struct {
	unsigned int count;   //number of entries already in the hash table
	size_t size;
	struct hash_table_entry **entries;
} hash_table;

//destroy hash table and free up space allocated to it
void free_hash_table(hash_table *symtab);

//create a new hash table
hash_table *create_hash_table(void);

//hash function for hash table entries
unsigned int hash(const char *s, size_t table_size);

//create a new hash table entry
//entry contains pointer to key, associated address, pointer to the next key stored in the entry bucket
entry *create_entry(char *s, uint32_t address);

//resize hash table (currently sets new size to be 2 * previous size)
//bool resize(hash_table *symbol_table);

//insert into hash table
//all the arguments are necessary as resize function uses insert with old table and new size
void insert(char *s, uint32_t address, hash_table *symtab);

//look up a key in the hash table
uint32_t lookup(char *s, hash_table *symtab);

//rehash function used when resizing the hash table
void rehash(hash_table *symtab, entry **old_entries, size_t old_size);

#endif
