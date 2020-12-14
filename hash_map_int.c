#include "hash_map_int.h"
//#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "util.h"
#include <stdio.h>

size_t hash_map_int_hash(size_t x)
{
    // https://stackoverflow.com/a/12996028
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
}


HashMap_int* hash_map_init_int(uint32_t capacity)
{
    HashMap_int* map = calloc(1, sizeof(HashMap_int));
    map->size = 0;
    map->capacity = capacity;
    map->buckets = calloc(capacity, sizeof(HashMapBucket_int*));
    return map;
}


void hash_map_free_bucket_int(HashMapBucket_int* bucket)
{
    if (bucket->next) hash_map_free_bucket_int(bucket->next);
    free(bucket);
}

void hash_map_free_int(HashMap_int* map)
{
    for (int i = 0; i < map->capacity; ++i)
    {
        HashMapBucket_int* bucket = map->buckets[i];
        if (bucket) hash_map_free_bucket_int(bucket);
    } 

    free(map->buckets);
    free(map);
}

size_t hash_map_compute_index_int(const HashMap_int* map, size_t key)
{
    const size_t hash = hash_map_int_hash(key);
    return hash % map->capacity;
}


void hash_map_insert_int(HashMap_int* map, size_t key, void* value)
{
    const size_t idx = hash_map_compute_index_int(map, key);
    HashMapBucket_int* newBucket = calloc(1, sizeof(HashMapBucket_int));
    newBucket->key = key;
    newBucket->value = value;

    HashMapBucket_int** slot = &map->buckets[idx];
    
    size_t depth = 0;

    while (*slot)
    {
        slot = &((*slot)->next);
        depth++;
    }
    
    *slot = newBucket;
    map->size++;
    map->max_depth = MAX(map->max_depth, depth);
}


const HashMapBucket_int* hash_map_find_int(const HashMap_int* map, size_t key)
{
    const size_t idx = hash_map_compute_index_int(map, key);
    HashMapBucket_int* bucket = map->buckets[idx];

    while (bucket && bucket->key != key)
    {
        bucket = bucket->next;
    }

    return bucket;
}


bool hash_map_has_key_int(const HashMap_int* map, size_t key)
{
    return hash_map_find_int(map, key);
}


void hash_map_update_value_int(HashMap_int* map, size_t key, void* value)
{
    HashMapBucket_int* bucket = (HashMapBucket_int*) hash_map_find_int(map, key);
    assert(bucket);
    bucket->value = value;
}


size_t hash_map_next_occupied_bucket_index_int(const HashMap_int* map, size_t startIndex)
{
    for (size_t i = startIndex; i < map->capacity; ++i)
    {
        if (map->buckets[i])
        {
            return i;
        }
    }

    return 0xffffffffffffffff;
}


HashMapIterator_int* hash_map_iterator_begin_int(const HashMap_int* map)
{
    HashMapIterator_int* iter = calloc(1, sizeof(HashMapIterator_int));
    iter->map = map;
    size_t firstOccupiedBucketIndex = hash_map_next_occupied_bucket_index_int(map, 0);

    if (firstOccupiedBucketIndex < map->capacity)
    {
        iter->bucketIndex = firstOccupiedBucketIndex;
        iter->bucket = map->buckets[firstOccupiedBucketIndex];
    }

    return iter;
}


void hash_map_iterator_set_end_int(const HashMap_int* map, HashMapIterator_int* iter)
{
    iter->map = map;
    iter->bucketIndex = 0;
    iter->bucket = 0;
}


void hash_map_iterator_next_int(HashMapIterator_int* iter)
{

    if (hash_map_iterator_is_end_int(iter))
    {
        return;
    }

    if (iter->bucket->next)
    {
        iter->bucket = iter->bucket->next;
    }
    else
    {
        size_t nextOccupiedBucketIndex =
            hash_map_next_occupied_bucket_index_int(iter->map, iter->bucketIndex + 1);

        if (nextOccupiedBucketIndex < iter->map->capacity)
        {
            iter->bucketIndex = nextOccupiedBucketIndex;
            iter->bucket = iter->map->buckets[nextOccupiedBucketIndex];
        } 
        else
        {
            hash_map_iterator_set_end_int(iter->map, iter);
            return;
        }
    }
}


bool hash_map_iterator_is_end_int(const HashMapIterator_int* iter)
{
    return iter->bucket == NULL;
}


size_t hash_map_iterator_get_key_int(const HashMapIterator_int* iter)
{
    if (iter->bucket == NULL)
    {
        return SIZE_MAX;
    }

    return iter->bucket->key;
}


void* hash_map_iterator_get_value_int(const HashMapIterator_int* iter)
{
    if (iter->bucket == NULL)
    {
        return NULL;
    }

    return iter->bucket->value;
}

