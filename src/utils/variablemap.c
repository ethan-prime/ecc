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

// 1 if successfully edited. 0 otherwise.
int variablemap_edit(variablemap* vm, char* key, char* new_value) {
    variablemap_node* node;
    for(int i = 0; i < vm->len; i++) {
        node = (variablemap_node*)list_get(vm, i);
        if (strcmp(key, node->key) == 0) {
            node->value = new_value;
            node->from_current_block = 1;
        }
    }
    return 0;
}

void variablemap_add(variablemap* vm, char* key, char* value) {
    if (variablemap_edit(vm, key, value)) {
        return;
    }

    variablemap_node* node = (variablemap_node*)malloc(sizeof(variablemap_node));
    node->key = key;
    node->value = value;
    node->from_current_block = 1;

    list_append(vm, (void*)node);
}

void variablemap_print(variablemap* vm) {
    for (int i = 0; i < vm->len; i++) {
        variablemap_node* node = (variablemap_node*)list_get(vm, i);
        printf("%s -> %s\n", node->key, node->value);    
    }
}

variablemap* variablemap_copy(variablemap* vm) {
    variablemap* new_vm = variablemap_init();

    for (int i = 0; i < vm->len; i++) {
        variablemap_node* node = (variablemap_node*)list_get(vm, i);
        variablemap_node* new_node = (variablemap_node*)malloc(sizeof(variablemap_node));
        new_node->key = node->key;
        new_node->value = node->value;
        new_node->from_current_block = 0; // this is inherited
        list_append(new_vm, (void*)new_node);
    }

    return new_vm;
}

int from_current_block(variablemap* vm, char* key) {
    for(int i = 0; i < vm->len; i++) {
        variablemap_node* node = (variablemap_node*)list_get(vm, i);
        if (strcmp(key, node->key) == 0) {
            return node->from_current_block;
        }
    }
    return 0;
}