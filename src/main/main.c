#include "huffman.h"

void fprint_usage(char* program) {
	fprintf(stderr, "Usage: %s <compress|decompress> <input> <output> [symbol_size]\n", program);
}

int main(int argc, char** argv) {
	if (argc < 4 || argc > 5) {
		fprint_usage(argv[0]);
		return 1;
	}

	if (strcmp(argv[1], "compress") && strcmp(argv[1], "decompress")) {
		fprint_usage(argv[0]);
		return 1;
	}

	FILE* input = fopen(argv[2], "rb");
	if (input == NULL) {
		fprint_usage(argv[0]);
		printf("Error opening input file.\n");
		return 1;
	}

	FILE* output = fopen(argv[3], "wb");
	if (output == NULL) {
		fprint_usage(argv[0]);
		printf("Error opening output file\n");
		fclose(input);
		return 1;
	}

	uint8_t symbol_len = 1;
	if (argc == 5) {
		if (!atoi(argv[4])) {
			fprint_usage(argv[0]);
			fprintf(stderr, "Symbol length should be an integer.\n");
			fclose(input);
			fclose(output);
			return 1;
		} else {
			uint32_t symbol_len_32 = atoi(argv[4]);
			if (symbol_len_32 < 1 || symbol_len_32 > MAX_SYMBOL_BYTES) {
				fprint_usage(argv[0]);
				fprintf(stderr, "Symbol length should be an integer higher or equal to one and lower or equal to four.\n");
				fclose(input);
				fclose(output);
				return 1;
			}
			symbol_len = (uint8_t)symbol_len_32;
		}
	}

	if (!strcmp(argv[1], "compress")) {
		fseek(input, 0, SEEK_END);
		int64_t orig_file_size = ftell(input);
		if (orig_file_size == -1) {
			fprintf(stderr, "Error using ftell.\n");
			return 1;
		}
		uint64_t original_file_size = (uint64_t)orig_file_size;
		rewind(input);
		int compression_res = compress_file(input, output, symbol_len, original_file_size);
		if (compression_res == 1) {
			printf("Compression failed.\n");
			fclose(input);
			fclose(output);
			return 1;
		} else {
			printf("Compression was successful.\n");
			fclose(input);
			fclose(output);
			return 0;
		}
	} else {
		int decompression_res = decompress_file(input, output, symbol_len);
		if (decompression_res == 1) {
			printf("Decompression failed.\n");
			fclose(input);
			fclose(output);
			return 1;
		} else {
			printf("Decompression was successful.\n");
			fclose(input);
			fclose(output);
			return 0;
		}
	}
}
