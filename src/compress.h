#ifndef COMPRESS_H
#define COMPRESS_H
#include "huffman.h"

/**
 * @brief Computes Huffman code lengths by traversing the tree.
 * @param huffman_tree Huffman tree structure.
 * @param root Root node of the Huffman tree.
 */
void counting_code_lengths(HuffmanTree *huffman_tree, Node *root);

/**
 * @brief Generates canonical Huffman codes from code lengths.
 * @param huffman_tree Huffman tree structure.
 */
void encoding_from_len(HuffmanTree *huffman_tree);

/**
 * @brief Compresses data from an input file to an output file.
 * @param input Input file stream.
 * @param output Output file stream.
 * @param symbol_length Length of symbols in bytes.
 * @param original_file_size Original input file size in bytes.
 * @return 0 on success, non-zero on error.
 */
int compress_file(FILE *input, FILE *output, uint8_t symbol_length,
                  uint64_t original_file_size);

#endif
