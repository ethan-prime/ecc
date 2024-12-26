#include "variablemap.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

variablemap* variablemap_init() {
    return (variablemap *)malloc(sizeof(variablemap));
}

char* variablemap_get(variablemap* vm, char* key) {
    variablemap_node* node;
    for(int i = 0; i < vm->len; i++) {
        node = (variablemap_node*)list_get(vm, i);
        if (strcmp(key, node->key) == 0) {
            return node->value;
        }
    }
    return NULL;
}

void variablemap_add(variablemap* vm, char* key, char* value) {
    variablemap_node* node = (variablemap_node*)malloc(sizeof(variablemap_node));
    node->key = key;
    node->value = value;

    list_append(vm, (void*)node);
}

void variablemap_print(variablemap* vm) {
    for (int i = 0; i < vm->len; i++) {
        variablemap_node* node = (variablemap_node*)list_get(vm, i);
        printf("%s -> %s\n", node->key, node->value);    
    }
}