# Test Case Specification - Prefix Management System

**Project:** IP Prefix Management System (Radix Tree)  
**Created:** 2025-11-04  
**Version:** 1.0

---

## Table of Contents

1. [`add()` Function Tests](#1-add-function-tests)
2. [`check()` Function Tests](#2-check-function-tests)
3. [`del()` Function Tests](#3-del-function-tests)
4. [Integration Tests](#4-integration-tests)
5. [Advanced Integration Tests](#5-advanced-integration-tests)

---

## 1. `add()` Function Tests

### TC-ADD-1: Valid Prefix Addition
**Purpose:** Verify that prefixes are correctly added to the radix tree

**Pre-conditions:**
- System initialized using `prefix_mgmt_init()`

**Test Steps:**

| Step | Action | Input Data | Expected Result |
|------|--------|------------|-----------------|
| 1 | Add first prefix | `add(0x0A000000, 16)` (10.0.0.0/16) | Returns 0 (success) |
| 2 | Check tree content | Traverse tree | 1 prefix in tree |
| 3 | Add second prefix | `add(0xA0000000, 8)` (160.0.0.0/8) | Returns 0 (success) |
| 4 | Check tree content | Traverse tree | 2 prefixes in tree |
| 5 | Add third prefix | `add(0xA1000000, 8)` (161.0.0.0/8) | Returns 0 (success) |
| 6 | Check tree content | Traverse tree | 3 prefixes in tree |
| 7 | Add fourth prefix (parent) | `add(0xA0000000, 4)` (160.0.0.0/4) | Returns 0 (success) |
| 8 | Check tree content | Traverse tree | 4 prefixes in tree |

**Expected Outcome:** All prefixes are successfully added and stored in the tree with correct prefix lengths

---

### TC-ADD-2: Invalid Mask - Below Range
**Purpose:** Check that the function rejects mask values below valid range

**Pre-conditions:**
- System initialized using `prefix_mgmt_init()`

**Test Steps:**

| Step | Action | Input Data | Expected Result |
|------|--------|------------|-----------------|
| 1 | Try to add prefix with invalid mask | `add(0x0A000000, -1)` (10.0.0.0/-1) | Returns -1 (error) |

**Expected Outcome:** Function returns error code -1 for invalid mask

---

### TC-ADD-3: Invalid Mask - Above Range
**Purpose:** Check that the function rejects mask values above valid range

**Pre-conditions:**
- System initialized using `prefix_mgmt_init()`

**Test Steps:**

| Step | Action | Input Data | Expected Result |
|------|--------|------------|-----------------|
| 1 | Try to add prefix with invalid mask | `add(0x0A000000, 33)` (10.0.0.0/33) | Returns -1 (error) |

**Expected Outcome:** Function returns error code -1 for invalid mask

---

### TC-ADD-4: Invalid Base - Misaligned
**Purpose:** Verify that function rejects IP addresses that don't match their mask

**Pre-conditions:**
- System initialized using `prefix_mgmt_init()`

**Test Steps:**

| Step | Action | Input Data | Expected Result |
|------|--------|------------|-----------------|
| 1-31 | Try to add misaligned addresses | Various misaligned IPs with masks 1-31 | All return -1 (error) |

**Example:** `add(0x40000000, 1)` should fail because the address has bits set beyond the mask length

**Expected Outcome:** All attempts to add misaligned addresses fail with error code -1

---

### TC-ADD-5: Valid Prefix - Same Address Different Mask
**Purpose:** Verify that the same IP address can be added with different mask lengths

**Pre-conditions:**
- System initialized using `prefix_mgmt_init()`

**Test Steps:**

| Step | Action | Input Data | Expected Result |
|------|--------|------------|-----------------|
| 1 | Add prefix /16 | `add(0x0A000000, 16)` (10.0.0.0/16) | Returns 0 (success) |
| 2 | Add prefix /8 | `add(0x0A000000, 8)` (10.0.0.0/8) | Returns 0 (success) |
| 3 | Add prefix /24 | `add(0x0A000000, 24)` (10.0.0.0/24) | Returns 0 (success) |
| 4 | Check tree content | Traverse tree | 3 prefixes in tree |
| 5 | Verify all prefixes exist | Check each prefix | All three prefixes found with correct masks |

**Expected Outcome:** Three different prefixes are stored (10.0.0.0/8, /16, /24)

---

### TC-ADD-6: Duplicate Prefix
**Purpose:** Verify that adding the same prefix twice doesn't create errors

**Pre-conditions:**
- System initialized using `prefix_mgmt_init()`

**Test Steps:**

| Step | Action | Input Data | Expected Result |
|------|--------|------------|-----------------|
| 1 | Add prefix | `add(0x0A0B0000, 16)` (10.11.0.0/16) | Returns 0 (success) |
| 2 | Add same prefix again | `add(0x0A0B0000, 16)` (10.11.0.0/16) | Returns 0 (success) |
| 3 | Check tree content | Traverse tree | Only 1 prefix in tree |

**Expected Outcome:** Duplicate addition succeeds but only one prefix exists in the tree

---

### TC-ADD-7: Boundary Mask 0
**Purpose:** Test the default route (0.0.0.0/0)

**Pre-conditions:**
- System initialized using `prefix_mgmt_init()`

**Test Steps:**

| Step | Action | Input Data | Expected Result |
|------|--------|------------|-----------------|
| 1 | Add default route | `add(0x00000000, 0)` (0.0.0.0/0) | Returns 0 (success) |

**Expected Outcome:** Default route is successfully added

---

### TC-ADD-8: Boundary Mask 32
**Purpose:** Test host-specific prefix (/32)

**Pre-conditions:**
- System initialized using `prefix_mgmt_init()`

**Test Steps:**

| Step | Action | Input Data | Expected Result |
|------|--------|------------|-----------------|
| 1 | Add /32 prefix | `add(0x0A000000, 32)` (10.0.0.0/32) | Returns 0 (success) |

**Expected Outcome:** Host prefix is successfully added

---

### TC-ADD-9: Fail When Not Initialized
**Purpose:** Verify that add() fails if system is not initialized

**Pre-conditions:**
- System cleaned up using `prefix_mgmt_cleanup()`

**Test Steps:**

| Step | Action | Input Data | Expected Result |
|------|--------|------------|-----------------|
| 1 | Try to add prefix | `add(0xA0B00000, 16)` (160.176.0.0/16) | Returns -1 (error) |
| 2 | Initialize system | `prefix_mgmt_init()` | - |
| 3 | Add same prefix | `add(0xA0B00000, 16)` | Returns 0 (success) |

**Expected Outcome:** Function fails before initialization, succeeds after

---

### TC-ADD-10: Multiple Prefixes for Improved Coverage
**Purpose:** Test complex scenarios with multiple related prefixes

**Pre-conditions:**
- System initialized using `prefix_mgmt_init()`

**Test Steps:**

| Step | Action | Input Data | Expected Result |
|------|--------|------------|-----------------|
| 1 | Add first /24 prefix | `add(0x0A0B0100, 24)` (10.11.1.0/24) | Returns 0 (success) |
| 2 | Add second /24 prefix | `add(0x0A0B0200, 24)` (10.11.2.0/24) | Returns 0 (success) |
| 3 | Add third /24 prefix | `add(0x0A0B8000, 24)` (10.11.128.0/24) | Returns 0 (success) |
| 4 | Add parent /16 prefix | `add(0x0A0B0000, 16)` (10.11.0.0/16) | Returns 0 (success) |
| 5 | Check tree content | Traverse tree | 4 prefixes in tree |

**Expected Outcome:** All four prefixes coexist correctly in the tree

---

## 2. `check()` Function Tests

### TC-CHK-1: Check Without Initialization
**Purpose:** Verify that check() fails when system is not initialized

**Pre-conditions:**
- System cleaned up using `prefix_mgmt_cleanup()`

**Test Steps:**

| Step | Action | Input Data | Expected Result |
|------|--------|------------|-----------------|
| 1 | Try to check IP | `check(0x0A000000)` (10.0.0.0) | Returns -1 (not found) |

**Expected Outcome:** Function returns -1 because system is not initialized

---

### TC-CHK-2: Empty Collection
**Purpose:** Verify behavior when no prefixes are stored

**Pre-conditions:**
- System initialized but no prefixes added

**Test Steps:**

| Step | Action | Input Data | Expected Result |
|------|--------|------------|-----------------|
| 1 | Check first IP | `check(0x0A000000)` (10.0.0.0) | Returns -1 (not found) |
| 2 | Check second IP | `check(0xFFFFFFFF)` (255.255.255.255) | Returns -1 (not found) |

**Expected Outcome:** All checks return -1 because no prefixes exist

---

### TC-CHK-3: Boundary IP 0.0.0.0
**Purpose:** Test the lowest possible IP address

**Pre-conditions:**
- System initialized
- Default route added: `add(0x00000000, 0)` (0.0.0.0/0)

**Test Steps:**

| Step | Action | Input Data | Expected Result |
|------|--------|------------|-----------------|
| 1 | Check IP 0.0.0.0 | `check(0x00000000)` | Returns 0 (matched /0) |

**Expected Outcome:** IP matches the default route

---

### TC-CHK-4: Boundary IP 255.255.255.255
**Purpose:** Test the highest possible IP address

**Pre-conditions:**
- System initialized
- Default route added: `add(0x00000000, 0)` (0.0.0.0/0)

**Test Steps:**

| Step | Action | Input Data | Expected Result |
|------|--------|------------|-----------------|
| 1 | Check IP 255.255.255.255 | `check(0xFFFFFFFF)` | Returns 0 (matched /0) |

**Expected Outcome:** IP matches the default route

---

### TC-CHK-5: Default Route Catches Everything
**Purpose:** Verify that default route matches all IP addresses

**Pre-conditions:**
- System initialized
- Default route added: `add(0x00000000, 0)` (0.0.0.0/0)

**Test Steps:**

| Step | Action | Input Data | Expected Result |
|------|--------|------------|-----------------|
| 1 | Check 0.0.0.0 | `check(0x00000000)` | Returns 0 |
| 2 | Check 10.0.0.0 | `check(0x0A000000)` | Returns 0 |
| 3 | Check 127.0.0.1 | `check(0x7F000001)` | Returns 0 |
| 4 | Check 255.255.255.255 | `check(0xFFFFFFFF)` | Returns 0 |

**Expected Outcome:** All IPs match the default route with mask length 0

---

### TC-CHK-6: Exact Match
**Purpose:** Verify exact matching of base address

**Pre-conditions:**
- System initialized
- Prefix added: `add(0x0A000000, 8)` (10.0.0.0/8)

**Test Steps:**

| Step | Action | Input Data | Expected Result |
|------|--------|------------|-----------------|
| 1 | Check base address | `check(0x0A000000)` (10.0.0.0) | Returns 8 (matched /8) |

**Expected Outcome:** Base address matches with correct mask length

---

### TC-CHK-7: Match Within Range
**Purpose:** Verify that IPs within prefix range are matched

**Pre-conditions:**
- System initialized
- Prefix added: `add(0x0A000000, 8)` (10.0.0.0/8)

**Test Steps:**

| Step | Action | Input Data | Expected Result |
|------|--------|------------|-----------------|
| 1 | Check IP in range | `check(0x0A141E28)` (10.20.30.40) | Returns 8 (matched /8) |
| 2 | Check last IP in range | `check(0x0AFFFFFF)` (10.255.255.255) | Returns 8 (matched /8) |
| 3 | Check second IP in range | `check(0x0A000001)` (10.0.0.1) | Returns 8 (matched /8) |

**Expected Outcome:** All IPs in range match the prefix

---

### TC-CHK-8: No Match
**Purpose:** Verify that IPs outside prefix range are not matched

**Pre-conditions:**
- System initialized
- Prefix added: `add(0x0A000000, 8)` (10.0.0.0/8)

**Test Steps:**

| Step | Action | Input Data | Expected Result |
|------|--------|------------|-----------------|
| 1 | Check IP outside range | `check(0x0B000000)` (11.0.0.0) | Returns -1 (not found) |
| 2 | Check another IP outside | `check(0xC0A80000)` (192.168.0.0) | Returns -1 (not found) |

**Expected Outcome:** IPs outside the prefix range are not matched

---

### TC-CHK-9: Longest Prefix Match - Nested
**Purpose:** Verify longest prefix matching with nested prefixes

**Pre-conditions:**
- System initialized
- Prefixes added:
  - `add(0x0A000000, 8)` (10.0.0.0/8)
  - `add(0x0A140000, 16)` (10.20.0.0/16)
  - `add(0x0A141E00, 24)` (10.20.30.0/24)

**Test Steps:**

| Step | Action | Input Data | Expected Result |
|------|--------|------------|-----------------|
| 1 | Check IP matching /24 | `check(0x0A141E28)` (10.20.30.40) | Returns 24 (most specific) |
| 2 | Check base of /24 | `check(0x0A141E00)` (10.20.30.0) | Returns 24 |

**Expected Outcome:** Most specific (longest) prefix is returned

---

### TC-CHK-10: Longest Prefix Match - Intermediate
**Purpose:** Verify longest prefix matching at intermediate levels

**Pre-conditions:**
- System initialized
- Prefixes added:
  - `add(0x0A000000, 8)` (10.0.0.0/8)
  - `add(0x0A140000, 16)` (10.20.0.0/16)

**Test Steps:**

| Step | Action | Input Data | Expected Result |
|------|--------|------------|-----------------|
| 1 | Check IP matching /16 | `check(0x0A140100)` (10.20.1.0) | Returns 16 |
| 2 | Check last IP of /16 | `check(0x0A14FFFF)` (10.20.255.255) | Returns 16 |
| 3 | Check IP matching only /8 | `check(0x0A0A0000)` (10.10.0.0) | Returns 8 |

**Expected Outcome:** Correct prefix level is matched for each IP

---

### TC-CHK-11: Multiple Disjoint Prefixes
**Purpose:** Verify correct matching with multiple separate prefixes

**Pre-conditions:**
- System initialized
- Prefixes added:
  - `add(0x0A000000, 8)` (10.0.0.0/8)
  - `add(0xAC100000, 12)` (172.16.0.0/12)
  - `add(0xC0A80000, 16)` (192.168.0.0/16)

**Test Steps:**

| Step | Action | Input Data | Expected Result |
|------|--------|------------|-----------------|
| 1 | Check first prefix | `check(0x0A000000)` (10.0.0.0) | Returns 8 |
| 2 | Check second prefix | `check(0xAC100000)` (172.16.0.0) | Returns 12 |
| 3 | Check third prefix | `check(0xC0A80000)` (192.168.0.0) | Returns 16 |
| 4 | Check outside all prefixes | `check(0x0B000000)` (11.0.0.0) | Returns -1 (not found) |

**Expected Outcome:** Each IP matches its own prefix, unmatched IP returns -1

---

### TC-CHK-12: Host Prefix (/32)
**Purpose:** Test single host prefix

**Pre-conditions:**
- System initialized
- Host prefix added: `add(0x0A000001, 32)` (10.0.0.1/32)

**Test Steps:**

| Step | Action | Input Data | Expected Result |
|------|--------|------------|-----------------|
| 1 | Check exact host | `check(0x0A000001)` (10.0.0.1) | Returns 32 |
| 2 | Check different host | `check(0x0A000002)` (10.0.0.2) | Returns -1 (not found) |

**Expected Outcome:** Only the exact host IP matches

---

### TC-CHK-13: Complex Nested Scenario
**Purpose:** Test complex hierarchy with multiple nesting levels

**Pre-conditions:**
- System initialized
- Prefixes added (RFC1918 private networks):
  - `add(0x0A000000, 8)` (10.0.0.0/8)
  - `add(0x0A140000, 16)` (10.20.0.0/16)
  - `add(0x0A141E00, 24)` (10.20.30.0/24)
  - `add(0x0A141E28, 32)` (10.20.30.40/32)

**Test Steps:**

| Step | Action | Input Data | Expected Result |
|------|--------|------------|-----------------|
| 1 | Check /32 match | `check(0x0A141E28)` (10.20.30.40) | Returns 32 (most specific) |
| 2 | Check /24 match | `check(0x0A141E29)` (10.20.30.41) | Returns 24 |
| 3 | Check /16 match | `check(0x0A140001)` (10.20.0.1) | Returns 16 |
| 4 | Check /8 match | `check(0x0A0B0001)` (10.11.0.1) | Returns 8 |
| 5 | Check no match | `check(0x0B000000)` (11.0.0.0) | Returns -1 (not found) |

**Expected Outcome:** Each IP returns the most specific matching prefix

---

## 3. `del()` Function Tests

### TC-DEL-1: Invalid Mask - Below Range
**Purpose:** Verify that del() rejects mask values below valid range

**Pre-conditions:**
- System initialized using `prefix_mgmt_init()`

**Test Steps:**

| Step | Action | Input Data | Expected Result |
|------|--------|------------|-----------------|
| 1 | Try to delete with invalid mask | `del(0xC0A80000, -1)` (192.168.0.0/-1) | Returns -1 (error) |

**Expected Outcome:** Function returns error code -1 for invalid mask

---

### TC-DEL-2: Invalid Mask - Above Range
**Purpose:** Verify that del() rejects mask values above valid range

**Pre-conditions:**
- System initialized using `prefix_mgmt_init()`

**Test Steps:**

| Step | Action | Input Data | Expected Result |
|------|--------|------------|-----------------|
| 1 | Try to delete with invalid mask | `del(0xC0A80000, 33)` (192.168.0.0/33) | Returns -1 (error) |

**Expected Outcome:** Function returns error code -1 for invalid mask

---

### TC-DEL-3: Invalid Base - Misaligned
**Purpose:** Verify that del() rejects IP addresses that don't match their mask

**Pre-conditions:**
- System initialized using `prefix_mgmt_init()`

**Test Steps:**

| Step | Action | Input Data | Expected Result |
|------|--------|------------|-----------------|
| 1-31 | Try to delete misaligned addresses | Various misaligned IPs with masks 1-31 | All return -1 (error) |

**Expected Outcome:** All attempts to delete misaligned addresses fail with error code -1

---

### TC-DEL-4: Delete From Not Initialized Tree
**Purpose:** Verify that del() fails when system is not initialized

**Pre-conditions:**
- System cleaned up using `prefix_mgmt_cleanup()`

**Test Steps:**

| Step | Action | Input Data | Expected Result |
|------|--------|------------|-----------------|
| 1 | Try to delete prefix | `del(0x0A000000, 16)` (10.0.0.0/16) | Returns -1 (error) |
| 2 | Initialize system | `prefix_mgmt_init()` | - |
| 3 | Delete prefix | `del(0x0A000000, 16)` | Returns 0 (success) |

**Expected Outcome:** Delete fails before initialization, succeeds after

---

### TC-DEL-5: Delete Existing Prefix
**Purpose:** Verify that existing prefix can be successfully deleted

**Pre-conditions:**
- System initialized
- Prefix added: `add(0x0A000000, 16)` (10.0.0.0/16)

**Test Steps:**

| Step | Action | Input Data | Expected Result |
|------|--------|------------|-----------------|
| 1 | Verify prefix exists | Traverse tree | 1 prefix in tree |
| 2 | Delete the prefix | `del(0x0A000000, 16)` | Returns 0 (success) |
| 3 | Verify prefix is gone | Traverse tree | 0 prefixes in tree |
| 4 | Verify with check | `check(0x0A000000)` | Returns -1 (not found) |

**Expected Outcome:** Prefix is successfully removed from the tree

---

### TC-DEL-6: Delete Multiple Prefixes
**Purpose:** Test deletion of multiple prefixes at different levels

**Pre-conditions:**
- System initialized
- Prefixes added:
  - `add(0x0A000000, 8)` (10.0.0.0/8)
  - `add(0x0A000000, 16)` (10.0.0.0/16)
  - `add(0x0A000000, 24)` (10.0.0.0/24)

**Test Steps:**

| Step | Action | Input Data | Expected Result |
|------|--------|------------|-----------------|
| 1 | Verify all exist | Traverse tree | 3 prefixes in tree |
| 2 | Delete /16 prefix | `del(0x0A000000, 16)` | Returns 0 (success) |
| 3 | Verify only 2 remain | Traverse tree | 2 prefixes (/8 and /24) |
| 4 | Delete /8 prefix | `del(0x0A000000, 8)` | Returns 0 (success) |
| 5 | Verify only 1 remains | Traverse tree | 1 prefix (/24) |
| 6 | Delete /24 prefix | `del(0x0A000000, 24)` | Returns 0 (success) |
| 7 | Verify tree is empty | Traverse tree | 0 prefixes |

**Expected Outcome:** Prefixes are deleted one by one, others remain intact

---

### TC-DEL-7: Delete Default Route
**Purpose:** Test deletion of the default route (0.0.0.0/0)

**Pre-conditions:**
- System initialized
- Default route added: `add(0x00000000, 0)` (0.0.0.0/0)

**Test Steps:**

| Step | Action | Input Data | Expected Result |
|------|--------|------------|-----------------|
| 1 | Verify default route exists | Traverse tree | 1 prefix in tree |
| 2 | Delete default route | `del(0x00000000, 0)` | Returns 0 (success) |
| 3 | Verify tree is empty | Traverse tree | 0 prefixes in tree |

**Expected Outcome:** Default route is successfully removed

---

### TC-DEL-8: Delete Default Route With Other Prefixes
**Purpose:** Verify that deleting default route doesn't affect other prefixes

**Pre-conditions:**
- System initialized
- Prefixes added:
  - `add(0x00000000, 0)` (0.0.0.0/0)
  - `add(0x0A000000, 8)` (10.0.0.0/8)

**Test Steps:**

| Step | Action | Input Data | Expected Result |
|------|--------|------------|-----------------|
| 1 | Verify both exist | Traverse tree | 2 prefixes in tree |
| 2 | Delete default route | `del(0x00000000, 0)` | Returns 0 (success) |
| 3 | Verify /8 still exists | Traverse tree | 1 prefix (/8) in tree |
| 4 | Verify /8 works | `check(0x0A000000)` | Returns 8 |

**Expected Outcome:** Default route deleted, specific prefix remains

---

### TC-DEL-9: Delete Non-Existent Prefix
**Purpose:** Verify behavior when trying to delete a prefix that doesn't exist

**Pre-conditions:**
- System initialized
- Prefixes added:
  - `add(0x0A000000, 8)` (10.0.0.0/8)
  - `add(0x0CA80000, 16)` (12.168.0.0/16)
  - `add(0xEC100000, 12)` (236.16.0.0/12)

**Test Steps:**

| Step | Action | Input Data | Expected Result |
|------|--------|------------|-----------------|
| 1 | Verify 3 prefixes exist | Traverse tree | 3 prefixes in tree |
| 2 | Delete non-existent prefix | `del(0x8B000000, 8)` (139.0.0.0/8) | Returns 0 (success) |
| 3 | Verify nothing changed | Traverse tree | Still 3 prefixes in tree |

**Expected Outcome:** Operation succeeds gracefully, existing prefixes unchanged

---

### TC-DEL-10: Delete From Empty Tree
**Purpose:** Verify behavior when deleting from an empty tree

**Pre-conditions:**
- System initialized
- No prefixes added

**Test Steps:**

| Step | Action | Input Data | Expected Result |
|------|--------|------------|-----------------|
| 1 | Verify tree is empty | Traverse tree | 0 prefixes in tree |
| 2 | Try to delete prefix | `del(0x0A000000, 8)` (10.0.0.0/8) | Returns 0 (success) |
| 3 | Verify tree still empty | Traverse tree | 0 prefixes in tree |

**Expected Outcome:** Operation succeeds gracefully on empty tree

---

### TC-DEL-11: Delete Same Prefix Multiple Times
**Purpose:** Verify behavior when deleting the same prefix repeatedly

**Pre-conditions:**
- System initialized
- Prefixes added:
  - `add(0x0A000000, 8)` (10.0.0.0/8)
  - `add(0xC0A80000, 16)` (192.168.0.0/16)
  - `add(0xAC100000, 12)` (172.16.0.0/12)

**Test Steps:**

| Step | Action | Input Data | Expected Result |
|------|--------|------------|-----------------|
| 1 | Verify 3 prefixes exist | Traverse tree | 3 prefixes in tree |
| 2 | Delete 192.168.0.0/16 first time | `del(0xC0A80000, 16)` | Returns 0 (success) |
| 3 | Verify 2 remain | Traverse tree | 2 prefixes in tree |
| 4 | Delete 192.168.0.0/16 second time | `del(0xC0A80000, 16)` | Returns 0 (success) |
| 5 | Verify still 2 remain | Traverse tree | Still 2 prefixes in tree |

**Expected Outcome:** Second deletion succeeds gracefully, other prefixes unaffected

---

## 4. Integration Tests

### TC-INT-1: Add-Check-Delete Sequence
**Purpose:** Test basic workflow with all three operations

**Pre-conditions:**
- System initialized

**Test Steps:**

| Step | Action | Input Data | Expected Result |
|------|--------|------------|-----------------|
| 1 | Add prefix | `add(0x0A000000, 8)` (10.0.0.0/8) | Returns 0 (success) |
| 2 | Check IP in range | `check(0x0A0A0000)` (10.10.0.0) | Returns 8 (matched) |
| 3 | Delete prefix | `del(0x0A000000, 8)` | Returns 0 (success) |
| 4 | Check same IP again | `check(0x0A0A0000)` | Returns -1 (not found) |

**Expected Outcome:** Complete workflow executes correctly

---

### TC-INT-2: Network Routing Table Simulation
**Purpose:** Simulate a typical network routing table with multiple overlapping prefixes

**Pre-conditions:**
- System initialized

**Test Steps:**

| Step | Action | Input Data | Expected Result |
|------|--------|------------|-----------------|
| 1 | Add default route | `add(0x00000000, 0)` (0.0.0.0/0) | Returns 0 |
| 2-4 | Add RFC1918 private networks | 10.0.0.0/8, 172.16.0.0/12, 192.168.0.0/16 | All return 0 |
| 5-6 | Add specific routes | 10.20.0.0/16, 10.20.30.0/24 | All return 0 |
| 7 | Check 10.20.30.40 | `check(0x0A141E28)` | Returns 24 (most specific) |
| 8 | Check 10.20.0.1 | `check(0x0A140001)` | Returns 16 |
| 9 | Check 10.10.0.0 | `check(0x0A0A0000)` | Returns 8 |
| 10 | Check 172.16.0.0 | `check(0xAC100000)` | Returns 12 |
| 11 | Check 192.168.0.1 | `check(0xC0A80001)` | Returns 16 |
| 12 | Check 8.8.8.8 | `check(0x08080808)` | Returns 0 (default) |
| 13 | Delete /16 route | `del(0x0A140000, 16)` | Returns 0 |
| 14 | Check 10.20.30.40 again | `check(0x0A141E28)` | Returns 24 (still matches /24) |
| 15 | Check 10.20.0.1 again | `check(0x0A140001)` | Returns 8 (now matches /8) |

**Expected Outcome:** Routing table behaves correctly with overlapping prefixes and updates

---

### TC-INT-3: Large Scale Operations
**Purpose:** Test system with many prefixes (stress test)

**Pre-conditions:**
- System initialized

**Test Steps:**

| Step | Action | Input Data | Expected Result |
|------|--------|------------|-----------------|
| 1 | Add 100 /24 prefixes | 10.0.0.0/24 through 10.0.99.0/24 | All return 0 |
| 2 | Verify all exist | Check 10.0.i.1 for i=0 to 99 | All return 24 |
| 3 | Delete first 50 prefixes | Delete 10.0.0.0/24 through 10.0.49.0/24 | All return 0 |
| 4 | Verify deleted | Check 10.0.i.1 for i=0 to 49 | All return -1 |
| 5 | Verify remaining | Check 10.0.i.1 for i=50 to 99 | All return 24 |

**Expected Outcome:** System handles large numbers of prefixes correctly

---

### TC-INT-4: ISP Prefix Management
**Purpose:** Simulate ISP customer prefix allocation scenario

**Pre-conditions:**
- System initialized

**Test Steps:**

| Step | Action | Input Data | Expected Result |
|------|--------|------------|-----------------|
| 1 | Add ISP aggregate | `add(0xC6336000, 19)` (198.51.96.0/19) | Returns 0 |
| 2-4 | Allocate to customers A, B, C | Add 198.51.96.0/24, 97.0/24, 98.0/24 | All return 0 |
| 5 | Check Customer A IP | `check(0xC6336001)` (198.51.96.1) | Returns 24 |
| 6 | Check Customer B IP | `check(0xC6336101)` (198.51.97.1) | Returns 24 |
| 7 | Check unallocated IP | `check(0xC6336300)` (198.51.99.0) | Returns 19 (aggregate) |
| 8 | Customer B leaves | `del(0xC6336100, 24)` (198.51.97.0/24) | Returns 0 |
| 9 | Check Customer B IP again | `check(0xC6336101)` | Returns 19 (back to aggregate) |

**Expected Outcome:** ISP prefix management works correctly with customer allocations and deallocations

---

## 5. Advanced Integration Tests

### TC-INT2-1: Basic Insert Delete
**Purpose:** Test basic insertion and deletion of a single prefix

**Pre-conditions:**
- System initialized

**Test Steps:**

| Step | Action | Input Data | Expected Result |
|------|--------|------------|-----------------|
| 1 | Add prefix | `add(0xC0A80100, 24)` (192.168.1.0/24) | Returns 0 |
| 2 | Verify with check | `check(0xC0A80100)` | Returns 24 |
| 3 | Delete prefix | `del(0xC0A80100, 24)` | Returns 0 |
| 4 | Verify deletion | `check(0xC0A80100)` | Returns -1 |

**Expected Outcome:** Single prefix lifecycle works correctly

---

### TC-INT2-2: Cleanup Leaf With Prefix Parent
**Purpose:** Test that deleting a child node preserves parent prefix

**Pre-conditions:**
- System initialized

**Test Steps:**

| Step | Action | Input Data | Expected Result |
|------|--------|------------|-----------------|
| 1 | Add parent prefix | `add(0xC0A80000, 16)` (192.168.0.0/16) | Returns 0 |
| 2 | Add child prefix | `add(0xC0A80100, 24)` (192.168.1.0/24) | Returns 0 |
| 3 | Verify parent | `check(0xC0A80000)` | Returns 16 |
| 4 | Verify child | `check(0xC0A80100)` | Returns 24 |
| 5 | Delete child | `del(0xC0A80100, 24)` | Returns 0 |
| 6 | Verify parent still exists | `check(0xC0A80000)` | Returns 16 |
| 7 | Check child IP | `check(0xC0A80100)` | Returns 16 (matches parent) |

**Expected Outcome:** Child deletion doesn't affect parent prefix

---

### TC-INT2-3: Cleanup Leaf Merge With Sibling
**Purpose:** Test tree cleanup when deleting leaves with siblings

**Pre-conditions:**
- System initialized

**Test Steps:**

| Step | Action | Input Data | Expected Result |
|------|--------|------------|-----------------|
| 1-3 | Add three prefixes | 192.168.1.0/24, 192.168.2.0/24, 192.168.1.128/25 | All return 0 |
| 4 | Delete 192.168.1.0/24 | `del(0xC0A80100, 24)` | Returns 0 |
| 5 | Verify deleted | `check(0xC0A80100)` | Returns -1 |
| 6 | Verify /25 still exists | `check(0xC0A80180)` (192.168.1.128) | Returns 25 |
| 7 | Verify other /24 exists | `check(0xC0A80200)` (192.168.2.0) | Returns 24 |

**Expected Outcome:** Tree structure correctly maintained after deletion

---

### TC-INT2-4: Delete Node With Two Children
**Purpose:** Test deletion of a node that has both left and right children

**Pre-conditions:**
- System initialized

**Test Steps:**

| Step | Action | Input Data | Expected Result |
|------|--------|------------|-----------------|
| 1 | Add parent /24 | `add(0xC0A80100, 24)` (192.168.1.0/24) | Returns 0 |
| 2 | Add left child /25 | `add(0xC0A80100, 25)` (192.168.1.0/25) | Returns 0 |
| 3 | Add right child /25 | `add(0xC0A80180, 25)` (192.168.1.128/25) | Returns 0 |
| 4 | Delete parent | `del(0xC0A80100, 24)` | Returns 0 |
| 5 | Check left child IP | `check(0xC0A80100)` | Returns 25 |
| 6 | Check right child IP | `check(0xC0A80180)` | Returns 25 |

**Expected Outcome:** Parent removed, both children remain as branch point

---

### TC-INT2-5: Delete Node With One Child
**Purpose:** Test deletion and tree merging when node has single child

**Pre-conditions:**
- System initialized

**Test Steps:**

| Step | Action | Input Data | Expected Result |
|------|--------|------------|-----------------|
| 1 | Add /24 prefix | `add(0xC0A80100, 24)` (192.168.1.0/24) | Returns 0 |
| 2 | Add /25 child | `add(0xC0A80180, 25)` (192.168.1.128/25) | Returns 0 |
| 3 | Delete /24 | `del(0xC0A80100, 24)` | Returns 0 |
| 4 | Check deleted prefix IP | `check(0xC0A80100)` | Returns -1 |
| 5 | Check child still exists | `check(0xC0A80180)` | Returns 25 |

**Expected Outcome:** Parent merges with child, child remains accessible

---

### TC-INT2-6: Cascade Cleanup
**Purpose:** Test cascading tree cleanup through multiple levels

**Pre-conditions:**
- System initialized

**Test Steps:**

| Step | Action | Input Data | Expected Result |
|------|--------|------------|-----------------|
| 1-3 | Build 3-level hierarchy | Add 192.168.0.0/20, 192.168.1.0/24, 192.168.1.128/25 | All return 0 |
| 4 | Delete /25 (bottom) | `del(0xC0A80180, 25)` | Returns 0 |
| 5 | Delete /24 (middle) | `del(0xC0A80100, 24)` | Returns 0 |
| 6 | Check top level | `check(0xC0A80000)` | Returns 20 |
| 7 | Check middle level IP | `check(0xC0A80100)` | Returns 20 (matches parent) |
| 8 | Check bottom level IP | `check(0xC0A80180)` | Returns 20 (matches parent) |

**Expected Outcome:** Tree cascades properly through deletions, top level remains

---

### TC-INT2-7: Complex Scenario
**Purpose:** Test complex tree with multiple branches and deletion patterns

**Pre-conditions:**
- System initialized

**Test Steps:**

| Step | Action | Input Data | Expected Result |
|------|--------|------------|-----------------|
| 1-5 | Build complex tree | Add 192.168.0.0/16, 192.168.1.0/24, 192.168.2.0/24, 192.168.3.0/24, 192.168.1.128/25 | All return 0 |
| 6-10 | Verify all exist | Check all 5 prefixes | All return correct mask values |
| 11-13 | Delete in specific order | Delete 192.168.1.0/24, 192.168.1.128/25, 192.168.2.0/24 | All return 0 |
| 14 | Verify /16 remains | `check(0xC0A80000)` | Returns 16 |
| 15 | Verify 3.0/24 remains | `check(0xC0A80300)` | Returns 24 |
| 16 | Check deleted 1.0 range | `check(0xC0A80100)` | Returns 16 (matches /16) |
| 17 | Check deleted 2.0 range | `check(0xC0A80200)` | Returns 16 (matches /16) |

**Expected Outcome:** Complex deletions maintain tree integrity, correct prefixes remain

---

### TC-INT2-8: Root Prefix
**Purpose:** Test operations on root prefix (default route)

**Pre-conditions:**
- System initialized

**Test Steps:**

| Step | Action | Input Data | Expected Result |
|------|--------|------------|-----------------|
| 1 | Add root prefix | `add(0x00000000, 0)` (0.0.0.0/0) | Returns 0 |
| 2 | Verify with check | `check(0x00000000)` | Returns 0 |
| 3 | Delete root prefix | `del(0x00000000, 0)` | Returns 0 |
| 4 | Verify deletion | `check(0x00000000)` | Returns -1 |

**Expected Outcome:** Root prefix (default route) operations work correctly

---

## Summary

This test specification covers:

- **32 test cases** across all three main functions (`add`, `check`, `del`)
- **8 integration tests** for complex scenarios
- **8 advanced integration tests** for tree structure validation
- **Total: 48 test cases**

