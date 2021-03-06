#include "hash_map.h"
#include <string.h>
#include <stdlib.h>

uint32_t sdbm(const char* str)
{
    uint32_t hash = 0;
    int c;

    while (true == (c = *str++))
    {
        hash = c + (hash << 6) + (hash << 16) - hash;
    }

    return hash;
}


HashMap* hash_map_init(uint32_t capacity)
{
    HashMap* map = calloc(1, sizeof(HashMap));
    map->size = 0;
    map->capacity = capacity;
    map->buckets = calloc(capacity, sizeof(HashMapBucket*));
    return map;
}

void hash_map_free_bucket(HashMapBucket* bucket, void(*free_callback)(void*))
{
    if (bucket->next) hash_map_free_bucket(bucket->next, free_callback);

    if (free_callback)
    {
        free_callback(bucket->value);
    }

    free(bucket);
}

void hash_map_free(HashMap* map, void(*free_callback)(void*))
{
    for (size_t i = 0; i < map->capacity; ++i)
    {
        HashMapBucket* bucket = map->buckets[i];
        if (bucket) hash_map_free_bucket(bucket, free_callback);
    } 

    free(map->buckets);
    free(map);
}

void hash_map_insert(HashMap* map, const char* key, void* value)
{
    const uint32_t hash = sdbm(key) % map->capacity;
    HashMapBucket* newBucket = calloc(1, sizeof(HashMapBucket));
    newBucket->key = malloc(strlen(key) + 1);
    strcpy(newBucket->key, key);
    newBucket->value = value;

    HashMapBucket** slot = &map->buckets[hash];

    while (*slot)
    {
        slot = &((*slot)->next);
    }

    *slot = newBucket;
    map->size++;
}


HashMapIterator* hash_map_find(const HashMap* map, const char* key)
{
    HashMapIterator* iter = calloc(1, sizeof(HashMapIterator));
    iter->map = map;

    const uint32_t hash = sdbm(key); 
    const uint32_t index = hash % map->capacity;
    HashMapBucket* bucket = map->buckets[hash];

    while (bucket && strcmp(bucket->key, key))
    {
        bucket = bucket->next;
    }
    
    if (bucket)
    {
        iter->bucket = bucket;
        iter->bucketIndex = index;
    }

    return iter;
}


bool hash_map_has_key(const HashMap* map, const char* key)
{
    HashMapIterator* iter = hash_map_find(map, key);
    bool result = !hash_map_iterator_is_end(iter);
    free(iter);
    return result;
}


size_t hash_map_next_occupied_bucket_index(const HashMap* map, size_t startIndex)
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


HashMapIterator* hash_map_iterator_begin(const HashMap* map)
{
    HashMapIterator* iter = calloc(1, sizeof(HashMapIterator));
    iter->map = map;
    size_t firstOccupiedBucketIndex = hash_map_next_occupied_bucket_index(map, 0);

    if (firstOccupiedBucketIndex < map->capacity)
    {
        iter->bucketIndex = firstOccupiedBucketIndex;
        iter->bucket = map->buckets[firstOccupiedBucketIndex];
    }

    return iter;
}


void hash_map_iterator_set_end(const HashMap* map, HashMapIterator* iter)
{
    iter->map = map;
    iter->bucketIndex = 0;
    iter->bucket = 0;
}


void hash_map_iterator_next(HashMapIterator* iter)
{

    if (hash_map_iterator_is_end(iter))
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
            hash_map_next_occupied_bucket_index(iter->map, iter->bucketIndex + 1);

        if (nextOccupiedBucketIndex < iter->map->capacity)
        {
            iter->bucketIndex = nextOccupiedBucketIndex;
            iter->bucket = iter->map->buckets[nextOccupiedBucketIndex];
        } 
        else
        {
            hash_map_iterator_set_end(iter->map, iter);
            return;
        }
    }
}


bool hash_map_iterator_is_end(const HashMapIterator* iter)
{
    return iter->bucket == NULL;
}


char* hash_map_iterator_get_key(const HashMapIterator* iter)
{
    if (iter->bucket == NULL)
    {
        return NULL;
    }

    return iter->bucket->key;
}


void* hash_map_iterator_get_value(const HashMapIterator* iter)
{
    if (iter->bucket == NULL)
    {
        return NULL;
    }

    return iter->bucket->value;
}


strarr_t* hash_map_get_keys(HashMap* map)
{
    strarr_t* result = strarr_init_with_size(map->size);
    HashMapIterator* iter = hash_map_iterator_begin(map);
    
    while (!hash_map_iterator_is_end(iter))
    {
        strarr_push(result, hash_map_iterator_get_key(iter));
        hash_map_iterator_next(iter);
    }

    free(iter);
    return result;
}

void* hash_map_get_value(HashMap* map, const char* key)
{
    HashMapIterator* iter = hash_map_find(map, key);
    void* result = NULL;
    if (!hash_map_iterator_is_end(iter))
    {
        result = hash_map_iterator_get_value(iter);
    }

    free(iter);

    return result;
}

