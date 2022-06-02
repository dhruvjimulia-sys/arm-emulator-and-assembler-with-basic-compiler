#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

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

void free_hash_table(hash_table *symbol_table);

hash_table *create_hash_table(void);

unsigned int hash(char *s, unsigned int size_of_table);

entry *create_entry(char *s, uint32_t address);

bool resize(hash_table *symbol_table);

void insert(char *s, uint32_t address, entry **entries, unsigned int size_of_table, hash_table *symbol_table);

uint32_t lookup(char *s, hash_table *symbol_table);

void rehash(hash_table *symbol_table, entry **rehash_entries, unsigned int new_size);

#endif
