#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "symbol_table.h"

#define TABLE_SIZE 211

//destroy hash table and free up space
void free_hash_table(hash_table *symbol_table) {
	for (unsigned int i = 0; i < symbol_table->size; i++) {
		free((void*) symbol_table->entries[i]->symbol);
	}

	free(symbol_table->entries);
	free(symbol_table);
}

hash_table *create_hash_table(void) {
	//allocate memory for symbol table
	hash_table *symbol_table = malloc(sizeof(hash_table));
	symbol_table->count = 0;
	symbol_table->size = TABLE_SIZE;
    
	if (symbol_table == NULL) {
		//memory not allocated succesfully
		return NULL;
	}

	//allocate memory for entry buckets in hash table
	//initialise all pointers to NULL
	symbol_table->entries = calloc(TABLE_SIZE, sizeof(entry));
	if (symbol_table->entries == NULL) {
		free_hash_table(symbol_table);
		return NULL;
	}

	return symbol_table;
}

//hash function for symbols
unsigned int hash(char *s, unsigned int size_of_table) {
	unsigned int hash_val = 0;
	while (*s != '\0') {
		hash_val += *s;
	}
	//make sure hash_val is in range [0, hash table size - 1]
	return hash_val % size_of_table;
}

entry *create_entry(char *s, uint32_t address) {
	//creates a pointer to a new hash table entry
	entry *new_entry= malloc(sizeof(entry));
	new_entry->symbol = malloc(strlen(s) + 1);

	strcpy(new_entry->symbol, s);
	new_entry->address = address;
	new_entry->next = NULL;
	return new_entry;
}

bool resize(hash_table *symbol_table) {
	unsigned int new_size = symbol_table->size * 2;

	//check for overflow (new size is too big)
	if (new_size < symbol_table->size) {
		return false;
	}

	//allocate memory for new hash table
	entry **rehash_entries = calloc(new_size, sizeof(entry));
	if (rehash_entries == NULL) {
		return false;
	}

	//rehash symbols from the old symbol table
	rehash(symbol_table, rehash_entries, new_size);

	//free old entries and update symbol table
	free(symbol_table->entries);
	symbol_table->entries = rehash_entries;
	symbol_table->size = new_size;
	return true;
}

void insert(char *s, uint32_t address, entry **entries, unsigned int size_of_table, hash_table *symbol_table) {
	unsigned int slot = hash(s,size_of_table);

	//create entry for symbol being inserted
	entry *new_entry = create_entry(s, address);

	//try to look up entry with the same key (same symbol) in the hash table
	entry *curr_entry= entries[slot];
	if (curr_entry == NULL) {
		//hash table slot empty, no entry yet
		if (symbol_table->count == symbol_table->size) {
			//hash table is full
			//resize hash table to fit symbols
			resize(symbol_table);
		}
     
		//insert symbol directly into its slot
		entries[slot] = new_entry; 
		symbol_table->count++;

	} else {
		//chaining collision handling method (entry buckets)
		entry *prev;

		while (curr_entry != NULL) {
			//check if symbol matches key of current entry in the entry bucket
			if (strcmp(curr_entry->symbol, s) == 0) {
				//symbol matches key, replace address
				curr_entry->address = address;
				return;
			} else {
				//set curr_entry to next entry in the bucket
				prev = curr_entry;
				curr_entry = prev->next;
			}
		}

		//end of chain (entry bucket), add symbol to address mapping as a new entry to the bucket
		prev->next = new_entry;
	}
}

uint32_t lookup(char *s, hash_table *symbol_table) {
	unsigned int slot = hash(s, symbol_table->size);

	entry *lookup_entry = symbol_table->entries[slot];

	if (lookup_entry == NULL) {
		//slot is empty, symbol has no corresponding address in symbol table
		return -1;
	}

	while (lookup_entry != NULL) {
		if (strcmp(lookup_entry->symbol, s) == 0) {
			return lookup_entry->address;
		}

	*lookup_entry = *lookup_entry->next;
	}    

	//no match found
	return -1;
}

//rehash entries (used in resizing table)
void rehash(hash_table *symbol_table, entry **rehash_entries, unsigned int new_size){
	for (unsigned int i = 0; i < symbol_table->size; i++) {
		entry *curr_entry = symbol_table->entries[i];
		if (curr_entry->symbol != NULL) {
			insert(curr_entry->symbol, curr_entry->address, rehash_entries, new_size, symbol_table);
		}
	}
}
