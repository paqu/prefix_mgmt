#include <gtest/gtest.h>

#include "prefix_mgmt/prefix_mgmt.h"

class IntegrationTest : public ::testing::Test {
  protected:
    void SetUp() override { prefix_mgmt_init(); }

    void TearDown() override { prefix_mgmt_cleanup(); }
};

// TC-INT-1: Add-Check-Delete Sequence
TEST_F(IntegrationTest, AddCheckDeleteSequence) {
    // Add
    EXPECT_EQ(0, add(0x0A000000, 8));

    // Check
    EXPECT_EQ(8, check(0x0A0A0000)); // 10.10.0.0

    // Delete
    EXPECT_EQ(0, del(0x0A000000, 8));

    // Check again
    EXPECT_EQ(-1, check(0x0A0A0000));
}

// TC-INT-2: Complex Scenario - Network Routing Table
TEST_F(IntegrationTest, NetworkRoutingTableSimulation) {
    // Simulate typical routing table

    // Default route
    add(0x00000000, 0);

    // RFC1918 private networks
    add(0x0A000000, 8);  // 10.0.0.0/8
    add(0xAC100000, 12); // 172.16.0.0/12
    add(0xC0A80000, 16); // 192.168.0.0/16

    // More specific routes
    add(0x0A140000, 16); // 10.20.0.0/16
    add(0x0A141E00, 24); // 10.20.30.0/24

    // Test lookups
    EXPECT_EQ(24, check(0x0A141E28)); // 10.20.30.40 -> /24
    EXPECT_EQ(16, check(0x0A140001)); // 10.20.0.1 -> /16
    EXPECT_EQ(8, check(0x0A0A0000));  // 10.10.0.0 -> /8
    EXPECT_EQ(12, check(0xAC100000)); // 172.16.0.0 -> /12
    EXPECT_EQ(16, check(0xC0A80001)); // 192.168.0.1 -> /16
    EXPECT_EQ(0, check(0x08080808));  // 8.8.8.8 -> /0 (default)

    // Update routes
    del(0x0A140000, 16);              // Remove /16
    EXPECT_EQ(24, check(0x0A141E28)); // 10.20.30.40 still /24
    EXPECT_EQ(8, check(0x0A140001));  // 10.20.0.1 now /8
}

// TC-INT-3: Stress Test - Large Scale (simplified)
TEST_F(IntegrationTest, LargeScaleOperations) {
    // Add many prefixes in /24 range
    for (unsigned int i = 0; i < 100; i++) {
        unsigned int base = 0x0A000000 | (i << 8); // 10.0.i.0/24
        EXPECT_EQ(0, add(base, 24));
    }

    // Check they all exist
    for (unsigned int i = 0; i < 100; i++) {
        unsigned int ip = 0x0A000000 | (i << 8) | 1; // 10.0.i.1
        EXPECT_EQ(24, check(ip));
    }

    // Delete half
    for (unsigned int i = 0; i < 50; i++) {
        unsigned int base = 0x0A000000 | (i << 8);
        EXPECT_EQ(0, del(base, 24));
    }

    // Check deleted ones are gone
    for (unsigned int i = 0; i < 50; i++) {
        unsigned int ip = 0x0A000000 | (i << 8) | 1;
        EXPECT_EQ(-1, check(ip));
    }

    // Check remaining ones still exist
    for (unsigned int i = 50; i < 100; i++) {
        unsigned int ip = 0x0A000000 | (i << 8) | 1;
        EXPECT_EQ(24, check(ip));
    }
}

// Realistic scenario: ISP prefix management
TEST_F(IntegrationTest, ISPPrefixManagement) {
    // ISP has aggregate
    add(0xC6336000, 19); // 198.51.96.0/19

    // Allocates to customers
    add(0xC6336000, 24); // 198.51.96.0/24 - Customer A
    add(0xC6336100, 24); // 198.51.97.0/24 - Customer B
    add(0xC6336200, 24); // 198.51.98.0/24 - Customer C

    // Check routing
    EXPECT_EQ(24, check(0xC6336001)); // 198.51.96.1 -> Customer A
    EXPECT_EQ(24, check(0xC6336101)); // 198.51.97.1 -> Customer B
    EXPECT_EQ(19, check(0xC6336300)); // 198.51.99.0 -> Aggregate

    // Customer B leaves, revoke their prefix
    del(0xC6336100, 24);
    EXPECT_EQ(19, check(0xC6336101)); // Now routes to aggregate
}
