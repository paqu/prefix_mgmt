#include <gtest/gtest.h>

class DelFunctionTest : public ::testing::Test {
  protected:
    void SetUp() override {}

    void TearDown() override {}
};

TEST_F(DelFunctionTest, Template) { EXPECT_EQ(1, 1); }
