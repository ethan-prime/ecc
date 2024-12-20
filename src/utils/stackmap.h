#include "utils_list.h"
// implementation of a mapping from IR pseudoregister identifier -> byte offset.

#pragma once

typedef struct {
    char* identifier;
    int size;
} stackmap_node;

typedef list(stackmap_node*) stackmap;

stackmap* stackmap_init();
int stackmap_getoffset(stackmap* sm, char* identifier);
void stackmap_add(stackmap* sm, char* identifier, int size);