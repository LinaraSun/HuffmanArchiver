#include "huffman.h"

#define TABLE_BITS 12
#define TABLE_SIZE (1 << TABLE_BITS)

HuffmanTree* read_header(FILE* file, uint64_t* original_file_size_ptr) {

	uint8_t* buffer_4b = (uint8_t*)malloc(sizeof(uint8_t) * 4);
	if (!buffer_4b) {
		fprintf(stderr, "Failed to allocate memory while reading header.\n");
		return NULL;
	}

	if (fread(buffer_4b, 1, 4, file) != 4) {
		fprintf(stderr, "Invalid file format - no bytes to read magic word from.\n");
		free(buffer_4b);
		return NULL;
	}

	if (memcmp("HUFF", buffer_4b, 4) != 0) {
		fprintf(stderr, "Invalid file format - incorrect magic word.\n");
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
		fprintf(stderr, "Invalid file format - no bytes past magic word.\n");
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
		free(buffer_4b);
		free(buffer_1b);
		return create_tree(NULL, 1);
	}

	if (fread(buffer_4b, 1, 4, file) != 4) {
		fprintf(stderr, "Invalid file format.\n");
		free(buffer_4b);
		free(buffer_1b);
		return NULL;
	}

	// uint32_t symbols_count = *buffer_4b;
	uint32_t symbols_count = 0;
	memcpy(&symbols_count, buffer_4b, sizeof(uint32_t));

	if (symbols_count == 0) {
		fprintf(stderr, "Invalid header - original file size greater than zero but zero symbols.\n");
		free(buffer_4b);
		free(buffer_1b);
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

	ht->codes = (uint32_t*)calloc(symbols_count, sizeof(uint32_t));
	if (!ht->codes) {}

	ht->symbols = (uint8_t**)calloc(symbols_count, sizeof(uint8_t*));
	if (!ht->symbols) {}

	for (int i = 0; i < symbols_count; i++) {
		ht->symbols[i] = (uint8_t*)calloc(symbol_size, sizeof(uint8_t));
		if (!ht->symbols[i]) {
			// Error
			return NULL;
		}
	}

	ht->code_lengths = (uint8_t*)calloc(symbols_count, sizeof(uint8_t));
	if (!ht->code_lengths) {}

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

	if (buffer_sym != NULL) free(buffer_sym);
	if (buffer_code_len != NULL) free(buffer_code_len);
	recovering_codes(ht);
	return ht;
}

void recovering_codes(HuffmanTree* ht) {

	if (!ht || !ht->symbols || !ht->code_lengths) {
		fprintf(stderr, "Invalid Huffman tree given to recover codes\n");
		return;
	}

	uint32_t temp_len = 0;
	uint8_t* temp_sym = NULL;

	for (uint32_t i = 1; i < ht->symbols_count; i++) {
		uint32_t j = i;
		while (j >= 1 && ((ht->code_lengths[j - 1] > ht->code_lengths[j]) || 
		(ht->code_lengths[j - 1] == ht->code_lengths[j] && compare_symbols(ht->symbols[j - 1], ht->symbols[j], ht->symbol_length) > 0))) {
			temp_len = ht->code_lengths[j];
			ht->code_lengths[j] = ht->code_lengths[j - 1];
			ht->code_lengths[j - 1] = temp_len;

			temp_sym = ht->symbols[j];
			ht->symbols[j] = ht->symbols[j - 1];
			ht->symbols[j - 1] = temp_sym;

			j--;
		}
	}

	uint32_t code = 0;
	uint8_t current_len = 1;

	for (uint32_t i = 0; i < ht->symbols_count; i++) {
		if (current_len < ht->code_lengths[i]) {
			code = code << (ht->code_lengths[i] - current_len);
			current_len = ht->code_lengths[i];
		}

		ht->codes[i] = code;

		code++;
	}
}

int writing_decoded_file(FILE* in, FILE* out, HuffmanTree* ht, uint64_t original_file_size) {

	HashDecodeEntry* table = (HashDecodeEntry*)calloc(TABLE_SIZE, sizeof(HashDecodeEntry));
	for (uint32_t i = 0; i < ht->symbols_count; i++) {
		uint32_t base_index = ht->codes[i] << (TABLE_BITS - ht->code_lengths[i]);
		for (uint32_t j = base_index; j <= base_index + ((1 << (TABLE_BITS - ht->code_lengths[i])) - 1); j++) {
			table[j].symbol = ht->symbols[i];
			table[j].code_len = ht->code_lengths[i];
		}
	}

	uint32_t bit_buffer = 0;
	int8_t bits_in_buffer = 0;

	uint8_t byte_read = 0;
	uint64_t bytes_written = 0;

	uint32_t index = 0;

	while (bytes_written < original_file_size) {
		while (bits_in_buffer < 12 && (fread(&byte_read, 1, 1, in) == 1)) {
			bit_buffer = (bit_buffer << 8) | (uint32_t)byte_read;
			bits_in_buffer += 8;
		}

		if (bits_in_buffer > 12) {
			index = (bit_buffer >> (bits_in_buffer - TABLE_BITS));
		} else if (bits_in_buffer == 12) {
			index = bit_buffer;
		} else if (bits_in_buffer > 0) {
			index = (bit_buffer << (TABLE_BITS - bits_in_buffer));
		}

		if (index >= TABLE_SIZE) {
			fprintf(stderr, "Index out of bounds.\n");
			if (table) free(table);
			return 1;
		}

		HashDecodeEntry* entry = &(table[index]);

		if (fwrite(entry->symbol, 1, ht->symbol_length, out) != ht->symbol_length) {
			// Error
			return 1;
		}

		bytes_written += ht->symbol_length;

		if (entry->code_len > bits_in_buffer) {
    			fprintf(stderr,
            			"Need %u bits, have %u\n",
            			(unsigned)entry->code_len,
           			(unsigned)bits_in_buffer);

    			fprintf(stderr,
            			"index=%u bit_buffer=0x%08x\n",
            			index,
            			bit_buffer);
			free(table);
    			return 1;
		}

		bits_in_buffer -= entry->code_len;
		if (bits_in_buffer  < 0) {
			// Error
			free(table);
			return 1;
		}
		bit_buffer &= ((uint32_t)1 << bits_in_buffer) - 1;
	}

	free(table);
	return 0;
}

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

	int writing_result = writing_decoded_file(input, output, ht, *original_file_size_ptr);
	if (writing_result == 1) {
		free(original_file_size_ptr);
		free_tree(ht);
		return 1;
	}

	free(original_file_size_ptr);
	free_tree(ht);
	return 0;
}
