#ifndef COMPRESS_H
#define COMPRESS_H
#include "huffman.h"

// Counting code lengths using the Huffman tree
void counting_code_lengths(HuffmanTree *huffman_tree, Node *root);

// Encoding symbols from code lengths
void encoding_from_len(HuffmanTree *huffman_tree);

// Function for compressing the input file to output file
int compress_file(FILE *input, FILE *output, uint8_t symbol_length,
                  uint64_t original_file_size);
#endif
