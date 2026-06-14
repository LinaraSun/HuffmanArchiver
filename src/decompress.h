#ifndef DECOMPRESS_H
#define DECOMPRESS_H
#include "huffman.h"

/** @brief Entry of the decode hash table for fast code lookup. */
typedef struct HashDecodeEntry HashDecodeEntry;

/**
 * @brief Decompresses data from an input file to an output file.
 * @param input Input file stream.
 * @param output Output file stream.
 * @return 0 on success, non-zero on error.
 */
int decompress_file(FILE *input, FILE *output);

#endif
