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

// TC-CHK-4: Boundary - IP 255.255.255.255
TEST_F(CheckFunctionTest, BoundaryIP_255_255_255_255) {
    add(0x00000000, 0); // 0.0.0.0/0 (covers everything)

    EXPECT_EQ(0, check(0xFFFFFFFF)); // 255.255.255.255
}

// TC-CHK-5: Default route catches everything
TEST_F(CheckFunctionTest, DefaultRouteCatchesEverything) {
    add(0x00000000, 0); // 0.0.0.0/0

    // Everything should match
    EXPECT_EQ(0, check(0x00000000));
    EXPECT_EQ(0, check(0x0A000000));
    EXPECT_EQ(0, check(0x7F000001));
    EXPECT_EQ(0, check(0xFFFFFFFF));
}
// TC-CHk-5 - Exact match
TEST_F(CheckFunctionTest, ExactMatch) {
    unsigned int base = 0x0A000000; // 10.0.0.0
    add(base, 8);

    // Check exact base address
    EXPECT_EQ(8, check(0x0A000000)); // 10.0.0.0
}

// TC-CHK-6: Match Within Range
TEST_F(CheckFunctionTest, MatchWithinRange) {
    unsigned int base = 0x0A000000; // 10.0.0.0/8
    add(base, 8);

    // Check IP within range
    EXPECT_EQ(8, check(0x0A141E28)); // 10.20.30.40
    EXPECT_EQ(8, check(0x0AFFFFFF)); // 10.255.255.255
    EXPECT_EQ(8, check(0x0A000001)); // 10.0.0.1
}

// TC-CHK-7: No Match
TEST_F(CheckFunctionTest, NoMatch) {
    unsigned int base = 0x0A000000; // 10.0.0.0/8
    add(base, 8);

    // Check IP outside range
    EXPECT_EQ(-1, check(0x0B000000)); // 11.0.0.0
    EXPECT_EQ(-1, check(0xC0A80000)); // 192.168.0.0
}

// TC-CHK-8: Longest Prefix Match - Nested
TEST_F(CheckFunctionTest, LongestPrefixMatchNested) {
    // Add nested prefixes
    add(0x0A000000, 8);  // 10.0.0.0/8
    add(0x0A140000, 16); // 10.20.0.0/16
    add(0x0A141E00, 24); // 10.20.30.0/24

    // Check returns most specific
    EXPECT_EQ(24, check(0x0A141E28)); // 10.20.30.40 -> /24
    EXPECT_EQ(24, check(0x0A141E00)); // 10.20.30.0 -> /24
}

// TC-CHK-9: Longest Prefix Match - Intermediate
TEST_F(CheckFunctionTest, LongestPrefixMatchIntermediate) {
    // Add two levels
    add(0x0A000000, 8);  // 10.0.0.0/8
    add(0x0A140000, 16); // 10.20.0.0/16

    // Check IP that matches /16 but not more specific
    EXPECT_EQ(16, check(0x0A140100)); // 10.20.1.0 -> /16
    EXPECT_EQ(16, check(0x0A14FFFF)); // 10.20.255.255 -> /16

    // Check IP that only matches /8
    EXPECT_EQ(8, check(0x0A0A0000)); // 10.10.0.0 -> /8
}

// TC-CHK-10: Multiple Disjoint Prefixes
TEST_F(CheckFunctionTest, MultipleDisjointPrefixes) {
    // Add disjoint prefixes
    add(0x0A000000, 8);  // 10.0.0.0/8
    add(0xAC100000, 12); // 172.16.0.0/12
    add(0xC0A80000, 16); // 192.168.0.0/16

    // Check each range
    EXPECT_EQ(8, check(0x0A000000));  // 10.0.0.0 -> /8
    EXPECT_EQ(12, check(0xAC100000)); // 172.16.0.0 -> /12
    EXPECT_EQ(16, check(0xC0A80000)); // 192.168.0.0 -> /16

    // Check outside ranges
    EXPECT_EQ(-1, check(0x0B000000)); // 11.0.0.0 -> not found
}

// TC-CHK-11: Host Prefix (/32)
TEST_F(CheckFunctionTest, HostPrefix32) {
    // Add single host
    add(0x0A000001, 32); // 10.0.0.1/32

    // Only exact IP matches
    EXPECT_EQ(32, check(0x0A000001)); // 10.0.0.1 -> found
    EXPECT_EQ(-1, check(0x0A000002)); // 10.0.0.2 -> not found
}

// Additional test: Complex nested scenario
TEST_F(CheckFunctionTest, ComplexNestedScenario) {
    // RFC1918 private networks
    add(0x0A000000, 8);  // 10.0.0.0/8
    add(0x0A140000, 16); // 10.20.0.0/16
    add(0x0A141E00, 24); // 10.20.30.0/24
    add(0x0A141E28, 32); // 10.20.30.40/32

    // Test different levels
    EXPECT_EQ(32, check(0x0A141E28)); // 10.20.30.40 -> /32 (most specific)
    EXPECT_EQ(24, check(0x0A141E29)); // 10.20.30.41 -> /24
    EXPECT_EQ(16, check(0x0A140001)); // 10.20.0.1 -> /16
    EXPECT_EQ(8, check(0x0A0B0001));  // 10.11.0.1 -> /8
    EXPECT_EQ(-1, check(0x0B000000)); // 11.0.0.0 -> not found
}
