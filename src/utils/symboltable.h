// symbol table implementation
// uses hashmap

#include "../utils/utils_hashmap.h"

#pragma once

typedef enum {
    TYPE_INT,
    TYPE_FUNC,
} type_t;

typedef struct {
    type_t type;
    int param_count; // only used for functions
} type_specifier;

typedef hashmap symboltable;

typedef struct {
    type_specifier* type;
    int defined; // 0 or 1.
} symboltable_node;

symboltable* symboltable_init();
void symboltable_add(symboltable* table, char* name, type_t type, int param_count, int defined);
#define NO_PARAMS 0
symboltable_node* symboltable_get(symboltable* table, char* name);