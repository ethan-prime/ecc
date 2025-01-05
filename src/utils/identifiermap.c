#include "identifiermap.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

identifiermap *identifiermap_init()
{
    return (identifiermap *)malloc(sizeof(identifiermap));
}

identifiermap_node* identifiermap_get(identifiermap *im, char *key)
{
    identifiermap_node *node;
    for (int i = 0; i < im->len; i++)
    {
        node = (identifiermap_node *)list_get(im, i);
        if (strcmp(key, node->key) == 0)
        {
            return node;
        }
    }
    return NULL;
}

// 1 if successfully edited. 0 otherwise.
int identifiermap_edit_name(identifiermap *im, char *key, char *new_value)
{
    identifiermap_node *node;
    for (int i = 0; i < im->len; i++)
    {
        node = (identifiermap_node *)list_get(im, i);
        if (strcmp(key, node->key) == 0)
        {
            node->value = new_value;
            node->from_current_scope = 1;
            return 1;
        }
    }
    return 0;
}

void identifiermap_add(identifiermap *im, char *key, char *value, int has_linkage)
{
    if (identifiermap_edit_name(im, key, value))
    {
        return;
    }

    identifiermap_node *node = (identifiermap_node *)malloc(sizeof(identifiermap_node));
    node->key = key;
    node->value = value;
    node->from_current_scope = 1;
    node->has_linkage = has_linkage;

    list_append(im, (void *)node);
}

void identifiermap_print(identifiermap *im)
{
    for (int i = 0; i < im->len; i++)
    {
        identifiermap_node *node = (identifiermap_node *)list_get(im, i);
        printf("%s -> %s\n", node->key, node->value);
    }
}

identifiermap *identifiermap_copy(identifiermap *im)
{
    identifiermap *new_im = identifiermap_init();

    for (int i = 0; i < im->len; i++)
    {
        identifiermap_node *node = (identifiermap_node *)list_get(im, i);
        identifiermap_node *new_node = (identifiermap_node *)malloc(sizeof(identifiermap_node));
        new_node->key = node->key;
        new_node->value = node->value;
        new_node->from_current_scope = 0; // this is inherited
        new_node->has_linkage = node->has_linkage;
        list_append(new_im, (void *)new_node);
    }

    return new_im;
}

int from_current_scope(identifiermap *im, char *key)
{
    for (int i = 0; i < im->len; i++)
    {
        identifiermap_node *node = (identifiermap_node *)list_get(im, i);
        if (strcmp(key, node->key) == 0)
        {
            return node->from_current_scope;
        }
    }
    return 0;
}