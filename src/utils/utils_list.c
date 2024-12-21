#include "utils_list.h"
#include <stdlib.h>

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
int list_add(list *l, void *ptr, int pos)
{
    if (pos >= l->len)
    {
        return 0;
    }

    list_node *node = list_node_create(ptr);
    list_node *curr = l->head;

    if (pos == 0 || l->len == 0)
    {
        node->next = l->head;
        l->head = node;
    }
    else if (pos == -1)
    {
        // insert at end
        while (curr->next != NULL)
        {
            curr = curr->next;
        }
        curr->next = node;
    }
    else
    {
        int idx = 0;
        list_node *prev = NULL;
        while (idx < pos)
        {
            prev = curr;
            curr = curr->next;
            idx++;
        }
        prev->next = node;
        node->next = curr;
    }

    // increment the length
    l->len++;

    return 1;
}

// removes node at index pos from list, returns 1 if successful, 0 otherwise.
// if pos = -1, removes from end.
int list_remove(list *l, int pos)
{
    if (pos >= l->len)
    {
        return 0;
    }

    list_node *curr = l->head;

    if (pos == 0)
    {
        l->head = l->head->next;
    }
    else if (pos == -1)
    {
        // remove at end
        while (curr->next->next != NULL)
        {
            curr = curr->next;
        }
        curr->next = NULL;
    }
    else
    {
        int idx = 0;
        list_node *prev = NULL;
        while (idx < pos)
        {
            prev = curr;
            curr = curr->next;
            idx++;
        }
        prev->next = curr->next;
    }

    // decrement the length
    l->len--;

    return 1;
}

// return ptr at index pos
void *list_get(list *l, int pos)
{
    if (pos < 0 || pos >= l->len)
    {
        return NULL;
    }

    list_node *curr = l->head;
    int idx = 0;
    while (idx < pos)
    {
        curr = curr->next;
        idx++;
    }

    return curr->val;
}

// concat l2 to the end of l1. l1 modified in-place.
void list_concat(list *l1, list *l2)
{
    list_node *curr = l1->head;

    if (curr == NULL) {
        l1->head = l2->head;
        l1->len += l2->len;
        return;
    }

    while (curr->next != NULL)
    {
        curr = curr->next;
    }

    curr->next = l2->head;
    l1->len += l2->len;
}