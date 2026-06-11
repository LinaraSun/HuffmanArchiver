#ifndef COMPRESS_H
#define COMPRESS_H
#include "huffman.h"

int compress_file(FILE *input, FILE *output, uint8_t symbol_length,
                  uint64_t original_file_size);
HuffmanTree *count_freq_1b(FILE *file);
HuffmanTree *count_freq_hash(FILE *file, uint8_t symbol_length);
void counting_code_lengths(HuffmanTree *huffman_tree, Node *root);
void counting_len_recursion(HuffmanTree *huffman_tree, Node *node,
                            uint8_t code_length);
void encoding_from_len(HuffmanTree *huffman_tree);
int write_header_to_file(FILE *output, HuffmanTree *huffman_tree,
                         uint64_t original_file_size);
int write_encoded_file_1b(FILE *input, FILE *output, HuffmanTree *huffman_tree);
int write_encoded_file_hash(FILE *input, FILE *output,
                            HuffmanTree *huffman_tree);
#endif
