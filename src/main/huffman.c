#include "huffman.h"

Node* create_node(uint8_t* symbol, uint32_t freq, uint8_t symbol_len) {
	Node* node = (Node*)malloc(sizeof(Node));
	if (!node) {
		fprintf(stderr, "Failed to allocate memory for a node.\n");
		return NULL;
	}

	node->frequency = freq;
	node->symbol_length = symbol_len;
	node->right = NULL;
	node->left = NULL;

	if (!symbol) {
		node->symbol_data = NULL;
		return node;
	}

	node->symbol_data = (uint8_t*)malloc(sizeof(uint8_t) * symbol_len);
	if (!node->symbol_data) {
		fprintf(stderr, "Failed to allocate memory for symbol data in a node.\n");
	}

	memcpy(node->symbol_data, symbol, symbol_len);

	return node;
}

void free_node(Node* node) {
	if (!node) return NULL;
	if (node->right) free_node(node->right);
	if (node->left) free_node(node->left);
	if (node->symbol_data) free(node->symbol_data);
	free(node);
}

uint8_t node_is_leaf(Node* node) {
	if (node->left || node->right) return 0;
	return 1;
}

PriorityQueue* pq_create(uint32_t initial_capacity) {

	PriorityQueue* pq = (PriorityQueue*)malloc(sizeof(PriorityQueue));
	if (!pq) {
		fprintf(stderr, "Failed to allocate memory for a priority queue.\n");
		return NULL;
	}

	pq->nodes = (Node**)malloc(sizeof(Node*) * initial_capacity);
	if (!pq->nodes) {
		fprintf(stderr, "Failed to allocate memory for nodes in a priority queue.\n");
		return NULL;
	}

	pq->size = 0;
	pq->capacity = initial_capacity;
	return pq;
}

void pq_push(PriorityQueue* pq, Node* node) {
	if (!pq) {
		fprintf(stderr, "Invalid priority queue passed to pq_push.\n");
		return;
	}

	if (!node) {
		fprintf(stderr, "Invalid node passed to pq_push.\n");
		return;
	}

	if (pq->size + 1 > pq->capacity) {
		pq->capacity *= 2;
		pq->nodes = (Node**)realloc(sizeof(Node*) * pq->capacity);
	}

	pq->nodes[pq->size] = node;
	uint32_t current_index = pq->size;
	pq->size++;

	while (current_index >= 1 && pq->nodes[current_index - 1]->frequency < node->frequency) {
		pq->nodes[current_index] = pq->nodes[current_index - 1];
		pq->nodes[current_index - 1] = node;
		current_index--;
	}
}

Node* pq_pop(PriorityQueue* pq) {
	if (!pq) {
		fprintf(stderr, "Invalid priority queue passed to pq_pop.\n");
		return NULL;
	}

	if (!pq->nodes[pq->size - 1]) {
		fprintf(stderr, "Invalid node at the end of priority queue passed to pq_pop.\n");
		return NULL;
	}

	Node* node = pq->nodes[pq->size - 1];
	pq->nodes[pq->size - 1] = NULL;
	pq->size--;
	return node;
}

Node* pq_merge(PriorityQueue* pq) {
	if (!pq) {
		fprintf(stderr, "Invalid priority queue passed to pq_merge.\n");
		return NULL;
	}

	while (pq->size > 1) {
		Node* node1 = pq_pop(pq);
		Node* node2 = pq_pop(pq);
		Node* res_node = create_node(NULL, node1->frequency + node2->frequency, node1->symbol_length);
		if (node1->frequency < node2->frequency) {
			res_node->right = node1;
			res_node->left = node2;
		} else {
			res_node->right = node2;
			res_node->left = node1;
		}
		pq_push(pq, res_node);
	}

	return pq->nodes[0];
}

void pq_free(PriorityQueue* pq) {
	if (!pq) return NULL;
	free(pq->nodes);
	free(pq);
}

HuffmanTree* create_tree(Node* node, uint8_t symbol_len) {
	HuffmanTree* ht = (HuffmanTree*)malloc(sizeof(HuffmanTree));
	if (!ht) {
		fprintf(stderr, "Failed to allocate memory for the huffman tree.\n");
		return NULL;
	}

	ht->root = node;
	ht->codes = NULL;
	ht->symbols = NULL;
	ht->code_lengths = NULL;
	ht->symbol_length = symbol_len;
	ht->symbols_count = 0;
	return ht;
}

void free_tree(HuffmanTree* ht) {
	if (!ht) return NULL;
	if (ht->root) free_node(ht->root);
	if (ht->codes) {
		for (int i = 0; i < ht->symbols_count; i++) {
			if (ht->codes[i]) free(ht->codes[i]);
		}
		free(ht->codes);
	}
	if (ht->symbols) free(ht->symbols);
	if (ht->code_lengths) free(ht->code_lengths);
	free(ht);
}

HashTable* create_hash_table(uint32_t capacity) {
	HashTable* table = (HashTable*)malloc(sizeof(HashTable));
	if (table == NULL) {
		fprintf(stderr, "Failed to allocate memory for a hash table.\n");
		return NULL;
	}

	table->buckets = calloc(capacity, sizeof(HashTableEntry*));
	if (!table->buckets) {
		fprintf(stderr, "Failed to allocate memory for buckets in hash table.\n");
		free(table);
		return NULL;
	}

	table->size = 0;
	table->capacity = capacity;
	return table;
}

void free_hash_table(HashTable* table) {
	if (!table) return;
	if (table->buckets) free(table->buckets);
	// Again, should I be worried here about the Individual entries?.. Yeah, I should
	// So I write free_hash_entry function
	free(table);
}

void free_hash_entry(HashTableEntry* entry) {
	if (!entry) return;
	if (entry->symbol_data) free(entry->symbol_data);
	if (entry->next) free_hash_entry(entry->next);
}

uint32_t hash_function(uint8_t* symbol, uint8_t symbol_len, uint32_t table_size) {
	uint32_t hash = 5381;
	for (uint8_t i = 0; i < symbol_len; i++) {
		hash = ((hash << 5) + hash) + symbol[i];
	}
	return hash % table_size;
}

void add_symbol_hash(HashTable* table, uint8_t* symbol, uint8_t symbol_len, uint32_t code, uint8_t code_len) {
	uint32_t hash_index = hash_function(symbol, symbol_len, table->capacity);

	HashTableEntry* current = table->buckets[hash_index];
	while (current) {
		if (compare_symbols(current->symbol_data, symbol, symbol_len) == 0) {
			current->frequency++;
			return;
		}
		current = current->next;
	}

	HashTableEntry* new_entry = (HashTableEntry*)malloc(sizeof(HashTableEntry));
	if (!new_entry) {
		// Error handling
		return;
	}
	new_entry->symbol_data = (uint8_t*)malloc(symbol_len);
	memcpy(new_entry->symbol_data, symbol, symbol_len);
	new_entry->frequency = 1;
	new_entry->symbol_len = symbol_len;
	new_entry->next = table->buckets[hash_index];
	new_entry->code_len = code_len;
	new_entry->code = code;
	table->buckets[hash_index] = new_entry;
	table->size++;
}

int compare_symbols(uint8_t* symbol1, uint8_t* symbol2, uint8_t symbol_len) {
	return memcmp(symbol1, symbol2, symbol_len);
}
