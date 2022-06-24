#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include "symbol_table.h"

#define TABLE_SIZE 41
#define PRIME 7

hash_table *create_hash_table(void) {
	//allocate memory for symbol table
	hash_table *new_table = malloc(sizeof(hash_table));
	if(new_table == NULL) {
		fprintf(stderr, "Memory allocation of hash table failed.");
		exit(EXIT_FAILURE);
	}

	//initialize properties of symbol table
	new_table->count = 0;
	new_table->size = TABLE_SIZE;
    
	//allocate memory for entry buckets in hash table
	new_table->entries = malloc(TABLE_SIZE * sizeof(entry *));
	
	if (new_table->entries == NULL) {
		free_hash_table(new_table);
		return NULL;
	}

	//initialize all pointers to entry buckets to NULL
	for (int i = 0; i < TABLE_SIZE; i++) {
		new_table->entries[i] = NULL;
	}

	return new_table;
}

unsigned int hash(const char *s, size_t table_size) {
	unsigned int hash_val = 0;
	unsigned int key_len = strlen(s); 
	for (int i = 0; i < key_len; i++) {	
		hash_val *= PRIME;
		hash_val += s[i];
	}
	//make sure hash_val is in range [0, hash table size - 1]
	return hash_val % table_size;
}

entry *create_entry(char *s, uint32_t address) {
	//creates a pointer to a new hash table entry
	entry *new_entry= malloc(sizeof(entry));
	assert(new_entry != NULL);

	new_entry->symbol = malloc(strlen(s) + 1);
	assert(new_entry->symbol != NULL);	

	strcpy(new_entry->symbol, s);
	new_entry->address = address;
	new_entry->next = NULL;
	return new_entry;
}


bool resize(hash_table *symtab) {
	size_t old_size = symtab->size;
	size_t new_size = symtab->size * 2;
	
	//check for overflow (if new size is too big)
	if (new_size < old_size) {
        	return false;
	}
	//set new size in hash table
	symtab->size = new_size;

	//allocate memory for new entry buckets
	entry **rehash_entries = malloc(new_size * sizeof(entry *));

	for (int i = 0; i < new_size; i++) {
                rehash_entries[i] = NULL;
        }

	if (rehash_entries == NULL) {
		fprintf(stderr, "Memory allocation for rehashed entries failed");
		return false;
	}

	//assign new entries to hash table
	entry **old_entries = symtab->entries;
	symtab->entries = rehash_entries;
	
	//rehash symbols from the old symbol table
	rehash(symtab, old_entries, old_size);

	//free old entries and update symbol table
	free(old_entries);
	
	return true;
}


void insert(char *s, uint32_t address, hash_table *symtab) {
	unsigned int slot = hash(s, symtab->size);
	entry **buckets = symtab->entries;

	//create entry for symbol being inserted
	entry *new_entry = create_entry(s, address);

	//try to look up entry with the same key (same symbol) in the hash table
	entry *curr_entry= buckets[slot];
	if (curr_entry == NULL) {
		//hash table slot empty, no entry yet
		if (symtab->count == symtab->size) {
			//hash table is full - resize hash table to fit symbols
			//resize(symtab);
			
			//reassign buckets variable to point to new rehashed table buckets of resized table
			buckets = symtab->entries;
		}
     
		//insert symbol directly into its slot
		buckets[slot] = new_entry; 
		symtab->count++;

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
				//set head to next entry in the bucket
				prev = curr_entry;
				curr_entry = prev->next;
			}
		}

		//end of chain (entry bucket), add symbol to address mapping as a new entry to the bucket
		prev->next = new_entry;
	}

}

uint32_t lookup(char *s, hash_table *symtab) {
	unsigned int slot = hash(s, symtab->size);

	entry *lookup_entry = symtab->entries[slot];

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

void free_hash_table(hash_table *symtab) {
        entry *prev;
        entry *curr;

        for (unsigned int i = 0; i < symtab->size; i++) {
                curr = symtab->entries[i];

                //deallocate memory of each entry in the bucket
                while (curr != NULL) {
                        prev = curr;
                        curr = curr->next;
                        free(prev->symbol);
                        free(prev);
                }

        }
        //deallocate rest of hash table
        free(symtab->entries);
        free(symtab);
}


void rehash(hash_table *symtab, entry **old_entries, size_t old_size){
	entry *prev;

	//rehash all currently stored keys based on new size of hash_table
	for (unsigned int i = 0; i < old_size; i++) {
		entry *curr = old_entries[i];
		
		//traverse each bucket to rehash every entry
		while (curr != NULL) {
			if (curr->symbol != NULL) {
				insert(curr->symbol, curr->address, symtab);
			}

			prev = curr;
			curr = curr->next;
			free(prev->symbol);
			free(prev);
		}
	}
}

/*
int main(void) {
	hash_table *symtab = create_hash_table();
	
	insert("label1", 0x0, symtab);
	insert("label2", 0x2, symtab);

	uint32_t address_one = lookup("label1", symtab);
	fprintf(stdout, "%x \n", address_one);
	uint32_t address_two = lookup("label2", symtab);
	fprintf(stdout, "%x \n", address_two);

	resize(symtab);
	
	uint32_t address_one_r = lookup("label1", symtab);
        fprintf(stdout, "%x \n", address_one_r);
        uint32_t address_two_r = lookup("label2", symtab);
        fprintf(stdout, "%x \n", address_two_r);

	free_hash_table(symtab);
	fprintf(stdout, "success \n");
}
*/
