#include "prefix_mgmt/prefix_mgmt.h"
#include <gtest/gtest.h>
#include <string.h>

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
PathEntry *find_path_by_prefix(PathEntry *paths, int path_count,
                               uint64_t full_prefix) {
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

class AddFunctionTest : public ::testing::Test {
  protected:
    void SetUp() override { prefix_mgmt_init(); }

    void TearDown() override { prefix_mgmt_cleanup(); }
};

// TC-ADD-1: Valid Prefix Addition
TEST_F(AddFunctionTest, ValidPrefixAddition) {

    NodeStack stack = {.top = 0};
    PathEntry paths[MAX_PATHS];
    int path_count = 0;
    radix_node_t *root_ptr;
    unsigned int base;
    char mask;
    PathEntry *found;

    /* First address  */
    base = 0x0A000000; // 10.0.0.0
    mask = 16;
    EXPECT_EQ(0, add(base, mask));
    root_ptr = get_root_addr();
    EXPECT_TRUE(root_ptr != NULL);
    traverse(root_ptr, 0, 0, &stack, paths, &path_count);
    EXPECT_EQ(1, path_count);
    found = find_path_by_prefix(paths, path_count,
                                (base >> (32 - mask)) & 0xffffffff);

    EXPECT_TRUE(found != NULL);
    EXPECT_EQ(found->full_prefix, (base >> (32 - mask)) & 0xffffffff);
    EXPECT_EQ(found->mask, 16);

    /* Second address */
    path_count = 0;
    memset(paths, 0, MAX_PATHS * sizeof(PathEntry));
    base = 0xA0000000; // 160.0.0.0/8
    mask = 8;
    EXPECT_EQ(0, add(base, mask));
    root_ptr = get_root_addr();
    EXPECT_TRUE(root_ptr != NULL);
    traverse(root_ptr, 0, 0, &stack, paths, &path_count);
    EXPECT_EQ(2, path_count);
    found = find_path_by_prefix(paths, path_count,
                                (base >> (32 - mask)) & 0xffffffff);

    EXPECT_TRUE(found != NULL);
    EXPECT_EQ(found->full_prefix, (base >> (32 - mask)) & 0xffffffff);
    EXPECT_EQ(found->mask, 8);

    /* Third address - partial common prefix  */
    path_count = 0;
    memset(paths, 0, MAX_PATHS * sizeof(PathEntry));
    base = 0xA1000000; // 161.0.0.0/8
    mask = 8;
    EXPECT_EQ(0, add(base, mask));
    root_ptr = get_root_addr();
    EXPECT_TRUE(root_ptr != NULL);
    traverse(root_ptr, 0, 0, &stack, paths, &path_count);
    EXPECT_EQ(3, path_count);
    found = find_path_by_prefix(paths, path_count,
                                (base >> (32 - mask)) & 0xffffffff);

    EXPECT_TRUE(found != NULL);
    EXPECT_EQ(found->full_prefix, (base >> (32 - mask)) & 0xffffffff);
    EXPECT_EQ(found->mask, 8);

    /* 4th address - partial common prefix -- overlapping  */
    path_count = 0;
    memset(paths, 0, MAX_PATHS * sizeof(PathEntry));
    base = 0xA0000000; // 160.0.0.0/4
    mask = 4;
    EXPECT_EQ(0, add(base, mask));
    root_ptr = get_root_addr();
    EXPECT_TRUE(root_ptr != NULL);
    traverse(root_ptr, 0, 0, &stack, paths, &path_count);
    EXPECT_EQ(4, path_count);
    found = find_path_by_prefix(paths, path_count,
                                (base >> (32 - mask)) & 0xffffffff);

    EXPECT_TRUE(found != NULL);
    EXPECT_EQ(found->full_prefix, (base >> (32 - mask)) & 0xffffffff);
    EXPECT_EQ(found->mask, 4);
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
// TC-ADD-5: Valid prefix -- the same address different mask
TEST_F(AddFunctionTest, ValidPrefixAdditionDifferentMask) {

    NodeStack stack = {.top = 0};
    PathEntry paths[MAX_PATHS];
    int path_count = 0;
    radix_node_t *root_ptr;
    unsigned int base;
    char mask;
    PathEntry *found;

    base = 0x0A000000; // 10.0.0.0
    EXPECT_EQ(0, add(base, 16));
    EXPECT_EQ(0, add(base, 8));
    EXPECT_EQ(0, add(base, 24));
    root_ptr = get_root_addr();
    EXPECT_TRUE(root_ptr != NULL);
    traverse(root_ptr, 0, 0, &stack, paths, &path_count);
    EXPECT_EQ(3, path_count);

    mask = 16;
    found = find_path_by_prefix(paths, path_count,
                                (base >> (32 - mask)) & 0xffffffff);

    EXPECT_TRUE(found != NULL);
    EXPECT_EQ(found->full_prefix, (base >> (32 - mask)) & 0xffffffff);
    EXPECT_EQ(found->mask, 16);

    mask = 8;
    found = find_path_by_prefix(paths, path_count,
                                (base >> (32 - mask)) & 0xffffffff);

    EXPECT_TRUE(found != NULL);
    EXPECT_EQ(found->full_prefix, (base >> (32 - mask)) & 0xffffffff);
    EXPECT_EQ(found->mask, 8);

    mask = 24;
    found = find_path_by_prefix(paths, path_count,
                                (base >> (32 - mask)) & 0xffffffff);
    EXPECT_TRUE(found != NULL);
    EXPECT_EQ(found->full_prefix, (base >> (32 - mask)) & 0xffffffff);
    EXPECT_EQ(found->mask, 24);

    path_count = 0;
    memset(paths, 0, MAX_PATHS * sizeof(PathEntry));
    prefix_mgmt_cleanup();

    base = 0xFA000000;
    EXPECT_EQ(0, add(base, 24));
    EXPECT_EQ(0, add(base, 16));
    EXPECT_EQ(0, add(base, 8));
    root_ptr = get_root_addr();
    EXPECT_TRUE(root_ptr != NULL);
    traverse(root_ptr, 0, 0, &stack, paths, &path_count);
    EXPECT_EQ(3, path_count);

    mask = 24;
    found = find_path_by_prefix(paths, path_count,
                                (base >> (32 - mask)) & 0xffffffff);

    EXPECT_TRUE(found != NULL);
    EXPECT_EQ(found->full_prefix, (base >> (32 - mask)) & 0xffffffff);
    EXPECT_EQ(found->mask, 24);

    mask = 16;
    found = find_path_by_prefix(paths, path_count,
                                (base >> (32 - mask)) & 0xffffffff);

    EXPECT_TRUE(found != NULL);
    EXPECT_EQ(found->full_prefix, (base >> (32 - mask)) & 0xffffffff);
    EXPECT_EQ(found->mask, 16);

    mask = 8;
    found = find_path_by_prefix(paths, path_count,
                                (base >> (32 - mask)) & 0xffffffff);
    EXPECT_TRUE(found != NULL);
    EXPECT_EQ(found->full_prefix, (base >> (32 - mask)) & 0xffffffff);
    EXPECT_EQ(found->mask, 8);
}

// TC-ADD-6: Duplicate Prefix
TEST_F(AddFunctionTest, DuplicatePrefix) {
    NodeStack stack = {.top = 0};
    PathEntry paths[MAX_PATHS];
    int path_count = 0;
    radix_node_t *root_ptr;
    unsigned int base;
    char mask;
    PathEntry *found;

    base = 0x0A0B0000; // 10.0.0.0/8
    mask = 16;
    EXPECT_EQ(0, add(base, mask));
    EXPECT_EQ(0, add(base, mask)); // Second add should succeed (no error)

    root_ptr = get_root_addr();
    EXPECT_TRUE(root_ptr != NULL);

    traverse(root_ptr, 0, 0, &stack, paths, &path_count);
    EXPECT_EQ(1, path_count);
    found = find_path_by_prefix(paths, path_count,
                                (base >> (32 - mask)) & 0xffffffff);

    EXPECT_TRUE(found != NULL);
    EXPECT_EQ(found->full_prefix, (base >> (32 - mask)) & 0xffffffff);
    EXPECT_EQ(found->mask, 16);
}

TEST_F(AddFunctionTest, BoundaryMask0) {
    EXPECT_EQ(0, add(0, 0)); // 0.0.0.0/0
}

TEST_F(AddFunctionTest, BoundaryMask32) {
    unsigned int base = 0x0A000000; // 10.0.0.0
    EXPECT_EQ(0, add(base, 32));    // 10.0.0.0/32
}
