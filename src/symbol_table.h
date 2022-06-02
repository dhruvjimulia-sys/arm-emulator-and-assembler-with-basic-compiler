#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

typedef struct {
    char *symbol;
    uint32_t address;
    struct entry *next_entry;
} entry;

typedef struct {
    entry **entries;  
    size_t count;   //number of entries already in the hash table
    size_t size;
} hash_table;

void free_hash_table(hash_table *symbol_table);

hash_table *create_hash_table(void);

unsigned int hash(char *s, hash_table *symbol_table);

void create_entry(char *s, uint32_t address);

bool resize(hash_table *symbol_table);

void insert(char *s, uint32_t address, entry *entries);

uint32_t lookup(char *s, hash_table *symbol_table);

void rehash(entry *rehash_entries);

#endif
