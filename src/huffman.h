#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
  uint8_t *symbol_data;
  uint32_t frequency;
  uint8_t symbol_length;
  struct Node *right;
  struct Node *left;
} Node;

typedef struct {
  Node **nodes;
  uint32_t size;
  uint32_t capacity;
} PriorityQueue;

typedef struct {
  Node *root;
  uint32_t *codes;
  uint8_t **symbols;
  uint8_t *code_lengths;
  uint32_t symbols_count;
  uint8_t symbol_length;
} HuffmanTree;

typedef struct HashTableEntry {
  uint8_t *symbol_data;
  uint32_t frequency;
  uint8_t symbol_length;
  uint32_t code;
  uint8_t code_length;
  struct HashTableEntry *next;
} HashTableEntry;

typedef struct {
  HashTableEntry **buckets;
  uint32_t size;
  uint32_t capacity;
} HashTable;

Node *create_node(uint8_t *symbol, uint32_t frequency, uint8_t symbol_length);
void free_node(Node *node);
uint8_t node_is_leaf(Node *node);

PriorityQueue *pq_create(uint32_t capacity);
void pq_push(PriorityQueue *priority_queue, Node *node);
Node *pq_pop(PriorityQueue *priority_queue);
Node *pq_merge(PriorityQueue *priority_queue);
void pq_free(PriorityQueue *priority_queue);

HuffmanTree *create_tree(Node *node, uint8_t symbol_length);
void free_tree(HuffmanTree *tree);

HashTable *create_hash_table(uint32_t capacity);
void free_hash_table(HashTable *table);
void free_hash_entry(HashTableEntry *entry);
uint32_t hash_function(uint8_t *symbol, uint8_t symbol_length,
                       uint32_t table_size);
void add_symbol_hash(HashTable *table, uint8_t *symbol, uint8_t symbol_length,
                     uint32_t code, uint8_t code_length);

int compare_symbols(uint8_t *symbol1, uint8_t *symbol2, uint8_t symbol_length);
int height_left(Node *node);

#endif
