// implementation of mapping from user-defined vars -> temp vars.
// very similar to stackmap.

#include "utils_list.h"

#pragma once

typedef struct {
    char* key; //user-defined
    char* value; // temp
} variablemap_node;

typedef list(variablemap_node*) variablemap;

variablemap* variablemap_init();
char* variablemap_get(variablemap* vm, char* key);
void variablemap_add(variablemap* vm, char* key, char* value);