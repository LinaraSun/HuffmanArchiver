#include "huffman.h"

HuffmanTree* count_freq_1b(FILE* file) {
	uint32_t* freq = (uint32_t*)malloc(sizeof(uint32_t) * 256);
	if (freq == NULL) {
		// Error handling
		return NULL;
	}
	for (int i = 0; i < 256; i++) {
		freq[i] = 0;
	}
	uint8_t* byte = (uint8_t)malloc(sizeof(uint8_t));
	if (byte == NULL) {
		// Error handling
		free(freq);
		return NULL;
	}
	while (fread(byte, 1, 1, file)) {
		freq[byte[0]]++;
	}

	uint32_t sym_count = 0;
	PriorityQueue* pq = pq_create(256);
	for (int i = 0; i < 256; i++) {
		if (freq[i] > 0) {
			uint8_t symbol = (uint8_t)i;
			sym_count++;
			Node* node = create_node(&symbol, freq[i], 1);
			pq_push(pq, node);
		}
	}

	Node* root = pq_merge(pq);
	pq_free(pq);
	HuffmanTree* ht = create_tree(root, 1);
	ht->codes = (uint32_t*)malloc(sizeof(uint32_t) * sym_count);
	ht->code_lengths = (uint8_t*)malloc(sizeof(uint8_t) * sym_count);
	// ht->frequencies = (uint32_t*)malloc(sizeof(uint32_t) * sym_count);
	ht->symbols = (uint8_t**)malloc(sizeof(uint8_t*) * sym_count);

	if (sym_count == 0) {
		// Empty file
	} else if (sym_count == 1) {
		ht->symbols_count = 1;
	} else {
		encoding(ht);
	}

	free(freq);
	free(byte);
	return ht;
}

HuffmanTree* count_freq_hash(FILE* file, uint8_t symbol_len) {}

void encoding(HuffmanTree* ht) {
	encoding_recursion(ht, ht->root, 0, 0);
}

void encoding_recursion(HuffmanTree* ht, Node* node, uint32_t code, uint8_t code_len) {
	if (node_is_leaf(node)) {
		ht->codes[ht->symbols_count] = code;
		// ht->frequencies[ht->symbols_count] = node->frequency;
		ht->symbols[ht->symbols_count] = node->symbol_data;
		ht->code_lengths[ht->symbols_count] = code_len; // + 1?
		ht->symbols_count++;
	} else {
		if (node->right) {
			encoding_recursion(ht, node->right, code << 1, code_len + 1);
		}

		if (node->left) {
			encoding_recursion(ht, node->left, code << 1 + 1, code_len + 1);
		}
	}
}

int write_header_to_file(FILE* out, HuffmanTree* ht, uint64_t original_file_size) {
	if (fwrite("HUFF", 1, 4, out) != 4) {
		// Error
	}

	uint8_t version = 1;
	if (fwrite(&version, 1, 1, out) != 1) {
		// Error
	}

	uint8_t symbol_size = ht->symbol_length;
	if (fwrite(&symbol_size, 1, 1, out) != 1) {
		// Error
	}

	if (fwrite(&original_file_size, 1, 8, out) != 8) {
		// Error
	}

	uint32_t symbols_count = ht->symbols_count;
	if (fwrite(&symbols_count, 1, 4, out) != 4) {
		// Error
	}

	for (int sym_index = 0; sym_index < symbols_count; sym_index++) {
		uint8_t* sym_data = ht->symbols[sym_index];
		if (fwrite(sym_data, 1, symbol_size, out) != symbol_size) {
			// Error
		}

		uint8_t code_len = ht->code_lengths[sym_index];
		if (fwrite(&code_len, 1, 1, out) != 1) {
			// Error
		}
	}

	return 0;
}

int compress_file(FILE* input, FILE* output, uint8_t symbol_len, uint64_t original_file_size) {
	// if input/output files are invalid, the program doesn't get to this point

	HuffmanTree* tree = NULL;
	if (symbol_len == 1) {
		HuffmanTree* tree = count_freq_1b(input);
	} else {
		// HuffmanTree* tree = count_freq_hash()
	}

	if (write_header_to_file(output, tree, original_file_size) != 0) {
		return 1;
	}

	// if (write_encoded_file(input, output, ))
}