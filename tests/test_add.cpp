#include <gtest/gtest.h>

class AddFunctionTest : public ::testing::Test {
  protected:
    void SetUp() override {}

    void TearDown() override {}
};

TEST_F(AddFunctionTest, Template) { EXPECT_EQ(1, 1); }
