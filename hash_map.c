#include "hash_map.h"
//#include <ctype.h>
#include <string.h>
#include <stdlib.h>

uint32_t sdbm(const char* str)
{
    uint32_t hash = 0;
    int c;

    while (c = *str++)
    {
        hash = c + (hash << 6) + (hash << 16) - hash;
    }

    return hash;
}


void hash_map_init(HashMap* map, uint32_t capacity)
{
    map->size = 0;
    map->capacity = capacity;
    map->buckets = calloc(capacity, sizeof(HashMapBucket*));
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


const HashMapBucket* hash_map_find(const HashMap* map, const char* key)
{
    const uint32_t hash = sdbm(key) % map->capacity; 
    const HashMapBucket* bucket = map->buckets[hash];

    while (bucket && strcmp(bucket->key, key))
    {
        bucket = bucket->next;
    }

    return bucket;
}


bool hash_map_has_key(const HashMap* map, const char* key)
{
    return hash_map_find(map, key);
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
}


void hash_map_iterator_begin(const HashMap* map, HashMapIterator* iter)
{
    memset(iter, 0, sizeof(HashMapIterator));
    iter->map = map;
    size_t firstOccupiedBucketIndex = hash_map_next_occupied_bucket_index(map, 0);
    if (firstOccupiedBucketIndex < map->capacity)
    {
        iter->bucketIndex = firstOccupiedBucketIndex;
        iter->bucket = map->buckets[firstOccupiedBucketIndex];
        iter->key = iter->bucket->key;
        iter->value = iter->bucket->value;
    }
}


void hash_map_iterator_set_end(const HashMap* map, HashMapIterator* iter)
{
    iter->map = map;
    iter->key = 0;
    iter->value = 0;
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

    iter->key = iter->bucket->key;
    iter->value = iter->bucket->value;
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

