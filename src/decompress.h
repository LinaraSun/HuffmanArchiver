#ifndef DECOMPRESS_H
#define DECOMPRESS_H
#include "huffman.h"

typedef struct {
  uint8_t *symbol;
  uint8_t code_length;
} HashDecodeEntry;

int decompress_file(FILE *input, FILE *output);
HuffmanTree *read_header(FILE *file, uint64_t *original_file_size_ptr);
void recovering_codes(HuffmanTree *huffman_tree);
int writing_decoded_file(FILE *input, FILE *output, HuffmanTree *huffman_tree,
                         uint64_t original_file_size);

#endif
