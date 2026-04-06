#include "huffman.h"

Node* create_node(uint8_t* symbol, uint32_t freq, uint8_t symbol_len) {
	Node* node = (Node*)malloc(sizeof(Node));
	// if (!node) {}
	// Should I even allocate memory for symbol data if symbol is NULL? 
	node->symbol_data = (uint8_t*)malloc(sizeof(uint8_t) * symbol_len);
	// if (!symbol_data) {}
	node->frequency = freq;
	node->symbol_length = symbol_len;
	node->right = NULL;
	node->left = NULL;
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
	else return 1;
}

PriorityQueue* pq_create(uint32_t initial_capacity) {
	PriorityQueue* pq = (PriorityQueue*)malloc(sizeof(PriorityQueue));
	// if (!pq) {}
	pq->nodes = (Node**)malloc(sizeof(Node*) * initial_capacity);
	// if (!nodes) {}
	pq->size = 0;
	pq->capacity = initial_capacity;
	return pq;
}

void pq_push(PriorityQueue* pq, Node* node) {
	// if (!pq && !node) {}
	// if (!pq) {}
	// if (!node) {}
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
	Node* node = pq->nodes[pq->size - 1];
	pq->nodes[pq->size - 1] = NULL;
	pq->size--;
	return node;
}

Node* pq_merge(PriorityQueue* pq) {
	// if (!pq) {}
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
	/* for (uint32_t i = 0; i < pq->size; i++) {
		free_node(pq->nodes[i]);
	}
	free(pq->nodes); */
	free(pq->nodes);
	free(pq);
}

HuffmanTree* create_tree(Node* node, uint8_t symbol_len) {
	HuffmanTree* ht = (HuffmanTree*)malloc(sizeof(HuffmanTree));
	if (ht == NULL) {
		// Error handling
		return NULL;
	}

	ht->root = node;
	ht->codes = NULL;
	// ht->frequencies = NULL;
	ht->symbols = NULL;
	ht->code_lengths = NULL;
	ht->symbol_length = symbol_len;
	ht->symbols_count = 0;
	return ht;
}

void free_tree(HuffmanTree* ht) {
	if (!ht) return NULL;
	if (ht->root) free_node(ht->root);
	if (ht->codes) free(ht->codes);
	// if (ht->frequencies) free(ht->frequencies);
	if (ht->symbols) free(ht->symbols);
	if (ht->code_lengths) free(ht->code_lengths);
	free(ht);
}

HashTable* create_hash_table(uint32_t capacity) {
	HashTable* table = (HashTable*)malloc(sizeof(HashTable));
	if (table == NULL) {
		// Error handling
		return NULL;
	}
	table->buckets = calloc(capacity, sizeof(HashTableEntry*));
	if (!table->buckets) {
		// Error handling
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
	free(table);
}

uint32_t hash_function(uint8_t* symbol, uint8_t symbol_len, uint32_t table_size) {
	uint32_t hash = 5381;
	for (uint8_t i = 0; i < symbol_len; i++) {
		hash = ((hash << 5) + hash) + symbol[i];
	}
	return hash % table_size;
}

void add_symbol_hash(HashTable* table, uint8_t* symbol, uint8_t symbol_len) {
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
	table->buckets[hash_index] = new_entry;
	table->size++;
}

int compare_symbols(uint8_t* symbol1, uint8_t* symbol2, uint8_t symbol_len) {
	return memcmp(symbol1, symbol2, symbol_len);
}