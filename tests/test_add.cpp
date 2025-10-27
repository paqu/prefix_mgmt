#include "prefix_mgmt/prefix_mgmt.h"
#include <gtest/gtest.h>

class AddFunctionTest : public ::testing::Test {
  protected:
    void SetUp() override {}

    void TearDown() override {}
};

#if 0 
// TC-ADD-1: Valid Prefix Addition
TEST_F(AddFunctionTest, ValidPrefixAddition) {
    // 10.0.0.0/8
    // tbd later
}
#endif

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
