#include "test_utils.h"
#include <gtest/gtest.h>

class CheckFunctionTest : public ::testing::Test {
  protected:
    void SetUp() override { prefix_mgmt_init(); }

    void TearDown() override { prefix_mgmt_cleanup(); }
};

// Test: Check without initialization
TEST_F(CheckFunctionTest, CheckWithoutInit) {
    prefix_mgmt_cleanup();

    EXPECT_EQ(-1, check(0x0A000000)); // Should return -1
}
