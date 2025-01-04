// implementation of mapping from user-defined vars -> temp vars.
// very similar to stackmap.

#include "utils_list.h"

#pragma once

typedef struct
{
    char *key;              // user-defined
    char *value;            // temp
    int from_current_scope; // whether this variable was declared in the current scope or not.
    int has_linkage;        // whether this variable has external linkage 
} identifiermap_node;

typedef list(identifiermap_node *) identifiermap;

identifiermap *identifiermap_init();
identifiermap_node* identifiermap_get(identifiermap *im, char *key);
void identifiermap_add(identifiermap *im, char *key, char *value, int has_linkage);
#define HAS_LINKAGE 1
#define NO_LINKAGE 0
void identifiermap_print(identifiermap *im);
identifiermap *identifiermap_copy(identifiermap *im);
int from_current_scope(identifiermap *im, char *key);