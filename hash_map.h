#ifndef HASH_MAP_H
#define HASH_MAP_H
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "strarr.h"

struct sHashMapBucket
{
    char* key;
    void* value;
    struct sHashMapBucket* next;
};

typedef struct sHashMapBucket HashMapBucket;

typedef struct
{
    uint32_t size;
    uint32_t capacity; 
    HashMapBucket** buckets;
} HashMap;

typedef struct 
{
    const HashMap* map;
    size_t bucketIndex;
    HashMapBucket* bucket;
} HashMapIterator;

HashMap* hash_map_init(uint32_t capacity);
void hash_map_free(HashMap* map, void(*free_callback)(void*));
void hash_map_insert(HashMap* map, const char* key, void* value);
HashMapIterator* hash_map_find(const HashMap* map, const char* key);
bool hash_map_has_key(const HashMap* map, const char* key);
void* hash_map_get_value(HashMap* map, const char* key);

HashMapIterator* hash_map_iterator_begin(const HashMap* map);
void hash_map_iterator_next(HashMapIterator* iter);
bool hash_map_iterator_is_end(const HashMapIterator* iter);
char* hash_map_iterator_get_key(const HashMapIterator* iter);
void* hash_map_iterator_get_value(const HashMapIterator* iter);

strarr_t* hash_map_get_keys(HashMap* map);

#endif
