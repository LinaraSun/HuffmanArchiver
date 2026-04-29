#include "huffman.h"

HuffmanTree* read_header(FILE* file, uint64_t* original_file_size_ptr) {

	uint8_t* buffer_4b = (uint8_t*)malloc(sizeof(uint8_t) * 4);
	if (!buffer_4b) {
		fprintf(stderr, "Failed to allocate memory while reading header.\n");
		return NULL;
	}

	if (fread(buffer_4b, 1, 4, file) != 4) {
		fprintf(stderr, "Invalid file format.\n");
		free(buffer_4b);
		return NULL;
	}

	if (memcmp("HUFF", buffer_4b, 4) != 0) {
		fprintf(stderr, "Invalid file format.\n");
		free(buffer_4b);
		return NULL;
	}

	uint8_t* buffer_1b = (uint8_t*)malloc(sizeof(uint8_t));
	if (!buffer_1b) {
		fprintf(stderr, "Failed to allocate memory while reading header.\n");
		free(buffer_4b);
		return NULL;
	}

	if (fread(buffer_1b, 1, 1, file) != 1) {
		fprintf(stderr, "Invalid file format.\n");
		free(buffer_4b);
		free(buffer_1b);
		return NULL;
	}

	uint8_t one = 1;
	if (memcmp(&one, buffer_1b, 1) != 0) {
		fprintf(stderr, "Invalid version.\n");
		free(buffer_4b);
		free(buffer_1b);
		return NULL;
	}

	if (fread(buffer_1b, 1, 1, file) != 1) {
		fprintf(stderr, "Invalid file format.\n");
		free(buffer_4b);
		free(buffer_1b);
		return NULL;
	}

	uint8_t symbol_size = *buffer_1b;
	if (symbol_size < 1 || symbol_size > 4) {
		fprintf(stderr, "Invalid symbol size.\n");
		free(buffer_4b);
		free(buffer_1b);
		return NULL;
	}

	if (fread(original_file_size_ptr, 8, 1, file) != 1) {
		fprintf(stderr, "Invalid file format.\n");
		free(buffer_4b);
		free(buffer_1b);
		return NULL;
	}

	uint64_t original_file_size = *original_file_size_ptr;
	if (original_file_size == 0) {
		if (fread(buffer_4b, 1, 4, file) != 4) {
			fprintf(stderr, "Invalid file format.\n");
			return NULL;
		}

		uint32_t symbols_count = *buffer_4b;

		if (symbols_count != 0) {
			fprintf(stderr, "Invalid header.\n");
			return NULL;
		}

		free(buffer_4b);
		free(buffer_1b);
		return create_tree(NULL, 1);
	}

	if (fread(buffer_4b, 1, 4, file) != 4) {
		fprintf(stderr, "Invalid file format.\n");
		return NULL;
	}

	uint32_t symbols_count = *buffer_4b;

	if (symbols_count == 0) {
		fprintf(stderr, "Invalid header.\n");
		return NULL;
	}

	free(buffer_1b);
	free(buffer_4b);

	uint8_t* buffer_sym = (uint8_t*)malloc(sizeof(uint8_t) * symbol_size);
	if (!buffer_sym) {
		fprintf(stderr, "Failed to allocate memory while reading header.\n");
		return NULL;
	}

	uint8_t* buffer_code_len = (uint8_t*)malloc(sizeof(uint8_t));
	if (!buffer_code_len) {
		fprintf(stderr, "Failed to allocate memory while reading header.\n");
		free(buffer_sym);
		return NULL;
	}

	HuffmanTree* ht = create_tree(NULL, symbol_size);

	ht->codes = (uint32_t*)malloc(sizeof(uint32_t) * symbols_count);
	ht->symbols = (uint8_t**)malloc(sizeof(uint8_t*) * symbols_count);
	ht->code_lengths = (uint8_t*)malloc(sizeof(uint8_t) * symbols_count);

	for (uint32_t i = 0; i < symbols_count; i++) {
		if (fread(buffer_sym, symbol_size, 1, file) != 1) {
			fprintf(stderr, "Failed to read symbol in header.\n");
			return NULL;
		}

		if (fread(buffer_code_len, 1, 1, file) != 1) {
			fprintf(stderr, "Failed to read code length.\n");
			return NULL;
		}

		memcpy(ht->symbols[ht->symbols_count], buffer_sym, symbol_size);
		ht->code_lengths[ht->symbols_count] = *buffer_code_len;
		ht->symbols_count++;
	}

	recovering_codes(ht);

	free(buffer_4b);
	free(buffer_1b);
	return ht;
}

void recovering_codes(HuffmanTree* ht) {
	// How should I do this...
	// Sort by length, then by order in the header, probably just by switching if strictly <
	// Then assign codes
	if (!ht || !ht->symbols || !ht->code_lengths) {
		fprintf(stderr, "Invalid Huffman tree given to recover codes\n");
		return;
	}

	uint32_t temp_len = 0;
	uint8_t* temp_sym = NULL;

	for (uint32_t i = 1; i < ht->symbols_count; i++) {
		uint32_t j = i;
		while (ht->code_lengths[j - 1] > ht->code_lengths[j]) {
			temp_len = ht->code_lengths[j];
			ht->code_lengths[j] = ht->code_lengths[j - 1];
			ht->code_lengths[j - 1] = temp_len;

			temp_sym = ht->symbols[j];
			ht->symbols[j] = ht->symbols[j - 1];
			ht->symbols[j - 1] = temp_sym;

			j--;
		}
	}
}

int writing_decoded_file(FILE* out, HuffmanTree* ht, uint64_t original_file_size) {}

int decompress_file(FILE* input, FILE* output, uint8_t symbol_len) {

	HuffmanTree* ht = NULL;

	uint64_t* original_file_size_ptr = (uint64_t*)malloc(sizeof(uint64_t));
	if (!original_file_size_ptr) {
		fprintf(stderr, "Failed to allocate memory.\n");
		return 1;
	}

	ht = read_header(input, original_file_size_ptr);

	if (!ht) {
		free(original_file_size_ptr);
		return 1;
	}

	if (!ht->symbols) {
		free(original_file_size_ptr);
		free_tree(ht);
		return 0;
	}

	free(original_file_size_ptr);
	free_tree(ht);
	return 0;
}
