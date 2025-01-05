#include "symboltable.h"
#include <stdlib.h>
#include <stdio.h>

symboltable* symboltable_init() {
    symboltable* table = hashmap_init(100, NULL);
    return table;
}

type_specifier* create_type_specifier(type_t type, int param_count) {
    type_specifier* spec = (type_specifier *)malloc(sizeof(type_specifier));
    spec->type = type;
    spec->param_count = param_count; // this is irrelevant for TYPE_INT, we set it anyways.
    return spec;
}

symboltable_node* create_symboltable_node(type_specifier* type_spec, int defined) {
    symboltable_node* node = (symboltable_node *)malloc(sizeof(symboltable_node));
    node->type_spec = type_spec;
    node->defined = defined;
    return node;
}

void symboltable_add(symboltable* table, char* name, type_t type, int param_count, int defined) {
    type_specifier* spec = create_type_specifier(type, param_count);
    symboltable_node* node = create_symboltable_node(spec, defined);
    hashmap_add(table, name, node);
}

symboltable_node* symboltable_get(symboltable* table, char* name) {
    return (symboltable_node *)hashmap_get(table, name);
}

void symboltable_print(symboltable* table) {
    printf("%-20s %-10s %-10s %-10s\n", "--------------------", "----------", "----------", "----------");
    printf("%-20s %-10s %-10s %-10s\n", "NAME", "TYPE", "PARAMS", "DEF?");
    printf("%-20s %-10s %-10s %-10s\n", "--------------------", "----------", "----------", "----------");
    for (int i = 0; i < table->n_buckets; i++) {
        for (int j = 0; j < table->buckets[i]->len; j++) {
            hashmap_node* entry = (hashmap_node*)list_get(table->buckets[i], j);
            symboltable_node* symbol = (symboltable_node*)entry->value;
            char* type_ = (symbol->type_spec->type == TYPE_INT) ? "int" : "func";
            printf("%-20s %-10s %-10d %-10d\n", entry->key, type_, symbol->type_spec->param_count, symbol->defined);
        }
    }
    printf("%-20s %-10s %-10s %-10s\n", "--------------------", "----------", "----------", "----------");
}