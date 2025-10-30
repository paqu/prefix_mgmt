#include "test_utils.h"
#include <string.h>

void push(NodeStack *s, radix_node_t *n) {
    if (s->top < MAX_PATH_LEN)
        s->stack[s->top++] = n;
}

void pop(NodeStack *s) {
    if (s->top > 0)
        s->top--;
}

void traverse(radix_node_t *node, unsigned int prefix_bits, int shift,
              NodeStack *stack, PathEntry *paths, int *path_count) {
    if (!node)
        return;

    push(stack, node);

    unsigned int full_bits = (prefix_bits << node->skip) | node->prefix;
    int total_len = shift + node->skip;

    if (node->is_prefix) {
        if (*path_count < MAX_PATHS) {
            PathEntry *p = &paths[*path_count];
            p->full_prefix = full_bits;
            p->mask = node->mask;
            p->path_len = stack->top;
            memcpy(p->path, stack->stack, sizeof(radix_node_t *) * stack->top);
            (*path_count)++;
        }
    }

    traverse(node->left, full_bits, total_len, stack, paths, path_count);
    traverse(node->right, full_bits, total_len, stack, paths, path_count);

    pop(stack);
}

PathEntry *find_path_by_prefix(PathEntry *paths, int path_count,
                               unsigned int full_prefix) {
    if (!paths || path_count <= 0) {
        return NULL;
    }

    for (int i = 0; i < path_count; i++) {
        if (paths[i].full_prefix == full_prefix) {
            return &paths[i];
        }
    }

    return NULL;
}
