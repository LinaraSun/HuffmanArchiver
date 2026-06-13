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
} Node; // Nodes for Huffman table and priority queue

typedef struct {
  Node **nodes;
  uint32_t size;
  uint32_t capacity;
} PriorityQueue; // Priority queue for building the Huffman tree

typedef struct {
  Node *root;
  uint32_t *codes;
  uint8_t **symbols;
  uint8_t *code_lengths;
  uint32_t symbols_count;
  uint8_t symbol_length;
} HuffmanTree; // Huffman tree for counting code lengths

typedef struct HashTableEntry {
  uint8_t *symbol_data;
  uint32_t frequency;
  uint8_t symbol_length;
  uint32_t code;
  uint8_t code_length;
  struct HashTableEntry *next;
} HashTableEntry; // Entry of hash table

typedef struct {
  HashTableEntry **buckets;
  uint32_t size;
  uint32_t capacity;
} HashTable; // Hash table for counting frequencies

Node *create_node(uint8_t *symbol, uint32_t frequency,
                  uint8_t symbol_length); // Creating a node
void free_node(
    Node *node); // Freeing the data of a node (excluding symbol data)
uint8_t node_is_leaf(Node *node); // Checking if the given node is a leaf

PriorityQueue *pq_create(uint32_t capacity); // Creating a priority queue
void pq_push(PriorityQueue *priority_queue,
             Node *node); // Pushing a node to priority queue
Node *pq_pop(
    PriorityQueue
        *priority_queue); // Taking the last node of a priority queue, deleting
                          // it from the priority queue and returning it
Node *pq_merge(PriorityQueue *priority_queue); // Building the Huffman tree
void pq_free(PriorityQueue *priority_queue);   // Freeing priority queue data
                                               // (excluding nodes data)

HuffmanTree *create_tree(Node *node,
                         uint8_t symbol_length); // Creating a Huffman tree
void free_tree(
    HuffmanTree *tree); // Freeing the Huffman tree data (including symbol data)

HashTable *create_hash_table(uint32_t capacity); // Creating hash table
void free_hash_table(HashTable *table);          // Freeing hash table data
void free_hash_entry(HashTableEntry *entry); // Freeing hash table entry data
uint32_t hash_function(uint8_t *symbol, uint8_t symbol_length,
                       uint32_t table_size); // Hash function for counting
                                             // frequencies using hash table
void add_symbol_hash(HashTable *table, uint8_t *symbol, uint8_t symbol_length,
                     uint32_t code,
                     uint8_t code_length); // Adding symbol to hash table or
                                           // incrementing frequency

int compare_symbols(uint8_t *symbol1, uint8_t *symbol2,
                    uint8_t symbol_length); // Comparing symbols
int height_left(
    Node *node); // Counting height for deterministic Huffman tree building
#endif
