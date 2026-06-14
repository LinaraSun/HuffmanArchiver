#include "huffman.h"

struct Node {
  uint8_t *symbol_data;
  uint32_t frequency;
  uint8_t symbol_length;
  struct Node *right;
  struct Node *left;
};

struct PriorityQueue {
  Node **nodes;
  uint32_t size;
  uint32_t capacity;
};

struct HuffmanTree {
  Node *root;
  uint32_t *codes;
  uint8_t **symbols;
  uint8_t *code_lengths;
  uint32_t symbols_count;
  uint8_t symbol_length;
};

struct HashTableEntry {
  uint8_t *symbol_data;
  uint32_t frequency;
  uint8_t symbol_length;
  uint32_t code;
  uint8_t code_length;
  struct HashTableEntry *next;
};

struct HashTable {
  HashTableEntry **buckets;
  uint32_t size;
  uint32_t capacity;
};

Node *create_node(uint8_t *symbol, uint32_t frequency, uint8_t symbol_length) {
  Node *node = (Node *)malloc(sizeof(Node));
  if (!node) {
    fprintf(stderr, "Failed to allocate memory for a node.\n");
    return NULL;
  }

  node->frequency = frequency;
  node->symbol_length = symbol_length;
  node->right = NULL;
  node->left = NULL;

  if (!symbol) {
    node->symbol_data = NULL;
    return node;
  }

  node->symbol_data = (uint8_t *)malloc(sizeof(uint8_t) * symbol_length);
  if (!node->symbol_data) {
    fprintf(stderr,
            "Failed to allocate memory for symbol data input a node.\n");
    free(node);
    return NULL;
  }

  memcpy(node->symbol_data, symbol, symbol_length);

  return node;
}

void free_node(Node *node) {
  if (!node)
    return;
  if (node->right)
    free_node(node->right);
  if (node->left)
    free_node(node->left);
  free(node);
}

uint8_t node_is_leaf(Node *node) { return !node->left && !node->right; }

PriorityQueue *pq_create(uint32_t initial_capacity) {

  PriorityQueue *priority_queue =
      (PriorityQueue *)malloc(sizeof(PriorityQueue));
  if (!priority_queue) {
    fprintf(stderr, "Failed to allocate memory for a priority queue.\n");
    return NULL;
  }

  priority_queue->nodes = (Node **)malloc(sizeof(Node *) * initial_capacity);
  if (!priority_queue->nodes) {
    fprintf(stderr,
            "Failed to allocate memory for nodes input a priority queue.\n");
    return NULL;
  }

  priority_queue->size = 0;
  priority_queue->capacity = initial_capacity;
  return priority_queue;
}

void pq_push(PriorityQueue *priority_queue, Node *node) {
  if (!priority_queue) {
    fprintf(stderr, "Invalid priority queue passed to pq_push.\n");
    return;
  }

  if (!node) {
    fprintf(stderr, "Invalid node passed to pq_push.\n");
    return;
  }

  if (priority_queue->size + 1 > priority_queue->capacity) {
    priority_queue->capacity *= 2;
    if (realloc(priority_queue->nodes,
                sizeof(Node *) * priority_queue->capacity) == NULL) {
      fprintf(stderr, "Error resizing priority queue.\n");
      return;
    }
  }

  priority_queue->nodes[priority_queue->size] = node;
  uint32_t current_index = priority_queue->size;
  priority_queue->size++;

  while (current_index >= 1 &&
         priority_queue->nodes[current_index - 1]->frequency <
             node->frequency) {
    priority_queue->nodes[current_index] =
        priority_queue->nodes[current_index - 1];
    priority_queue->nodes[current_index - 1] = node;
    current_index--;
  }

  while (current_index >= 1 &&
         priority_queue->nodes[current_index - 1]->frequency ==
             node->frequency) {
    if (node->symbol_data &&
        priority_queue->nodes[current_index - 1]->symbol_data &&
        compare_symbols(priority_queue->nodes[current_index - 1]->symbol_data,
                        node->symbol_data, node->symbol_length) > 0) {
      priority_queue->nodes[current_index] =
          priority_queue->nodes[current_index - 1];
      priority_queue->nodes[current_index - 1] = node;
      current_index--;
    } else if (node->symbol_data &&
               priority_queue->nodes[current_index - 1]->symbol_data) {
      break;
    } else if (node->symbol_data &&
               !priority_queue->nodes[current_index - 1]->symbol_data) {
      break;
    } else if (!node->symbol_data &&
               priority_queue->nodes[current_index - 1]->symbol_data) {
      priority_queue->nodes[current_index] =
          priority_queue->nodes[current_index - 1];
      priority_queue->nodes[current_index - 1] = node;
      current_index--;
    } else {
      if (height_left(node) <=
          height_left(priority_queue->nodes[current_index - 1])) {
        break;
      } else {
        priority_queue->nodes[current_index] =
            priority_queue->nodes[current_index - 1];
        priority_queue->nodes[current_index - 1] = node;
        current_index--;
      }
    }
  }
}

Node *pq_pop(PriorityQueue *priority_queue) {
  if (!priority_queue) {
    fprintf(stderr, "Invalid priority queue passed to pq_pop.\n");
    return NULL;
  }

  if (priority_queue->size == 0)
    return NULL;

  if (!priority_queue->nodes) {
    fprintf(stderr, "Invalid priority queue passed to pq_pop - no nodes.\n");
    return NULL;
  }

  if (!priority_queue->nodes[priority_queue->size - 1]) {
    fprintf(stderr,
            "Invalid node at the end of priority queue passed to pq_pop.\n");
    return NULL;
  }

  Node *node = priority_queue->nodes[priority_queue->size - 1];
  priority_queue->nodes[priority_queue->size - 1] = NULL;
  priority_queue->size--;
  return node;
}

Node *pq_merge(PriorityQueue *priority_queue) {
  if (!priority_queue) {
    fprintf(stderr, "Invalid priority queue passed to pq_merge.\n");
    return NULL;
  }

  while (priority_queue->size > 1) {
    Node *node1 = pq_pop(priority_queue);
    Node *node2 = pq_pop(priority_queue);
    Node *res_node = create_node(NULL, node1->frequency + node2->frequency,
                                 node1->symbol_length);
    if ((node1->symbol_data && node2->symbol_data) ||
        (node1->symbol_data && !node2->symbol_data)) {
      res_node->right = node1;
      res_node->left = node2;
    } else if (!node1->symbol_data && node2->symbol_data) {
      res_node->right = node2;
      res_node->left = node1;
    } else if (height_left(node1) > height_left(node2)) {
      res_node->right = node2;
      res_node->left = node1;
    } else {
      res_node->right = node1;
      res_node->left = node2;
    }
    pq_push(priority_queue, res_node);
  }

  return priority_queue->nodes[0];
}

void pq_free(PriorityQueue *priority_queue) {
  if (!priority_queue)
    return;
  if (priority_queue->nodes)
    free(priority_queue->nodes);
  free(priority_queue);
}

HuffmanTree *create_tree(Node *node, uint8_t symbol_length) {
  HuffmanTree *huffman_tree = (HuffmanTree *)malloc(sizeof(HuffmanTree));
  if (!huffman_tree) {
    fprintf(stderr, "Failed to allocate memory for the huffman tree.\n");
    return NULL;
  }

  huffman_tree->root = node;
  huffman_tree->codes = NULL;
  huffman_tree->symbols = NULL;
  huffman_tree->code_lengths = NULL;
  huffman_tree->symbol_length = symbol_length;
  huffman_tree->symbols_count = 0;
  return huffman_tree;
}

void free_tree(HuffmanTree *huffman_tree) {
  if (!huffman_tree)
    return;
  if (huffman_tree->root)
    free_node(huffman_tree->root);
  if (huffman_tree->codes)
    free(huffman_tree->codes);
  if (huffman_tree->symbols) {
    for (uint32_t i = 0; i < huffman_tree->symbols_count; i++) {
      if (huffman_tree->symbols[i])
        free(huffman_tree->symbols[i]);
    }
    free(huffman_tree->symbols);
  }
  if (huffman_tree->code_lengths)
    free(huffman_tree->code_lengths);
  free(huffman_tree);
}

HashTable *create_hash_table(uint32_t capacity) {
  HashTable *table = (HashTable *)malloc(sizeof(HashTable));
  if (table == NULL) {
    fprintf(stderr, "Failed to allocate memory for a hash table.\n");
    return NULL;
  }

  table->buckets = calloc(capacity, sizeof(HashTableEntry *));
  if (!table->buckets) {
    fprintf(stderr,
            "Failed to allocate memory for buckets input hash table.\n");
    free(table);
    return NULL;
  }

  table->size = 0;
  table->capacity = capacity;
  return table;
}

void free_hash_table(HashTable *table) {
  if (!table)
    return;
  if (table->buckets) {
    for (uint32_t i = 0; i < table->capacity; i++) {
      if (table->buckets[i])
        free_hash_entry(table->buckets[i]);
    }
    free(table->buckets);
  }
  free(table);
}

void free_hash_entry(HashTableEntry *entry) {
  if (!entry)
    return;
  if (entry->symbol_data)
    free(entry->symbol_data);
  if (entry->next)
    free_hash_entry(entry->next);
  free(entry);
}

uint32_t hash_function(uint8_t *symbol, uint8_t symbol_length,
                       uint32_t table_size) {
  uint32_t hash = 5381;
  for (uint8_t i = 0; i < symbol_length; i++) {
    hash = ((hash << 5) + hash) + symbol[i];
  }
  return hash % table_size;
}

void add_symbol_hash(HashTable *table, uint8_t *symbol, uint8_t symbol_length,
                     uint32_t code, uint8_t code_length) {
  uint32_t hash_index = hash_function(symbol, symbol_length, table->capacity);

  HashTableEntry *current = table->buckets[hash_index];
  while (current) {
    if (compare_symbols(current->symbol_data, symbol, symbol_length) == 0) {
      current->frequency++;
      return;
    }
    current = current->next;
  }

  HashTableEntry *new_entry = (HashTableEntry *)malloc(sizeof(HashTableEntry));
  if (!new_entry) {
    fprintf(stderr, "Failed to allocate memory for a hash table entry.\n");
    return;
  }

  new_entry->symbol_data = (uint8_t *)malloc(sizeof(uint8_t) * symbol_length);
  memcpy(new_entry->symbol_data, symbol, symbol_length);
  new_entry->frequency = 1;
  new_entry->symbol_length = symbol_length;
  new_entry->next = table->buckets[hash_index];
  new_entry->code_length = code_length;
  new_entry->code = code;
  table->buckets[hash_index] = new_entry;
  table->size++;
}

int compare_symbols(uint8_t *symbol1, uint8_t *symbol2, uint8_t symbol_length) {
  return memcmp(symbol1, symbol2, symbol_length);
}

int height_left(Node *node) {
  if (node->symbol_data) {
    return 1;
  } else {
    return height_left(node->left) + 1;
  }
}
