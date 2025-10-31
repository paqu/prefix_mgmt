#include "test_utils.h"
#include <gtest/gtest.h>

class CheckFunctionTest : public ::testing::Test {
  protected:
    void SetUp() override { prefix_mgmt_init(); }

    void TearDown() override { prefix_mgmt_cleanup(); }
};

// TC-CHK-1 Check without initialization
TEST_F(CheckFunctionTest, CheckWithoutInit) {
    prefix_mgmt_cleanup();

    EXPECT_EQ(-1, check(0x0A000000)); // Should return -1
}

// TC-CHK-2: Empty Collection
TEST_F(CheckFunctionTest, EmptyCollection) {
    // No prefixes added
    EXPECT_EQ(-1, check(0x0A000000)); // 10.0.0.0
    EXPECT_EQ(-1, check(0xFFFFFFFF)); // 255.255.255.255
}
// TC-CHK-3: Boundary - IP 0.0.0.0
TEST_F(CheckFunctionTest, BoundaryIP_0_0_0_0) {
    add(0x00000000, 0); // 0.0.0.0/0 (default route)

    EXPECT_EQ(0, check(0x00000000)); // 0.0.0.0
}

// TC-CHK-4: Default route catches everything
TEST_F(CheckFunctionTest, DefaultRouteCatchesEverything) {
    add(0x00000000, 0); // 0.0.0.0/0

    // Everything should match
    EXPECT_EQ(0, check(0x00000000));
    EXPECT_EQ(0, check(0x0A000000));
    EXPECT_EQ(0, check(0x7F000001));
    EXPECT_EQ(0, check(0xFFFFFFFF));
}

TEST_F(CheckFunctionTest, ExactMatch) {
    unsigned int base = 0x0A000000; // 10.0.0.0
    add(base, 8);

    // Check exact base address
    EXPECT_EQ(8, check(0x0A000000)); // 10.0.0.0
}
