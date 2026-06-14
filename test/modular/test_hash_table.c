#include "huffman.h"
#include <assert.h>

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

void test_empty_table() {
  HashTable *table = create_hash_table(8192);

  assert(table->size == 0);

  free_hash_table(table);
}

void test_one_entry() {
  HashTable *table = create_hash_table(8192);

  uint8_t symbol[] = {'A', 'B'};

  add_symbol_hash(table, symbol, 2, 0, 0);

  assert(table->size == 1);

  uint32_t hash_index = hash_function(symbol, 2, 8192);

  HashTableEntry *entry = table->buckets[hash_index];

  assert(entry->frequency == 1);

  free_hash_table(table);
}

void test_same_entry_twice() {
  HashTable *table = create_hash_table(8192);

  uint8_t symbol[] = {'A', 'B'};

  add_symbol_hash(table, symbol, 2, 0, 0);
  add_symbol_hash(table, symbol, 2, 0, 0);

  assert(table->size == 1);

  uint32_t hash_index = hash_function(symbol, 2, 8192);

  HashTableEntry *entry = table->buckets[hash_index];

  assert(entry->frequency == 2);

  free_hash_table(table);
}

void test_different_symbols_2b() {
  HashTable *table = create_hash_table(8192);

  uint8_t symbolAB[] = {'A', 'B'};
  uint8_t symbolCD[] = {'C', 'D'};
  uint8_t symbolEF[] = {'E', 'F'};
  uint8_t symbolGH[] = {'G', 'H'};

  add_symbol_hash(table, symbolAB, 2, 0, 0);
  add_symbol_hash(table, symbolCD, 2, 0, 0);
  add_symbol_hash(table, symbolEF, 2, 0, 0);
  add_symbol_hash(table, symbolCD, 2, 0, 0);
  add_symbol_hash(table, symbolGH, 2, 0, 0);

  assert(table->size == 4);

  uint32_t index_ab = hash_function(symbolAB, 2, 8192);
  uint32_t index_ef = hash_function(symbolEF, 2, 8192);
  uint32_t index_gh = hash_function(symbolGH, 2, 8192);
  uint32_t index_cd = hash_function(symbolCD, 2, 8192);

  assert(table->buckets[index_ab]->frequency == 1);
  assert(table->buckets[index_ef]->frequency == 1);
  assert(table->buckets[index_gh]->frequency == 1);
  assert(table->buckets[index_cd]->frequency == 2);

  free_hash_table(table);
}

void test_different_symbols_3b() {
  HashTable *table = create_hash_table(8192);

  uint8_t symbolABC[] = {'A', 'B', 'C'};
  uint8_t symbolACB[] = {'A', 'C', 'B'};
  uint8_t symbolBAC[] = {'B', 'A', 'C'};
  uint8_t symbolBCA[] = {'B', 'C', 'A'};

  add_symbol_hash(table, symbolABC, 3, 0, 0);
  add_symbol_hash(table, symbolACB, 3, 0, 0);
  add_symbol_hash(table, symbolBAC, 3, 0, 0);
  add_symbol_hash(table, symbolACB, 3, 0, 0);
  add_symbol_hash(table, symbolBCA, 3, 0, 0);

  assert(table->size == 4);

  uint32_t index_abc = hash_function(symbolABC, 3, 8192);
  uint32_t index_bac = hash_function(symbolBAC, 3, 8192);
  uint32_t index_bca = hash_function(symbolBCA, 3, 8192);
  uint32_t index_acb = hash_function(symbolACB, 3, 8192);

  assert(table->buckets[index_abc]->frequency == 1);
  assert(table->buckets[index_bac]->frequency == 1);
  assert(table->buckets[index_bca]->frequency == 1);
  assert(table->buckets[index_acb]->frequency == 2);

  free_hash_table(table);
}

void test_different_symbols_4b() {
  HashTable *table = create_hash_table(16384);

  uint8_t symbolAB[] = {'A', 'B', 'A', 'B'};
  uint8_t symbolCD[] = {'C', 'D', 'C', 'D'};
  uint8_t symbolEF[] = {'E', 'F', 'E', 'F'};

  add_symbol_hash(table, symbolAB, 4, 0, 0);
  add_symbol_hash(table, symbolCD, 4, 0, 0);
  add_symbol_hash(table, symbolEF, 4, 0, 0);
  add_symbol_hash(table, symbolAB, 4, 0, 0);

  assert(table->size == 3);

  uint32_t index_ab = hash_function(symbolAB, 4, 16384);
  uint32_t index_cd = hash_function(symbolCD, 4, 16384);
  uint32_t index_ef = hash_function(symbolEF, 4, 16384);

  assert(table->buckets[index_ab]->frequency == 2);
  assert(table->buckets[index_cd]->frequency == 1);
  assert(table->buckets[index_ef]->frequency == 1);

  free_hash_table(table);
}

void test_codes() {
  HashTable *table = create_hash_table(8192);

  uint8_t symbolAB[] = {'A', 'B'};
  uint8_t symbolCD[] = {'C', 'D'};
  uint8_t symbolEF[] = {'E', 'F'};

  add_symbol_hash(table, symbolAB, 2, 1, 2);
  add_symbol_hash(table, symbolCD, 2, 2, 2);
  add_symbol_hash(table, symbolEF, 2, 3, 2);

  uint32_t index_ab = hash_function(symbolAB, 2, 8192);
  uint32_t index_cd = hash_function(symbolCD, 2, 8192);
  uint32_t index_ef = hash_function(symbolEF, 2, 8192);

  assert(table->buckets[index_ab]->code == 1);
  assert(table->buckets[index_cd]->code == 2);
  assert(table->buckets[index_ef]->code == 3);

  assert(table->buckets[index_ab]->code_length == 2);
  assert(table->buckets[index_cd]->code_length == 2);
  assert(table->buckets[index_ef]->code_length == 2);

  free_hash_table(table);
}

int main() {

  printf("Running tests: hash table.\n\n");

  test_empty_table();
  printf("Test 1: Empty table - PASS\n");

  test_one_entry();
  printf("Test 2: One entry - PASS\n");

  test_same_entry_twice();
  printf("Test 3: Same entry twice - PASS\n");

  test_different_symbols_2b();
  printf("Test 4: Different symbols (symbol size = 2) - PASS\n");

  test_different_symbols_3b();
  printf("Test 5: Different symbols (symbol size = 3) - PASS\n");

  test_different_symbols_4b();
  printf("Test 6: Different symbols (symbol size = 4) - PASS\n");

  test_codes();
  printf("Test 7: Storing codes and code lengths - PASS\n\n");

  printf("Hash table: all tests passed.\n\n");

  return 0;
}
