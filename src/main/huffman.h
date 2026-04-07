#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define MAX_SYMBOL_BYTES 4

typedef struct Node {
	uint8_t* symbol_data;
	uint32_t frequency;
	uint8_t symbol_length;
	struct Node* right;
	struct Node* left;
} Node;

typedef struct {
	Node** nodes;
	uint32_t size;
	uint32_t capacity;
} PriorityQueue;

typedef struct {
	Node* root;
	uint32_t* codes;
	uint8_t** symbols;
	uint8_t* code_lengths;
	uint32_t symbols_count;
	uint8_t symbol_length;
} HuffmanTree;

typedef struct HashTableEntry {
	uint8_t* symbol_data;
	uint32_t frequency;
	uint8_t symbol_len;
	uint32_t code;
	uint8_t code_len;
	struct HashTableEntry* next;
} HashTableEntry;

typedef struct {
	HashTableEntry** buckets;
	uint32_t size;
	uint32_t capacity;
} HashTable;

Node* create_node(uint8_t* symbol, uint32_t freq, uint8_t symbol_len);
void free_node(Node* node);
uint8_t node_is_leaf(Node* node);

PriorityQueue* pq_create(uint32_t capacity);
void pq_push(PriorityQueue* pq, Node* node);
Node* pq_pop(PriorityQueue* pq);
Node* pq_merge(PriorityQueue* pq);
void pq_free(PriorityQueue* pq);

HuffmanTree* create_tree(Node* node, uint8_t symbol_len);
void free_tree(HuffmanTree* tree);

HashTable* create_hash_table(uint32_t capacity);
void free_hash_table(HashTable* table);
// TODO THIS!!
void free_hash_entry(HashTableEntry* entry);
uint32_t hash_function(uint8_t* symbol, uint8_t symbol_len, uint32_t table_size);
void add_symbol_hash(HashTable* table, uint8_t* symbol, uint8_t symbol_len, uint32_t code, uint8_t code_len);
int compare_symbols(uint8_t* symbol1, uint8_t* symbol2, uint8_t symbol_len);

int compress_file(FILE* input, FILE* output, uint8_t symbol_len, uint64_t original_file_size);
HuffmanTree* count_freq_1b(FILE* file);
HuffmanTree* count_freq_hash(FILE* file, uint8_t symbol_len);
void encoding(HuffmanTree* ht);
void encoding_recursion(HuffmanTree* ht, Node* node, uint32_t code, uint8_t code_len);
int write_header_to_file(FILE* out, HuffmanTree* ht, uint64_t original_file_size);
int write_encoded_file_1b(FILE* input, FILE* output, HuffmanTree* ht);
int write_encoded_file_hash(FILE* input, FILE* output, HuffmanTree* ht);

int decompress_file(FILE* input, FILE* output, uint8_t symbol_len);
#endif
