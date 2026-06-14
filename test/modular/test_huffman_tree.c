#include "compress.h"
#include <assert.h>

struct Node {
  uint8_t *symbol_data;
  uint32_t frequency;
  uint8_t symbol_length;
  struct Node *right;
  struct Node *left;
};

struct HuffmanTree {
  Node *root;
  uint32_t *codes;
  uint8_t **symbols;
  uint8_t *code_lengths;
  uint32_t symbols_count;
  uint8_t symbol_length;
};

void test_two_nodes_encoding() {
  uint8_t symbolA[] = {'A'};
  uint8_t symbolB[] = {'B'};

  Node *node1 = create_node(symbolA, 3, 1);
  Node *node2 = create_node(symbolB, 5, 1);
  Node *root = create_node(NULL, node1->frequency + node2->frequency, 1);
  root->right = node1;
  root->left = node2;

  HuffmanTree *huffman_tree = create_tree(root, 1);
  huffman_tree->symbols = (uint8_t **)malloc(sizeof(uint8_t *) * 2);
  huffman_tree->code_lengths = (uint8_t *)malloc(2);
  huffman_tree->codes = (uint32_t *)malloc(sizeof(uint32_t) * 2);

  counting_code_lengths(huffman_tree, root);

  assert(huffman_tree->symbols_count == 2);
  assert(huffman_tree->code_lengths[0] == 1);
  assert(huffman_tree->code_lengths[1] == 1);

  encoding_from_len(huffman_tree);

  assert(huffman_tree->symbols[0][0] == symbolA[0]);
  assert(huffman_tree->symbols[1][0] == symbolB[0]);

  assert(huffman_tree->codes[0] == 0);
  assert(huffman_tree->codes[1] == 1);

  free_tree(huffman_tree);
}

void test_three_nodes_encoding() {
  uint8_t symbolA[] = {'A'};
  uint8_t symbolB[] = {'B'};
  uint8_t symbolC[] = {'C'};

  Node *node1 = create_node(symbolA, 3, 1);
  Node *node2 = create_node(symbolB, 5, 1);
  Node *node3 = create_node(symbolC, 7, 1);

  Node *root = create_node(
      NULL, node1->frequency + node2->frequency + node3->frequency, 1);
  root->right = node3;
  root->left = create_node(NULL, node1->frequency + node2->frequency, 1);
  root->left->right = node1;
  root->left->left = node2;

  HuffmanTree *huffman_tree = create_tree(root, 1);
  huffman_tree->symbols = (uint8_t **)malloc(sizeof(uint8_t *) * 3);
  huffman_tree->code_lengths = (uint8_t *)malloc(3);
  huffman_tree->codes = (uint32_t *)malloc(sizeof(uint32_t) * 3);

  counting_code_lengths(huffman_tree, root);

  assert(huffman_tree->symbols_count == 3);
  assert(huffman_tree->code_lengths[0] == 1);
  assert(huffman_tree->code_lengths[1] == 2);
  assert(huffman_tree->code_lengths[2] == 2);

  encoding_from_len(huffman_tree);

  assert(huffman_tree->symbols[0][0] == symbolC[0]);
  assert(huffman_tree->symbols[1][0] == symbolA[0]);
  assert(huffman_tree->symbols[2][0] == symbolB[0]);

  assert(huffman_tree->codes[0] == 0);
  assert(huffman_tree->codes[1] == 2);
  assert(huffman_tree->codes[2] == 3);

  free_tree(huffman_tree);
}

int main() {
  printf("Running tests: huffman tree.\n\n");

  test_two_nodes_encoding();
  printf("Test 1: Two nodes - PASS\n");

  test_three_nodes_encoding();
  printf("Test 2: Three nodes - PASS\n\n");

  printf("Huffman tree: all tests passed.\n\n");

  return 0;
}
