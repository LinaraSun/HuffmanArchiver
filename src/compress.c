#include "compress.h"

HuffmanTree *count_freq_1b(FILE *file) {
  uint32_t *frequency = (uint32_t *)calloc(sizeof(uint32_t) * 256, 1);
  if (!frequency) {
    fprintf(stderr, "Failed to allocate memory for symbol frequncies during 1 "
                    "byte compression.\n");
    return NULL;
  }

  uint8_t *byte = (uint8_t *)malloc(sizeof(uint8_t));
  if (!byte) {
    fprintf(stderr, "Failed to allocate memory.\n");
    free(frequency);
    return NULL;
  }

  while (fread(byte, 1, 1, file)) {
    frequency[byte[0]]++;
  }

  rewind(file);

  uint32_t sym_count = 0;
  PriorityQueue *priority_queue = pq_create(256);
  for (int i = 0; i < 256; i++) {
    if (frequency[i] > 0) {
      uint8_t symbol = (uint8_t)i;
      sym_count++;
      Node *node = create_node(&symbol, frequency[i], 1);
      pq_push(priority_queue, node);
    }
  }

  Node *root = pq_merge(priority_queue);
  HuffmanTree *huffman_tree = create_tree(root, 1);

  huffman_tree->codes = (uint32_t *)malloc(sizeof(uint32_t) * sym_count);
  if (!huffman_tree->codes) {
    fprintf(stderr, "Failed to allocate memory while counting frequencies.\n");
    free(frequency);
    free(byte);
    free(priority_queue);
    free_tree(huffman_tree);
    return NULL;
  }

  huffman_tree->code_lengths = (uint8_t *)malloc(sizeof(uint8_t) * sym_count);
  if (!huffman_tree->code_lengths) {
    fprintf(stderr, "Failed to allocate memory while counting frequencies.\n");
    free(frequency);
    free(byte);
    free(priority_queue);
    free_tree(huffman_tree);
    return NULL;
  }

  huffman_tree->symbols = (uint8_t **)malloc(sizeof(uint8_t *) * sym_count);
  if (!huffman_tree->symbols) {
    fprintf(stderr, "Failed to allocate memory while counting frequencies.\n");
    free(frequency);
    free(byte);
    free(priority_queue);
    free_tree(huffman_tree);
    return NULL;
  }

  if (sym_count == 1) {
    fread(byte, 1, 1, file);
    huffman_tree->codes[0] = 0;
    huffman_tree->code_lengths[0] = 1;
    huffman_tree->symbols[0] = huffman_tree->root->symbol_data;
    huffman_tree->symbols_count = 1;
    rewind(file);
  } else {
    counting_code_lengths(huffman_tree, huffman_tree->root);
    encoding_from_len(huffman_tree);
  }

  free(frequency);
  free(byte);
  pq_free(priority_queue);
  return huffman_tree;
}

HuffmanTree *count_freq_hash(FILE *file, uint8_t symbol_length) {
  uint32_t table_size = 0;

  if (symbol_length == 2) {
    table_size = 8192;
  } else if (symbol_length == 3) {
    table_size = 16384;
  } else if (symbol_length == 4) {
    table_size = 32768;
  }

  HashTable *hash_table = create_hash_table(table_size);

  uint8_t *buffer = (uint8_t *)malloc(sizeof(uint8_t) * symbol_length);
  if (!buffer) {
    fprintf(stderr, "Failed to allocate memory while counting frequencies.\n");
    free_hash_table(hash_table);
    return NULL;
  }

  uint8_t bytes_read = 0;

  while ((bytes_read = fread(buffer, 1, symbol_length, file)) ==
         symbol_length) {
    add_symbol_hash(hash_table, buffer, symbol_length, 0, 0);
  }

  if (bytes_read > 0) {
    memset(buffer + bytes_read, 0, symbol_length - bytes_read);
    add_symbol_hash(hash_table, buffer, symbol_length, 0, 0);
  }

  rewind(file);

  PriorityQueue *priority_queue = pq_create(hash_table->size);
  for (uint32_t i = 0; i < table_size; i++) {
    HashTableEntry *entry = hash_table->buckets[i];
    while (entry) {
      Node *leaf =
          create_node(entry->symbol_data, entry->frequency, symbol_length);
      pq_push(priority_queue, leaf);
      entry = entry->next;
    }
  }

  uint32_t sym_count = priority_queue->size;
  Node *root = pq_merge(priority_queue);
  HuffmanTree *huffman_tree = create_tree(root, symbol_length);

  huffman_tree->codes = (uint32_t *)malloc(sizeof(uint32_t) * sym_count);
  if (!huffman_tree->codes) {
    fprintf(stderr, "Failed to allocate memory while counting frequncies.\n");
    free_hash_table(hash_table);
    free(buffer);
    pq_free(priority_queue);
    free_tree(huffman_tree);
    return NULL;
  }

  huffman_tree->code_lengths = (uint8_t *)malloc(sizeof(uint8_t) * sym_count);
  if (!huffman_tree->code_lengths) {
    fprintf(stderr, "Failed to allocate memory while counting frequncies.\n");
    free_hash_table(hash_table);
    free(buffer);
    pq_free(priority_queue);
    free_tree(huffman_tree);
    return NULL;
  }

  huffman_tree->symbols = (uint8_t **)malloc(sizeof(uint8_t *) * sym_count);
  if (!huffman_tree->symbols) {
    fprintf(stderr, "Failed to allocate memory while counting frequncies.\n");
    free_hash_table(hash_table);
    free(buffer);
    pq_free(priority_queue);
    free_tree(huffman_tree);
    return NULL;
  }

  if (sym_count == 1) {
    fread(buffer, 1, symbol_length, file);
    huffman_tree->codes[0] = 0;
    huffman_tree->code_lengths[0] = 1;
    huffman_tree->symbols[0] = huffman_tree->root->symbol_data;
    huffman_tree->symbols_count = 1;
    rewind(file);
  } else {
    counting_code_lengths(huffman_tree, huffman_tree->root);
    encoding_from_len(huffman_tree);
  }

  free(buffer);
  pq_free(priority_queue);
  free_hash_table(hash_table);
  return huffman_tree;
}

void counting_code_lengths(HuffmanTree *huffman_tree, Node *root) {
  counting_len_recursion(huffman_tree, root, 0);
}

void counting_len_recursion(HuffmanTree *huffman_tree, Node *node,
                            uint8_t code_length) {
  if (node_is_leaf(node)) {
    huffman_tree->code_lengths[huffman_tree->symbols_count] = code_length;
    huffman_tree->symbols[huffman_tree->symbols_count] = node->symbol_data;
    huffman_tree->symbols_count++;
  } else {
    if (node->right) {
      counting_len_recursion(huffman_tree, node->right, code_length + 1);
    }

    if (node->left) {
      counting_len_recursion(huffman_tree, node->left, code_length + 1);
    }
  }
}

void encoding_from_len(HuffmanTree *huffman_tree) {

  uint8_t temp_len = 0;
  uint8_t *temp_sym = NULL;
  for (uint32_t i = 1; i < huffman_tree->symbols_count; i++) {
    uint32_t j = i;
    while (
        j >= 1 &&
        ((huffman_tree->code_lengths[j - 1] > huffman_tree->code_lengths[j]) ||
         (huffman_tree->code_lengths[j - 1] == huffman_tree->code_lengths[j] &&
          compare_symbols(huffman_tree->symbols[j - 1],
                          huffman_tree->symbols[j],
                          huffman_tree->symbol_length) > 0))) {
      temp_len = huffman_tree->code_lengths[j];
      huffman_tree->code_lengths[j] = huffman_tree->code_lengths[j - 1];
      huffman_tree->code_lengths[j - 1] = temp_len;

      temp_sym = huffman_tree->symbols[j];
      huffman_tree->symbols[j] = huffman_tree->symbols[j - 1];
      huffman_tree->symbols[j - 1] = temp_sym;

      j--;
    }
  }

  uint32_t code = 0;
  uint8_t current_len = 1;

  for (uint32_t i = 0; i < huffman_tree->symbols_count; i++) {
    if (current_len < huffman_tree->code_lengths[i]) {
      code = code << (huffman_tree->code_lengths[i] - current_len);
      current_len = huffman_tree->code_lengths[i];
    }

    huffman_tree->codes[i] = code;

    code++;
  }
}

int write_header_to_file(FILE *output, HuffmanTree *huffman_tree,
                         uint64_t original_file_size) {
  if (fwrite("HUFF", 1, 4, output) != 4) {
    fprintf(stderr, "Error writing header to file.\n");
    return 1;
  }

  uint8_t version = 1;
  if (fwrite(&version, 1, 1, output) != 1) {
    fprintf(stderr, "Error writing header to file.\n");
    return 1;
  }

  uint8_t symbol_size = huffman_tree->symbol_length;
  if (fwrite(&symbol_size, 1, 1, output) != 1) {
    fprintf(stderr, "Error writing header to file.\n");
    return 1;
  }

  if (fwrite(&original_file_size, 1, 8, output) != 8) {
    fprintf(stderr, "Error writing header to file.\n");
    return 1;
  }

  uint32_t symbols_count = huffman_tree->symbols_count;

  if (symbols_count == 0)
    return 0;

  if (fwrite(&symbols_count, 1, 4, output) != 4) {
    fprintf(stderr, "Error writing header to file.\n");
    return 1;
  }

  for (uint32_t sym_index = 0; sym_index < symbols_count; sym_index++) {
    uint8_t *sym_data = huffman_tree->symbols[sym_index];
    if (fwrite(sym_data, 1, symbol_size, output) != symbol_size) {
      fprintf(stderr, "Error writing header to file.\n");
      return 1;
    }

    uint8_t code_length = huffman_tree->code_lengths[sym_index];
    if (fwrite(&code_length, 1, 1, output) != 1) {
      fprintf(stderr, "Error writing header to file.\n");
      return 1;
    }
  }

  return 0;
}

int write_encoded_file_1b(FILE *input, FILE *output,
                          HuffmanTree *huffman_tree) {

  uint32_t *codes = (uint32_t *)calloc(256, sizeof(uint32_t));
  if (!codes) {
    fprintf(stderr, "Failed to allocate memory while writing encoded file.\n");
    return 1;
  }

  uint8_t *code_lengths = (uint8_t *)calloc(256, sizeof(uint8_t));
  if (!code_lengths) {
    fprintf(stderr, "Failed to allocate memory while writing encoded file.\n");
    free(codes);
    return 1;
  }

  uint8_t *buffer = (uint8_t *)malloc(sizeof(uint8_t));
  if (!buffer) {
    fprintf(stderr, "Failed to allocate memory while writing encoded file.\n");
    free(codes);
    free(code_lengths);
    return 1;
  }

  for (uint32_t i = 0; i < huffman_tree->symbols_count; i++) {
    codes[huffman_tree->symbols[i][0]] = huffman_tree->codes[i];
    code_lengths[huffman_tree->symbols[i][0]] = huffman_tree->code_lengths[i];
  }

  rewind(input);

  uint8_t byte = 0;
  uint8_t bits_read = 0;

  while (fread(buffer, 1, 1, input) == 1) {
    uint32_t code = codes[buffer[0]];
    uint8_t code_length = code_lengths[buffer[0]];
    while (code_length > 0) {
      byte = (byte << 1) | ((code >> (code_length - 1)) & 1);
      code_length--;
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

int write_encoded_file_hash(FILE *input, FILE *output,
                            HuffmanTree *huffman_tree) {

  uint8_t symbol_length = huffman_tree->symbol_length;

  uint32_t table_size = 0;
  if (symbol_length == 2) {
    table_size = 8192;
  } else if (symbol_length == 3) {
    table_size = 16384;
  } else if (symbol_length == 4) {
    table_size = 32768;
  }

  uint8_t *buffer = (uint8_t *)malloc(sizeof(uint8_t) * symbol_length);
  if (!buffer) {
    fprintf(stderr, "Failed to allocate memory while writing encoded file.\n");
    return 1;
  }

  uint8_t bytes_read = 0;
  HashTable *table = create_hash_table(table_size);

  for (uint32_t i = 0; i < huffman_tree->symbols_count; i++) {
    add_symbol_hash(table, huffman_tree->symbols[i], symbol_length,
                    huffman_tree->codes[i], huffman_tree->code_lengths[i]);
  }

  rewind(input);

  uint8_t byte = 0;
  uint8_t bits_read = 0;

  while ((bytes_read = fread(buffer, 1, symbol_length, input)) ==
         symbol_length) {
    uint32_t hash_index = hash_function(buffer, symbol_length, table_size);
    HashTableEntry *entry = table->buckets[hash_index];

    while (entry && memcmp(entry->symbol_data, buffer, symbol_length) != 0) {
      entry = entry->next;
    }

    if (!entry) {
      fprintf(stderr, "Symbol not present in huffman tree.\n");
      free(buffer);
      return 1;
    }

    uint32_t code = entry->code;
    uint8_t code_length = entry->code_length;
    while (code_length > 0) {
      byte = (byte << 1) | ((code >> (code_length - 1)) & 1);
      code_length--;
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

int compress_file(FILE *input, FILE *output, uint8_t symbol_length,
                  uint64_t original_file_size) {
  if (!input || !output) {
    fprintf(stderr,
            "Invalid input and/or output streams passed to compress_file.\n");
    return 1;
  }

  HuffmanTree *tree = NULL;

  if (original_file_size == 0) {
    tree = create_tree(NULL, symbol_length);
  } else if (symbol_length == 1) {
    tree = count_freq_1b(input);
  } else {
    tree = count_freq_hash(input, symbol_length);
  }

  if (!tree) {
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

  if (symbol_length == 1) {
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
