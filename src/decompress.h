#ifndef DECOMPRESS_H
#define DECOMPRESS_H
#include "huffman.h"

typedef struct {
  uint8_t *symbol;
  uint8_t code_length;
} HashDecodeEntry; // Entry of a hash table for fast lookup of symbols using
                   // codes

int decompress_file(
    FILE *input,
    FILE *output); // Function for decompressing the input file to output file
HuffmanTree *read_header(
    FILE *file,
    uint64_t *original_file_size_ptr); // Reading header and taking the data
                                       // necessary for decompressing the file
void recovering_codes(HuffmanTree *huffman_tree); // Recovering codes using the
                                                  // data taken from header
int writing_decoded_file(
    FILE *input, FILE *output, HuffmanTree *huffman_tree,
    uint64_t
        original_file_size); // Decoding data using hash table for fast lookup
#endif
