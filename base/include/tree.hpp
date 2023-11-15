#pragma once
#include <libc.hpp>

typedef struct tree_node
{
    void *data;
    char name[768];
    uint32_t nodes[4096]; // tree_node nodes[4096] - recursion error or output gonna be big, tree_node *nodes[4096] - pointer to list
    tree_node_t *prev;
    uint16_t node_count;
} tree_node_t;

#define E_OK 0
#define E_NAME_TOO_LONG 1
#define E_OTHER 2

static tree_node_t *tree_gen(const char *name, void *data) {
    if (strlen(name) > 767) {
        return (tree_node_t *)E_NAME_TOO_LONG;
    }
    tree_node_t *out = new tree_node_t;
    memset(out, 0, sizeof(tree_node_t));
    strcpy((char *)(&out->name), name);
    out->data = data;
}