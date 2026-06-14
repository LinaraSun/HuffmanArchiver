#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Nodes for Huffman table and priority queue
typedef struct Node Node;

// Priority queue for building the Huffman tree
typedef struct PriorityQueue PriorityQueue;

// Huffman tree for counting code lengths
typedef struct HuffmanTree HuffmanTree;

// Entry of hash table
typedef struct HashTableEntry HashTableEntry;

// Hash table for counting frequencies
typedef struct HashTable HashTable;

// Creating a node
Node *create_node(uint8_t *symbol, uint32_t frequency, uint8_t symbol_length);
// Freeing the data of a node (excluding symbol data)
void free_node(Node *node);
// Checking if the given node is a leaf
uint8_t node_is_leaf(Node *node);

// Creating a priority queue
PriorityQueue *pq_create(uint32_t capacity);
// Pushing a node to priority queue
void pq_push(PriorityQueue *priority_queue, Node *node);
// Taking the last node of a priority queue, deleting it from the priority queue
// and returning it
Node *pq_pop(PriorityQueue *priority_queue);
// Building the Huffman tree and returning the root
Node *pq_merge(PriorityQueue *priority_queue);
// Freeing priority queue data (excluding nodes data)
void pq_free(PriorityQueue *priority_queue);

// Creating a Huffman tree
HuffmanTree *create_tree(Node *node, uint8_t symbol_length);
// Freeing the Huffman tree data (including symbol data)
void free_tree(HuffmanTree *tree);

// Creating hash table
HashTable *create_hash_table(uint32_t capacity);
// Freeing hash table data
void free_hash_table(HashTable *table);
// Freeing hash table entry data
void free_hash_entry(HashTableEntry *entry);
// Hash function for counting frequencies using hash table
uint32_t hash_function(uint8_t *symbol, uint8_t symbol_length,
                       uint32_t table_size);
// Adding symbol to hash table or incrementing frequency
void add_symbol_hash(HashTable *table, uint8_t *symbol, uint8_t symbol_length,
                     uint32_t code, uint8_t code_length);

// Comparing symbols
int compare_symbols(uint8_t *symbol1, uint8_t *symbol2, uint8_t symbol_length);
// Counting height for deterministic Huffman tree building
int height_left(Node *node);

#endif
