#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** @brief Nodes for Huffman tree and priority queue. */
typedef struct Node Node;

/** @brief Priority queue used for Huffman tree construction. */
typedef struct PriorityQueue PriorityQueue;

/** @brief Huffman tree structure. */
typedef struct HuffmanTree HuffmanTree;

/** @brief Entry of the hash table. */
typedef struct HashTableEntry HashTableEntry;

/** @brief Hash table used for frequency counting. */
typedef struct HashTable HashTable;

/**
 * @brief Creates a new node.
 * @param symbol Symbol associated with the node.
 * @param frequency Frequency of the symbol.
 * @param symbol_length Length of the symbol in bytes.
 * @return Pointer to the created node.
 */
Node *create_node(uint8_t *symbol, uint32_t frequency, uint8_t symbol_length);

/**
 * @brief Frees node data excluding symbol data.
 * @param node Node to free.
 */
void free_node(Node *node);

/**
 * @brief Checks whether a node is a leaf.
 * @param node Node to check.
 * @return Non-zero if the node is a leaf, zero otherwise.
 */
uint8_t node_is_leaf(Node *node);

/**
 * @brief Creates a priority queue.
 * @param capacity Initial queue capacity.
 * @return Pointer to the created priority queue.
 */
PriorityQueue *pq_create(uint32_t capacity);

/**
 * @brief Inserts a node into the priority queue.
 * @param priority_queue Target priority queue.
 * @param node Node to insert.
 */
void pq_push(PriorityQueue *priority_queue, Node *node);

/**
 * @brief Removes and returns the last node.
 * @param priority_queue Source priority queue.
 * @return Pointer to the removed node.
 */
Node *pq_pop(PriorityQueue *priority_queue);

/**
 * @brief Builds a Huffman tree from the priority queue.
 * @param priority_queue Priority queue containing leaf nodes.
 * @return Root node of the Huffman tree.
 */
Node *pq_merge(PriorityQueue *priority_queue);

/**
 * @brief Frees priority queue data excluding stored nodes.
 * @param priority_queue Priority queue to free.
 */
void pq_free(PriorityQueue *priority_queue);

/**
 * @brief Creates a Huffman tree.
 * @param node Root node of the tree.
 * @param symbol_length Length of symbols in bytes.
 * @return Pointer to the created Huffman tree.
 */
HuffmanTree *create_tree(Node *node, uint8_t symbol_length);

/**
 * @brief Frees Huffman tree data including symbol data.
 * @param tree Huffman tree to free.
 */
void free_tree(HuffmanTree *tree);

/**
 * @brief Creates a hash table.
 * @param capacity Hash table capacity.
 * @return Pointer to the created hash table.
 */
HashTable *create_hash_table(uint32_t capacity);

/**
 * @brief Frees hash table data.
 * @param table Hash table to free.
 */
void free_hash_table(HashTable *table);

/**
 * @brief Frees hash table entry data.
 * @param entry Hash table entry to free.
 */
void free_hash_entry(HashTableEntry *entry);

/**
 * @brief Computes a hash value for a symbol.
 * @param symbol Symbol data.
 * @param symbol_length Length of the symbol in bytes.
 * @param table_size Hash table size.
 * @return Hash value.
 */
uint32_t hash_function(uint8_t *symbol, uint8_t symbol_length,
                       uint32_t table_size);

/**
 * @brief Adds a symbol to the hash table or increments its frequency.
 * @param table Hash table.
 * @param symbol Symbol data.
 * @param symbol_length Length of the symbol in bytes.
 * @param code Symbol code.
 * @param code_length Code length in bits.
 */
void add_symbol_hash(HashTable *table, uint8_t *symbol, uint8_t symbol_length,
                     uint32_t code, uint8_t code_length);

/**
 * @brief Compares two symbols lexicographically.
 * @param symbol1 First symbol.
 * @param symbol2 Second symbol.
 * @param symbol_length Length of symbols in bytes.
 * @return Negative, zero, or positive value as in memcmp().
 */
int compare_symbols(uint8_t *symbol1, uint8_t *symbol2, uint8_t symbol_length);

/**
 * @brief Computes the left height of a subtree.
 * @param node Root node of the subtree.
 * @return Left height of the subtree.
 */
int height_left(Node *node);

#endif
