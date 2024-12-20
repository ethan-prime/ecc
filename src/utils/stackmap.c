#include "stackmap.h"
#include <stdlib.h>
#include <string.h>

stackmap* stackmap_init() {
    return (stackmap *)malloc(sizeof(stackmap));
}

void stackmap_add(stackmap* sm, char* identifier, int size) {
    stackmap_node* node = (stackmap_node*)malloc(sizeof(stackmap_node));

    node->identifier = identifier;
    node->size = size;

    list_append(sm, (void*)node);
}

// returns offset from rbp in the stack. -1 if undefined.
int stackmap_getoffset(stackmap* sm, char* identifier) {
    int offset = 0;
    stackmap_node* node;
    for(int i = 0; i < sm->len; i++) {
        node = (stackmap_node*)list_get(sm, i);
        offset += node->size;
        if (strcmp(identifier, node->identifier) == 0) {
            return offset;
        }
    }
    return -1;
}

// returns size in bytes of the stackmap. (basically, how many bytes the function requires...)
int stackmap_size(stackmap* sm) {
    int offset = 0;
    stackmap_node* node;
    for(int i = 0; i < sm->len; i++) {
        node = (stackmap_node*)list_get(sm, i);
        offset += node->size;
    }
    return offset;
}