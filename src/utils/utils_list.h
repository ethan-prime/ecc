#pragma once

typedef struct list_node {
    void* val;
    struct list_node* next;
} list_node;

typedef struct list {
    list_node* head;
    list_node* last; // keep track of last used node for faster indexing
    int last_idx; // keep track of last idx used for faster indexing
    int len;
} list;

#define list(t) list

// inits a list and returns a ptr to it.
list* list_init();

// adds ptr to the list at idx pos. if pos = -1, adds at end.
// return 1 if successful 0 otherwise
int list_add(list* l, void* ptr, int pos);
#define list_append(l, ptr) list_add((l), (ptr), -1)

// removes node at index pos from list, returns 1 if successful, 0 otherwise.
// if pos = -1, removes from end.
int list_remove(list* l, int pos);
#define list_pop(l, ptr) list_remove((l), (ptr), -1)

// return ptr at index l
void* list_get(list* l, int pos);

// concat l2 to the end of l1. l1 modified in-place.
void list_concat(list* l1, list* l2);