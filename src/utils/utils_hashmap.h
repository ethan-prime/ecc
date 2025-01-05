#pragma once

#include "utils_list.h"
#include <string.h>

// an implementation of a hashmap

typedef struct {
    char* key;
    void* value; // data stored in hashmap
} hashmap_node;

typedef struct {
    int n_buckets;
    list(hashmap_node*)** buckets;  // array of lists of hashmap nodes
    int (*hash_function)(char*);    // hash function
} hashmap;

hashmap* hashmap_init(int n_buckets, int (*hash_function)(char*));
void* hashmap_get(hashmap* map, char* key);
void hashmap_add(hashmap* map, char* key, void* value);
void hashmap_remove(hashmap* map, char* key);