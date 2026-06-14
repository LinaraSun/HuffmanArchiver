#ifndef DECOMPRESS_H
#define DECOMPRESS_H
#include "huffman.h"

// Entry of a hash table for fast lookup of symbols using codes
typedef struct HashDecodeEntry HashDecodeEntry;

// Function for decompressing the input file to output file
int decompress_file(FILE *input, FILE *output);

#endif
