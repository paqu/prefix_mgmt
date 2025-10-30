#include "prefix_mgmt/prefix_mgmt.h"
#include <gtest/gtest.h>

class DelFunctionTest : public ::testing::Test {
  protected:
    void SetUp() override { prefix_mgmt_init(); }

    void TearDown() override { prefix_mgmt_cleanup(); }
};

// TC-DEL-1: Invalid mask below range
TEST_F(DelFunctionTest, InvalidMaskBelowRange) {
    unsigned int base = 0xC0AB0000; // 192.168.0.0
    EXPECT_EQ(-1, del(base, -1));
}

// TC-DEL-2: Invalid mask above range
TEST_F(DelFunctionTest, InvalidMaskAboveRange) {
    unsigned int base = 0xC0AB0000; // 192.168.0.0
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
