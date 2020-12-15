#ifndef HASH_MAP_INT_H
#define HASH_MAP_INT_H
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

struct sHashMapBucket_int
{
    size_t key;
    void* value;
    struct sHashMapBucket_int* next;
};

typedef struct sHashMapBucket_int HashMapBucket_int;

typedef struct
{
    uint32_t size;
    uint32_t capacity; 
    size_t max_depth;
    HashMapBucket_int** buckets;
} HashMap_int;

typedef struct 
{
    const HashMap_int* map;
    size_t bucketIndex;
    HashMapBucket_int* bucket;
} HashMapIterator_int;

HashMap_int* hash_map_init_int(uint32_t capacity);
void hash_map_insert_int(HashMap_int* map, size_t key, void* value);
HashMapIterator_int* hash_map_find_int(const HashMap_int* map, size_t key);
bool hash_map_has_key_int(const HashMap_int* map, size_t key);
void hash_map_update_value_int(HashMap_int* map, size_t key, void* value);
void hash_map_free_int(HashMap_int* map, void(*free_callback)(void*));

HashMapIterator_int* hash_map_iterator_begin_int(const HashMap_int* map);
void hash_map_iterator_next_int(HashMapIterator_int* iter);
bool hash_map_iterator_is_end_int(const HashMapIterator_int* iter);
size_t hash_map_iterator_get_key_int(const HashMapIterator_int* iter);
void* hash_map_iterator_get_value_int(const HashMapIterator_int* iter);

#endif
