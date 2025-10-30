#include "prefix_mgmt/prefix_mgmt.h"
#include "test_utils.h"
#include <gtest/gtest.h>

class DelFunctionTest : public ::testing::Test {
  protected:
    void SetUp() override { prefix_mgmt_init(); }

    void TearDown() override { prefix_mgmt_cleanup(); }
};

// TC-DEL-1: Invalid mask below range
TEST_F(DelFunctionTest, InvalidMaskBelowRange) {
    unsigned int base = 0xC0A80000; // 192.168.0.0
    EXPECT_EQ(-1, del(base, -1));
}

// TC-DEL-2: Invalid mask above range
TEST_F(DelFunctionTest, InvalidMaskAboveRange) {
    unsigned int base = 0xC0A80000; // 192.168.0.0
    EXPECT_EQ(-1, del(base, 33));
}

// TC-DEL-3: Invalid Base - Misaligned
TEST_F(DelFunctionTest, InvalidBaseMisaligned) {
    unsigned int base = 0x80000000;

    for (int i = 1; i < 32; i++) {
        base >>= 1;
        EXPECT_EQ(-1, del(base, i));
    }
}

// TC-DEL-4: Delete from empty tree - should succeed (no-op)
TEST_F(DelFunctionTest, DeleteFromEmptyTree) {
    prefix_mgmt_cleanup(); // Ensure tree is empty

    // Deleting from empty tree should succeed (graceful no-op)
    EXPECT_EQ(0, del(0x0A000000, 16));
}

// TC-DEL-5: Delete existing prefix - should mark as deleted
TEST_F(DelFunctionTest, DeleteExistingPrefix) {
    NodeStack stack = {.top = 0};
    PathEntry paths[MAX_PATHS];
    int path_count = 0;

    unsigned int base = 0x0A000000; // 10.0.0.0
    char mask = 16;

    // Arrange: Add prefix
    EXPECT_EQ(0, add(base, mask));

    // Verify it exists
    traverse(get_root_addr(), 0, 0, &stack, paths, &path_count);
    EXPECT_EQ(1, path_count);
    PathEntry *found =
        find_path_by_prefix(paths, path_count, (base >> (32 - mask)));
    EXPECT_TRUE(found != NULL);
    EXPECT_EQ(found->mask, mask);

    // Act: Delete the prefix
    EXPECT_EQ(0, del(base, mask));

    // Assert: Prefix should be gone
    path_count = 0;
    memset(paths, 0, MAX_PATHS * sizeof(PathEntry));
    traverse(get_root_addr(), 0, 0, &stack, paths, &path_count);
    EXPECT_EQ(0, path_count); // No prefixes should remain

    found = find_path_by_prefix(paths, path_count, (base >> (32 - mask)));
    EXPECT_TRUE(found == NULL); // Prefix should not be found
}
