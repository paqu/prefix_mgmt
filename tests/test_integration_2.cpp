#include "prefix_mgmt/prefix_mgmt.h"
#include <gtest/gtest.h>

class IntegrationTest2 : public ::testing::Test {
  protected:
    void SetUp() override { prefix_mgmt_init(); }

    void TearDown() override { prefix_mgmt_cleanup(); }
};

TEST_F(IntegrationTest2, BasicInsertDelete) {
    // 192.168.1.0/24 = 0xC0A80100
    ASSERT_EQ(add(0xC0A80100, 24), 0);
    ASSERT_EQ(check(0xC0A80100), 24);

    ASSERT_EQ(del(0xC0A80100, 24), 0);
    ASSERT_EQ(check(0xC0A80100), -1);
}

TEST_F(IntegrationTest2, CleanupLeafWithPrefixParent) {
    // Insert 192.168.0.0/16 and 192.168.1.0/24
    ASSERT_EQ(add(0xC0A80000, 16), 0); // Parent
    ASSERT_EQ(add(0xC0A80100, 24), 0); // Child

    ASSERT_EQ(check(0xC0A80000), 16);
    ASSERT_EQ(check(0xC0A80100), 24);

    // Delete child - parent should stay (it's a prefix)
    ASSERT_EQ(del(0xC0A80100, 24), 0);

    ASSERT_EQ(check(0xC0A80000), 16);
    ASSERT_EQ(check(0xC0A80100), 16); // Should match parent prefix

    // Cleanup
    ASSERT_EQ(del(0xC0A80000, 16), 0);
}

TEST_F(IntegrationTest2, CleanupLeafMergeWithSibling) {
    // Insert three prefixes creating branch
    // 192.168.1.0/24
    // 192.168.2.0/24
    // 192.168.1.128/25
    ASSERT_EQ(add(0xC0A80100, 24), 0);
    ASSERT_EQ(add(0xC0A80200, 24), 0);
    ASSERT_EQ(add(0xC0A80180, 25), 0);

    // Now delete 192.168.1.0/24
    // The parent of /24 and /25 should merge with /25
    ASSERT_EQ(del(0xC0A80100, 24), 0);

    ASSERT_EQ(check(0xC0A80100), -1);
    ASSERT_EQ(check(0xC0A80180), 25);
    ASSERT_EQ(check(0xC0A80200), 24);

    // Cleanup
    ASSERT_EQ(del(0xC0A80180, 25), 0);
    ASSERT_EQ(del(0xC0A80200, 24), 0);
}

TEST_F(IntegrationTest2, DeleteNodeWithTwoChildren) {
    // 192.168.1.0/24 (parent)
    // 192.168.1.0/25 (left child)
    // 192.168.1.128/25 (right child)
    ASSERT_EQ(add(0xC0A80100, 24), 0);
    ASSERT_EQ(add(0xC0A80100, 25), 0);
    ASSERT_EQ(add(0xC0A80180, 25), 0);

    // Delete parent - should stay as branch point
    ASSERT_EQ(del(0xC0A80100, 24), 0);

    ASSERT_EQ(check(0xC0A80100), 25); // Should match left child
    ASSERT_EQ(check(0xC0A80180), 25);

    // Cleanup
    ASSERT_EQ(del(0xC0A80100, 25), 0);
    ASSERT_EQ(del(0xC0A80180, 25), 0);
}

TEST_F(IntegrationTest2, DeleteNodeWithOneChild) {
    // Insert 192.168.1.0/24 and 192.168.1.128/25
    ASSERT_EQ(add(0xC0A80100, 24), 0);
    ASSERT_EQ(add(0xC0A80180, 25), 0);

    // Delete /24 - should merge with /25
    ASSERT_EQ(del(0xC0A80100, 24), 0);

    ASSERT_EQ(check(0xC0A80100), -1);
    ASSERT_EQ(check(0xC0A80180), 25);

    // Cleanup
    ASSERT_EQ(del(0xC0A80180, 25), 0);
}

TEST_F(IntegrationTest2, CascadeCleanup) {
    // Build: 192.168.0.0/20 -> 192.168.1.0/24 -> 192.168.1.128/25
    ASSERT_EQ(add(0xC0A80000, 20), 0);
    ASSERT_EQ(add(0xC0A80100, 24), 0);
    ASSERT_EQ(add(0xC0A80180, 25), 0);

    // Delete from bottom up
    ASSERT_EQ(del(0xC0A80180, 25), 0); // /25 deleted, /24 stays
    ASSERT_EQ(del(0xC0A80100, 24), 0); // /24 deleted, should merge up

    ASSERT_EQ(check(0xC0A80000), 20);
    ASSERT_EQ(check(0xC0A80100), 20); // Should match parent
    ASSERT_EQ(check(0xC0A80180), 20); // Should match parent

    // Cleanup
    ASSERT_EQ(del(0xC0A80000, 20), 0);
}

TEST_F(IntegrationTest2, ComplexScenario) {
    // Build complex tree
    ASSERT_EQ(add(0xC0A80000, 16), 0); // 192.168.0.0/16
    ASSERT_EQ(add(0xC0A80100, 24), 0); // 192.168.1.0/24
    ASSERT_EQ(add(0xC0A80200, 24), 0); // 192.168.2.0/24
    ASSERT_EQ(add(0xC0A80300, 24), 0); // 192.168.3.0/24
    ASSERT_EQ(add(0xC0A80180, 25), 0); // 192.168.1.128/25

    // Verify all exist
    ASSERT_EQ(check(0xC0A80000), 16);
    ASSERT_EQ(check(0xC0A80100), 24);
    ASSERT_EQ(check(0xC0A80200), 24);
    ASSERT_EQ(check(0xC0A80300), 24);
    ASSERT_EQ(check(0xC0A80180), 25);

    // Delete in specific order
    ASSERT_EQ(del(0xC0A80100, 24), 0);
    ASSERT_EQ(del(0xC0A80180, 25), 0);
    ASSERT_EQ(del(0xC0A80200, 24), 0);

    // Only /16 and 3.0/24 should remain
    ASSERT_EQ(check(0xC0A80000), 16);
    ASSERT_EQ(check(0xC0A80300), 24);
    ASSERT_EQ(check(0xC0A80100), 16); // Should match /16
    ASSERT_EQ(check(0xC0A80200), 16); // Should match /16

    // Cleanup
    ASSERT_EQ(del(0xC0A80000, 16), 0);
    ASSERT_EQ(del(0xC0A80300, 24), 0);
}

TEST_F(IntegrationTest2, RootPrefix) {
    ASSERT_EQ(add(0x00000000, 0), 0);
    ASSERT_EQ(check(0x00000000), 0);

    ASSERT_EQ(del(0x00000000, 0), 0);
    ASSERT_EQ(check(0x00000000), -1);
}
