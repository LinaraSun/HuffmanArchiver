#include <assert.h>
#include "huffman.h"

void test_two_nodes_encoding() {
	uint8_t symbolA[] = {'A'};
	uint8_t symbolB[] = {'B'};

	Node* node1 = create_node(symbolA, 3, 1);
	Node* node2 = create_node(symbolB, 5, 1);
	Node* root = create_node(NULL, node1->frequency + node2->frequency, 1);
	root->right = node1;
	root->left = node2;

	HuffmanTree* ht = create_tree(root, 1);
	ht->symbols = (uint8_t**)malloc(sizeof(uint8_t*) * 2);
	ht->code_lengths = (uint8_t*)malloc(2);
	ht->codes = (uint32_t*)malloc(sizeof(uint32_t) * 2);

	counting_code_lengths(ht, root);

	assert(ht->symbols_count == 2);
	assert(ht->code_lengths[0] == 1);
	assert(ht->code_lengths[1] == 1);

	encoding_from_len(ht);

	assert(ht->symbols[0][0] == symbolA[0]);
	assert(ht->symbols[1][0] == symbolB[0]);

	assert(ht->codes[0] == 0);
	assert(ht->codes[1] == 1);

	free_tree(ht);
}

void test_three_nodes_encoding() {
	uint8_t symbolA[] = {'A'};
	uint8_t symbolB[] = {'B'};
	uint8_t symbolC[] = {'C'};

	Node* node1 = create_node(symbolA, 3, 1);
	Node* node2 = create_node(symbolB, 5, 1);
	Node* node3 = create_node(symbolC, 7, 1);

	Node* root = create_node(NULL, node1->frequency + node2->frequency + node3->frequency, 1);
	root->right = node3;
	root->left = create_node(NULL, node1->frequency + node2->frequency, 1);
	root->left->right = node1;
	root->left->left = node2;

	HuffmanTree* ht = create_tree(root, 1);
	ht->symbols = (uint8_t**)malloc(sizeof(uint8_t*) * 3);
	ht->code_lengths = (uint8_t*)malloc(3);
	ht->codes = (uint32_t*)malloc(sizeof(uint32_t) * 3);

	counting_code_lengths(ht, root);

	assert(ht->symbols_count == 3);
	assert(ht->code_lengths[0] == 1);
	assert(ht->code_lengths[1] == 2);
	assert(ht->code_lengths[2] == 2);

	encoding_from_len(ht);

	assert(ht->symbols[0][0] == symbolC[0]);
	assert(ht->symbols[1][0] == symbolA[0]);
	assert(ht->symbols[2][0] == symbolB[0]);

	assert(ht->codes[0] == 0);
	assert(ht->codes[1] == 2);
	assert(ht->codes[2] == 3);

	free_tree(ht);
}

int main() {
	printf("Running tests: huffman tree.\n\n");

	test_two_nodes_encoding();
	printf("Test 1: Two nodes - PASS\n");

	test_three_nodes_encoding();
	printf("Test 2: Three nodes - PASS\n\n");

	printf("Huffman tree: all tests passed.\n");

	return 0;
}
