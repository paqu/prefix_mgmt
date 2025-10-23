#include <gtest/gtest.h>

class CheckFunctionTest : public ::testing::Test {
  protected:
    void SetUp() override {}

    void TearDown() override {}
};

TEST_F(CheckFunctionTest, Template) { EXPECT_EQ(1, 1); }
