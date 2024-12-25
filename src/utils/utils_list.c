#include "utils_list.h"
#include <stdlib.h>
#include <stdio.h>

int HITS;
int ACCESSES;

// inits a list and returns a ptr to it.
list *list_init()
{
    return (list *)malloc(sizeof(list));
}

list_node *list_node_create(void *ptr)
{
    list_node *ln = (list_node *)malloc(sizeof(list_node));
    ln->val = ptr;
    ln->next = NULL;

    return ln;
}

// adds ptr to the list at idx pos. if pos = -1, adds at end.
// INSERTS BEFORE THE CURRENT ITEM AT THAT INDEX.
// return 1 if successful 0 otherwise
// amortized O(1)
int list_add(list *l, void *ptr, int pos)
{
    
    if (pos >= l->len)
    {
        return 0;
    }

    list_node *node = list_node_create(ptr);
    list_node *curr;

    if (pos == 0 || l->len == 0)
    {
        // insert at front
        node->next = l->head;
        l->head = node;
        l->last_idx = 0;
        l->last = l->head;
        l->len++;
        return 1;
    }
    // insert somewhere else

    pos = (pos == -1) ? l->len : pos; // -1 is the last node.
    // if we can use the last idx node, use that. else switch back to head
    int idx;
    if (l->last_idx < pos) {
        curr = l->last;
        idx = l->last_idx;
        
    } else {
        curr = l->head;
        idx = 0;
    }
    list_node *prev = NULL;
    while (idx < pos)
    {
        prev = curr;
        curr = curr->next;
        idx++;
    }

    prev->next = node;
    node->next = curr;

    // update last used
    l->last_idx = idx;
    l->last = node;

    // increment the length
    l->len++;

    return 1;
}

// removes node at index pos from list, returns 1 if successful, 0 otherwise.
// if pos = -1, removes from end.
// amortized O(1)
int list_remove(list *l, int pos)
{
    
    if (pos >= l->len)
    {
        return 0;
    }

    if (pos == 0)
    {
        l->head = l->head->next;
        l->last_idx = 0;
        l->last = l->head;
        l->len--;
        return 1;
    }

    pos = (pos == -1) ? l->len - 1 : pos; // -1 is the last node.
    // if we can use the last idx node, use that. else switch back to head
    list_node* curr;
    int idx;
    if (l->last_idx < pos) {
        
        curr = l->last;
        idx = l->last_idx;
    } else {
        curr = l->head;
        idx = 0;
    }
    list_node* prev = NULL;
    while (idx < pos)
    {
        prev = curr;
        curr = curr->next;
        idx++;
    }
    prev->next = curr->next;

    // update last used to prev.
    l->last_idx = idx - 1;
    l->last = prev;

    // decrement the length
    l->len--;

    return 1;
}

// return ptr at index pos
// amortized O(1)
void* list_get(list *l, int pos)
{
    
    if (pos < 0 || pos >= l->len)
    {
        return NULL;
    }

    // if we can use the last idx node, use that. else switch back to head
    list_node* curr;
    int idx;
    if (l->last_idx <= pos) {
        
        curr = l->last;
        idx = l->last_idx;
    } else {
        curr = l->head;
        idx = 0;
    }
    while (idx < pos)
    {
        curr = curr->next;
        idx++;
    }

    // update last used
    l->last = curr;
    l->last_idx = idx;
    
    return curr->val;
}

// concat l2 to the end of l1. l1 modified in-place.
// O(1)
void list_concat(list *l1, list *l2)
{
    list_node *curr = l1->head;

    if (curr == NULL) {
        l1->head = l2->head;
        l1->len += l2->len;
        l1->last_idx = 0;
        l1->last = l1->head;
        return;
    }

    while (curr->next != NULL)
    {
        curr = curr->next;
    }

    l1->last_idx = 0;
    l1->last = l1->head;
    curr->next = l2->head;
    l1->len += l2->len;
}