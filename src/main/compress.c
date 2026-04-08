#include "huffman.h"

HuffmanTree* count_freq_1b(FILE* file) {
	uint32_t* freq = (uint32_t*)calloc(sizeof(uint32_t) * 256, 1);
	if (!freq) {
		fprintf(stderr, "Failed to allocate memory for symbol frequncies during 1 byte compression.\n");
		return NULL;
	}

	uint8_t* byte = (uint8_t*)malloc(sizeof(uint8_t));
	if (!byte) {
		fprintf(stderr, "Failed to allocate memory.\n");
		free(freq);
		return NULL;
	}

	while (fread(byte, 1, 1, file)) {
		freq[byte[0]]++;
	}

	rewind(file);

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
	HuffmanTree* ht = create_tree(root, 1);

	ht->codes = (uint32_t*)malloc(sizeof(uint32_t) * sym_count);
	if (!ht->codes) {
		fprintf(stderr, "Failed to allocate memory while counting frequencies.\n");
		free(freq);
		free(byte);
		free(pq);
		free_tree(ht);
		return NULL;
	}

	ht->code_lengths = (uint8_t*)malloc(sizeof(uint8_t) * sym_count);
	if (!ht->code_lengths) {
		fprintf(stderr, "Failed to allocate memory while counting frequencies.\n");
		free(freq);
		free(byte);
		free(pq);
		free_tree(ht);
		return NULL;
	}

	ht->symbols = (uint8_t**)malloc(sizeof(uint8_t*) * sym_count);
	if (!ht->symbols) {
		fprintf(stderr, "Failed to allocate memory while counting frequencies.\n");
		free(freq);
		free(byte);
		free(pq);
		free_tree(ht);
		return NULL;
	}

	if (sym_count == 1) {
		fread(byte, 1, 1, file);
		ht->codes[0] = 0;
		ht->code_lengths[0] = 1;
		ht->symbols[0] = byte;
		ht->symbols_count = 1;
		rewind(file);
	} else {
		encoding(ht);
	}

	free(freq);
	free(byte);
	pq_free(pq);
	return ht;
}

HuffmanTree* count_freq_hash(FILE* file, uint8_t symbol_len) {
	uint32_t table_size = 0;

	if (symbol_len == 2) {
		table_size = 8192;
	} else if (symbol_len == 3) {
		table_size = 16384;
	} else if (symbol_len == 4) {
		table_size = 32768;
	}

	HashTable* hash_table = create_hash_table(table_size);

	uint8_t* buffer = (uint8_t*)malloc(sizeof(uint8_t) * symbol_len);
	if (!buffer) {
		fprintf(stderr, "Failed to allocate memory while counting frequencies.\n");
		free_hash_table(hash_table);
		return NULL;
	}

	uint8_t bytes_read = 0;

	while ((bytes_read = fread(buffer, 1, symbol_len, file)) == symbol_len) {
		add_symbol_hash(hash_table, buffer, symbol_len, 0, 0);
	}

	if (bytes_read > 0) {
		memset(buffer + bytes_read, 0, symbol_len - bytes_read);
		add_symbol_hash(hash_table, buffer, symbol_len, 0, 0);
	}

	rewind(file);

	PriorityQueue* pq = pq_create(hash_table->size);
	for (int i = 0; i < table_size; i++) {
		HashTableEntry* entry = hash_table->buckets[i];
		while (entry) {
			Node* leaf = create_node(entry->symbol_data, entry->frequency, symbol_len);
			pq_push(pq, leaf);
			entry = entry->next;
		}
	}

	uint32_t sym_count = pq->size;
	Node* root = pq_merge(pq);
	HuffmanTree* ht = create_tree(root, symbol_len);

	ht->codes = (uint32_t*)malloc(sizeof(uint32_t) * sym_count);
	if (!ht->codes) {
		fprintf(stderr, "Failed to allocate memory while counting frequncies.\n");
		free_hash_table(hash_table);
		free(buffer);
		pq_free(pq);
		free_tree(ht);
		return NULL;
	}

	ht->code_lengths = (uint8_t*)malloc(sizeof(uint8_t) * sym_count);
	if (!ht->code_lengths) {
		fprintf(stderr, "Failed to allocate memory while counting frequncies.\n");
		free_hash_table(hash_table);
		free(buffer);
		pq_free(pq);
		free_tree(ht);
		return NULL;
	}

	ht->symbols = (uint8_t**)malloc(sizeof(uint8_t*) * sym_count);
	if (!ht->symbols) {
		fprintf(stderr, "Failed to allocate memory while counting frequncies.\n");
		free_hash_table(hash_table);
		free(buffer);
		pq_free(pq);
		free_tree(ht);
		return NULL;
	}

	if (sym_count == 1) {
		fread(buffer, 1, symbol_len, file);
		ht->codes[0] = 0;
		ht->code_lengths[0] = 1;
		ht->symbols[0] = buffer;
		ht->symbols_count = 1;
		rewind(file);
	} else {
		encoding(ht);
	}

	free(buffer);
	pq_free(pq);
	free_hash_table(hash_table);
	return ht;
}

void encoding(HuffmanTree* ht) {
	encoding_recursion(ht, ht->root, 0, 0);
}

void encoding_recursion(HuffmanTree* ht, Node* node, uint32_t code, uint8_t code_len) {
	if (node_is_leaf(node)) {
		ht->codes[ht->symbols_count] = code;
		ht->symbols[ht->symbols_count] = node->symbol_data;
		ht->code_lengths[ht->symbols_count] = code_len;
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
		fprintf(stderr, "Error writing header to file.\n");
		return 1;
	}

	uint8_t version = 1;
	if (fwrite(&version, 1, 1, out) != 1) {
		fprintf(stderr, "Error writing header to file.\n");
		return 1;
	}

	uint8_t symbol_size = ht->symbol_length;
	if (fwrite(&symbol_size, 1, 1, out) != 1) {
		fprintf(stderr, "Error writing header to file.\n");
		return 1;
	}

	if (fwrite(&original_file_size, 1, 8, out) != 8) {
		fprintf(stderr, "Error writing header to file.\n");
		return 1;
	}

	uint32_t symbols_count = ht->symbols_count;

	if (symbols_count == 0) return 0;

	if (fwrite(&symbols_count, 1, 4, out) != 4) {
		fprintf(stderr, "Error writing header to file.\n");
		return 1;
	}

	for (int sym_index = 0; sym_index < symbols_count; sym_index++) {
		uint8_t* sym_data = ht->symbols[sym_index];
		if (fwrite(sym_data, 1, symbol_size, out) != symbol_size) {
			fprintf(stderr, "Error writing header to file.\n");
			return 1;
		}

		uint8_t code_len = ht->code_lengths[sym_index];
		if (fwrite(&code_len, 1, 1, out) != 1) {
			fprintf(stderr, "Error writing header to file.\n");
			return 1;
		}
	}

	return 0;
}

int write_encoded_file_1b(FILE* input, FILE* output, HuffmanTree* ht) {

	uint32_t* codes = (uint32_t*)calloc(256, sizeof(uint32_t));
	if (!codes) {
		fprintf(stderr, "Failed to allocate memory while writing encoded file.\n");
		return 1;
	}

	uint8_t* code_lengths = (uint8_t*)calloc(256, sizeof(uint8_t));
	if (!code_lengths) {
		fprintf(stderr, "Failed to allocate memory while writing encoded file.\n");
		free(codes);
		return 1;
	}


	uint8_t* buffer = (uint8_t*)malloc(sizeof(uint8_t));
	if (!buffer) {
		fprintf(stderr, "Failed to allocate memory while writing encoded file.\n");
		free(codes);
		free(code_lengths);
		return 1;
	}

	for (int i = 0; i < ht->symbols_count; i++) {
		codes[ht->symbols[i][0]] = ht->codes[i];
		code_lengths[ht->symbols[i][0]] = ht->code_lengths[i];
	}

	uint8_t byte = 0;
	uint8_t bits_read = 0;

	while (fread(buffer, 1, 1, input) == 1) {
		uint32_t code = codes[buffer[0]];
		uint8_t code_len = code_lengths[buffer[0]];
		while (code_len > 0) {
			byte = (byte << 1) | ((code >> (code_len - 1)) & 1);
			code_len--;
			bits_read++;
			if (bits_read == 8) {
				if (fwrite(&byte, 1, 1, output) != 1) {
					fprintf(stderr, "Error writing encoded file.\n");
					free(codes);
					free(code_lengths);
					free(buffer);
					return 1;
				}
				byte = 0;
				bits_read = 0;
			}
		}
	}

	if (bits_read > 0) {
		byte = byte << (8 - bits_read);
		if (fwrite(&byte, 1, 1, output) != 1) {
			fprintf(stderr, "Error writing encoded file.\n");
			free(codes);
			free(code_lengths);
			free(buffer);
			return 1;
		}
	}

	free(codes);
	free(code_lengths);
	free(buffer);
	return 0;
}

int write_encoded_file_hash(FILE* input, FILE* output, HuffmanTree* ht) {

	uint8_t symbol_len = ht->symbol_length;

	uint32_t table_size = 0;
	if (symbol_len == 2) {
		table_size = 8192;
	} else if (symbol_len == 3) {
		table_size = 16384;
	} else if (symbol_len == 4) {
		table_size = 32768;
	}

	uint8_t* buffer = (uint8_t*)malloc(sizeof(uint8_t) * symbol_len);
	if (!buffer) {
		fprintf(stderr, "Failed to allocate memory while writing encoded file.\n");
		return 1;
	}

	uint8_t bytes_read = 0;
	HashTable* table = create_hash_table(table_size);

	for (int i = 0; i < ht->symbols_count; i++) {
		add_symbol_hash(table, ht->symbols[i], symbol_len, ht->codes[i], ht->code_lengths[i]);
	}

	uint8_t byte = 0;
	uint8_t bits_read = 0;

	while ((bytes_read = fread(buffer, 1, symbol_len, input)) == symbol_len) {
		uint32_t hash_index = hash_function(buffer, symbol_len, table_size);
		HashTableEntry* entry = table->buckets[hash_index];
		while (entry && memcmp(entry->symbol_data, buffer, symbol_len) != 0) {
			entry = entry->next;
		}
		// Check for NULL entry and NULL data in it?
		uint32_t code = entry->code;
		uint8_t code_len = entry->code_len;
		while (code_len > 0) {
			byte = (byte << 1) | ((code >> (code_len - 1)) & 1);
			code_len--;
			bits_read++;
			if (bits_read == 8) {
				if (fwrite(&byte, 1, 1, output) != 1) {
					fprintf(stderr, "Error writing encoded message.\n");
					free(buffer);
					free_hash_table(table);
					return 1;
				}
				byte = 0;
				bits_read = 0;
			}
		}
	}

	if (bits_read > 0) {
		byte = byte << (8 - bits_read);
		if (fwrite(&byte, 1, 1, output) != 1) {
			fprintf(stderr, "Error writing encoded message.\n");
			free(buffer);
			free_hash_table(table);
			return 1;
		}
	}

	free(buffer);
	free_hash_table(table);
	return 0;
}

int compress_file(FILE* input, FILE* output, uint8_t symbol_len, uint64_t original_file_size) {
	if (!input || !output) {
		fprintf(stderr, "Invalid input and output streams passed to compress_file.\n");
		return 1;
	}

	HuffmanTree* tree = NULL;

	if (original_file_size == 0) {
		tree = create_tree(NULL, symbol_len);
	} else if (symbol_len == 1) {
		tree = count_freq_1b(input);
	} else {
		tree = count_freq_hash(input, symbol_len);
	}

	if (tree == NULL) {
		fprintf(stderr, "Error assembling the Huffman tree.\n");
		return 1;
	}

	if (write_header_to_file(output, tree, original_file_size) != 0) {
		free_tree(tree);
		return 1;
	}

	if (original_file_size == 0) {
		free_tree(tree);
		return 0;
	}

	if (symbol_len == 1) {
		if (write_encoded_file_1b(input, output, tree) != 0) {
			free_tree(tree);
			return 1;
		}
	} else {
		if (write_encoded_file_hash(input, output, tree) != 0) {
			free_tree(tree);
			return 1;
		}
	}

	free_tree(tree);
	return 0;
}
