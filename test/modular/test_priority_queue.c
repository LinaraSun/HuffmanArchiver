#include <assert.h>
#include "huffman.h"

void test_empty_pq() {
	PriorityQueue* pq = pq_create(256);
	assert(pq->size == 0);
	assert(pq_pop(pq) == NULL);
	pq_free(pq);
}

void test_one_node() {
	PriorityQueue* pq = pq_create(256);

	uint8_t symbol[] = {'A'};

	Node* node = create_node(symbol, 5, 1);

	pq_push(pq, node);

	assert(pq->size == 1);
	assert(pq_pop(pq) == node);
	assert(pq->size == 0);

	free(node->symbol_data);
	free_node(node);
	pq_free(pq);
}

void test_multiple_nodes() {
	PriorityQueue* pq = pq_create(256);

	uint8_t symbolA[] = {'A'};
	uint8_t symbolB[] = {'B'};
	uint8_t symbolC[] = {'C'};
	uint8_t symbolD[] = {'D'};
	uint8_t symbolE[] = {'E'};

	Node* node1 = create_node(symbolA, 5, 1);
	Node* node2 = create_node(symbolB, 8, 1);
	Node* node3 = create_node(symbolC, 11, 1);
	Node* node4 = create_node(symbolD, 2, 1);
	Node* node5 = create_node(symbolE, 16, 1);

	pq_push(pq, node1);
	pq_push(pq, node2);
	pq_push(pq, node3);
	pq_push(pq, node4);
	pq_push(pq, node5);

	Node* last_node = pq_pop(pq);
	assert(last_node == node4);

	last_node = pq_pop(pq);
	assert(last_node == node1);

	last_node = pq_pop(pq);
	assert(last_node == node2);

	last_node = pq_pop(pq);
	assert(last_node == node3);

	last_node = pq_pop(pq);
	assert(last_node == node5);

	free(node1->symbol_data);
	free(node2->symbol_data);
	free(node3->symbol_data);
	free(node4->symbol_data);
	free(node5->symbol_data);

	free_node(node1);
	free_node(node2);
	free_node(node3);
	free_node(node4);
	free_node(node5);

	pq_free(pq);
}

void test_same_freq_nodes() {
	PriorityQueue* pq = pq_create(256);

	uint8_t symbolA[] = {'A'};
	uint8_t symbolB[] = {'B'};
	uint8_t symbolC[] = {'C'};
	uint8_t symbolD[] = {'D'};

	Node* node1 = create_node(symbolD, 5, 1);
	Node* node2 = create_node(symbolA, 5, 1);
	Node* node3 = create_node(symbolC, 5, 1);
	Node* node4 = create_node(symbolB, 5, 1);

	pq_push(pq, node1);
	pq_push(pq, node2);
	pq_push(pq, node3);
	pq_push(pq, node4);

	Node* last_node = pq_pop(pq);
	assert(last_node == node1);

	last_node = pq_pop(pq);
	assert(last_node == node3);

	last_node = pq_pop(pq);
	assert(last_node == node4);

	last_node = pq_pop(pq);
	assert(last_node == node2);

	free(node1->symbol_data);
	free(node2->symbol_data);
	free(node3->symbol_data);
	free(node4->symbol_data);

	free_node(node1);
	free_node(node2);
	free_node(node3);
	free_node(node4);

	pq_free(pq);
}

void test_leaf_and_internal() {
	PriorityQueue* pq = pq_create(256);

	uint8_t symbolA[] = {'A'};
	uint8_t symbolB[] = {'B'};
	uint8_t symbolC[] = {'C'};

	Node* node1 = create_node(symbolA, 5, 1);
	Node* node2 = create_node(symbolB, 3, 1);
	Node* leaf = create_node(symbolC, 8, 1);

	Node* internal = create_node(NULL, node1->frequency + node2->frequency, 1);
	internal->left = node1;
	internal->right = node2;

	pq_push(pq, leaf);
	pq_push(pq, internal);

	assert(leaf == pq_pop(pq));
	assert(internal == pq_pop(pq));

	free(node1->symbol_data);
	free(node2->symbol_data);
	free(leaf->symbol_data);

	free_node(internal);
	free_node(leaf);

	pq_free(pq);
}

void test_merge() {
	PriorityQueue* pq = pq_create(256);

	uint8_t symbolA[] = {'A'};
	uint8_t symbolB[] = {'B'};
	uint8_t symbolC[] = {'C'};
	uint8_t symbolD[] = {'D'};
	uint8_t symbolE[] = {'E'};

	Node* node1 = create_node(symbolA, 5, 1);
	Node* node2 = create_node(symbolB, 3, 1);
	Node* node3 = create_node(symbolC, 7, 1);
	Node* node4 = create_node(symbolD, 5, 1);
	Node* node5 = create_node(symbolE, 8, 1);

	pq_push(pq, node1);
	pq_push(pq, node2);
	pq_push(pq, node3);
	pq_push(pq, node4);
	pq_push(pq, node5);

	Node* root = pq_merge(pq);

	assert(root != NULL);
	assert(root->right != NULL);
	assert(root->right->right == node1);
	assert(root->right->left == node3);
	assert(root->left != NULL);
	assert(root->left->right == node5);
	assert(root->left->left != NULL);
	assert(root->left->left->right == node2);
	assert(root->left->left->left == node4);

	free(node1->symbol_data);
	free(node2->symbol_data);
	free(node3->symbol_data);
	free(node4->symbol_data);
	free(node5->symbol_data);

	free_node(root);

	pq_free(pq);
}

int main() {

	printf("Running tests: priority queue.\n\n");
	
	test_empty_pq();
	printf("Test 1: Empty priority queue - PASS\n");

	test_one_node();
	printf("Test 2: One node in priority queue - PASS\n");

	test_multiple_nodes();
	printf("Test 3: Multiple nodes in priority queue - PASS\n");

	test_same_freq_nodes();
	printf("Test 4: Pushing nodes with same frequency to priority queue - PASS\n");

	test_leaf_and_internal();
	printf("Test 5: Pushing a leaf and an internal node to priority queue - PASS\n");

	test_merge();
	printf("Test 6: Merging the priority queue - PASS\n\n");

	printf("Priority queue: all tests passed.\n");

	return 0;
}
