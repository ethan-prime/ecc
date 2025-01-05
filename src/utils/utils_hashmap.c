#include "utils_hashmap.h"
#include <stdlib.h>

int char_sum(char* key) {
    int total = 0;
    int len = (int)strlen(key);
    for (int i = 0; i < len; i++) {
        total += (int)key[i];
    }
    return total;
}
#define DEFAULT_HASH &char_sum

hashmap* hashmap_init(int n_buckets, int (*hash_function)(char*)) {
    hashmap* map = (hashmap *)malloc(sizeof(hashmap));

    if (hash_function == NULL) {
        map->hash_function = DEFAULT_HASH;
    }

    map->n_buckets = n_buckets;
    map->buckets = (list**)malloc(sizeof(list*)*n_buckets);

    for (int i = 0; i < n_buckets; i++) {
        map->buckets[i] = list_init(); // initialize every list in the hashtable
    }

    return map;
}

hashmap_node* create_hashmap_node(char* key, void* value) {
    hashmap_node* node = (hashmap_node *)malloc(sizeof(hashmap_node));
    node->key = key;
    node->value = value;
    return node;
}

void* hashmap_get(hashmap* map, char* key) {
    int bucket = map->hash_function(key) % map->n_buckets;
    list(hashmap_node*)* l = map->buckets[bucket];
    for (int i = 0; i < l->len; i++) {
        hashmap_node* node = (hashmap_node *)list_get(l, i);
        if (strcmp(node->key, key) == 0) {
            return node->value;
        }
    }
    return NULL;
}

void hashmap_add(hashmap* map, char* key, void* value) {
    int bucket = map->hash_function(key) % map->n_buckets;
    list(hashmap_node*)* l = map->buckets[bucket];
    // if its already in the hashmap, update it.
    for (int i = 0; i < l->len; i++) {
        hashmap_node* node = (hashmap_node *)list_get(l, i);
        if (strcmp(node->key, key) == 0) {
            node->value = value;
            return;
        }
    }
    // else, just add it.
    hashmap_node* node = create_hashmap_node(key, value);
    list_append(l, (void*)node);
}

void hashmap_remove(hashmap* map, char* key) {
    int bucket = map->hash_function(key) % map->n_buckets;
    list(hashmap_node*)* l = map->buckets[bucket];
    for (int i = 0; i < l->len; i++) {
        hashmap_node* node = (hashmap_node *)list_get(l, i);
        if (strcmp(node->key, key) == 0) {
            // remove the key if it exists in the hashmap, else do nothing.
            list_remove(l, i);
            return;
        }
    }
}