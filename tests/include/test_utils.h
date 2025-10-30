#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include "prefix_mgmt/prefix_mgmt.h"

#define MAX_PATH_LEN 33
#define MAX_PATHS 1024

typedef struct {
    radix_node_t *stack[MAX_PATH_LEN];
    int top;
} NodeStack;

typedef struct {
    unsigned int full_prefix;
    char mask;
    radix_node_t *path[MAX_PATH_LEN];
    int path_len;
} PathEntry;

void push(NodeStack *s, radix_node_t *n);
void pop(NodeStack *s);
void traverse(radix_node_t *node, unsigned int prefix_bits, int shift,
              NodeStack *stack, PathEntry *paths, int *path_count);
PathEntry *find_path_by_prefix(PathEntry *paths, int path_count,
                               unsigned int full_prefix);

#endif /* TEST_UTILS_H */
