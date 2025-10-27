#include "prefix_mgmt/prefix_mgmt.h"
#include <gtest/gtest.h>

#define MAX_PATH_LEN 33
#define MAX_PATHS 1024

typedef struct {
    radix_node_t *stack[MAX_PATH_LEN];
    int top;
} NodeStack;

typedef struct {
    uint64_t full_prefix;
    uint64_t mask;
    radix_node_t *path[MAX_PATH_LEN];
    int path_len;
} PathEntry;

void push(NodeStack *s, radix_node_t *n) {
    if (s->top < MAX_PATH_LEN)
        s->stack[s->top++] = n;
}

void pop(NodeStack *s) {
    if (s->top > 0)
        s->top--;
}

void traverse(radix_node_t *node, uint64_t prefix_bits, int shift,
              NodeStack *stack, PathEntry *paths, int *path_count) {
    if (!node)
        return;

    push(stack, node);

    uint64_t full_bits = (prefix_bits << node->skip) | node->prefix;
    int total_len = shift + node->skip;

    if (node->is_prefix) {

        // zapisujemy do tablicy ścieżek
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

class AddFunctionTest : public ::testing::Test {
  protected:
    void SetUp() override {}

    void TearDown() override {}
};

// TC-ADD-1: Valid Prefix Addition
TEST_F(AddFunctionTest, ValidPrefixAddition) {
    unsigned int base = 0x0A000000; // 10.0.0.0
    EXPECT_EQ(0, add(base, 16));
}

// TC-ADD-2: Invalid Mask - Below Range
TEST_F(AddFunctionTest, InvalidMaskBelowRange) {
    unsigned int base = 0x0A000000; // 10.0.0.0
    EXPECT_EQ(-1, add(base, -1));
}

// TC-ADD-3: Invalid Mask - Above Range
TEST_F(AddFunctionTest, InvalidMaskAboveRange) {
    unsigned int base = 0x0A000000; // 10.0.0.0
    EXPECT_EQ(-1, add(base, 33));
}

// TC-ADD-4: Invalid Base - Misaligned
TEST_F(AddFunctionTest, InvalidBaseMisaligned) {
    unsigned int base = 0x80000000;

    for (int i = 1; i < 32; i++) {
        base >>= 1;
        EXPECT_EQ(-1, add(base, i));
    }
}
