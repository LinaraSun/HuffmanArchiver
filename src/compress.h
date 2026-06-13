#ifndef COMPRESS_H
#define COMPRESS_H
#include "huffman.h"

int compress_file(FILE *input, FILE *output, uint8_t symbol_length,
                  uint64_t original_file_size); // Function for compressing the
                                                // input file to output file
HuffmanTree *count_freq_1b(FILE *file); // Counting symbols' frequencies from
                                        // input file if symbol size equals 1
HuffmanTree *count_freq_hash(
    FILE *file,
    uint8_t symbol_length); // Counting symbols' frequencies from input file if
                            // symbol size is higher than 1
void counting_code_lengths(
    HuffmanTree *huffman_tree,
    Node *root); // Counting code lengths using the Huffman tree, calls a
                 // recursive function
void counting_len_recursion(
    HuffmanTree *huffman_tree, Node *node,
    uint8_t code_length); // Recursive function for counting code lengths, going
                          // through the nodes of the Huffman tree
void encoding_from_len(
    HuffmanTree *huffman_tree); // Calculating canonical codes
int write_header_to_file(
    FILE *output, HuffmanTree *huffman_tree,
    uint64_t original_file_size); // Writing header to output file
int write_encoded_file_1b(
    FILE *input, FILE *output,
    HuffmanTree *huffman_tree); // Writing compressed data to output file if
                                // symbol size equals 1
int write_encoded_file_hash(
    FILE *input, FILE *output,
    HuffmanTree *huffman_tree); // Writing compressed data to output file if
                                // symbol size is higher than 1
#endif
