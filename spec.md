# IPv4 Prefix Management System - Specification

## 1. General Description

A system for managing a collection of IPv4 prefixes implemented in C language. An IPv4 prefix defines a range of IP addresses using a base address and a network mask.

## 2. Definitions

- **IPv4 Prefix**: A pair consisting of a base address (32 bits) and mask length (0-32)
- **Base Address**: A 32-bit number representing an IP address
- **Mask**: A number in the range 0-32 specifying the number of most significant bits in the network mask

## 3. Examples

- Prefix `10.20.0.0/16` represents range `10.20.0.0 - 10.20.255.255`
- Prefix `32.64.128.0/20` represents range `32.64.128.0 - 32.64.143.255`

## 4. Functional Requirements

### 4.1. Add Prefix Function
```c
int add(unsigned int base, char mask)
```

**Description**: Adds a prefix to the collection

**Parameters**:
- `base`: Base address of the prefix (32-bit unsigned)
- `mask`: Mask length (0-32)

**Return Value**:
- `0`: Operation successful
- `-1`: Invalid arguments

**Behavior**:
- Prefixes are unique - adding an existing prefix has no effect
- Adding a prefix does not remove smaller prefixes contained within it

### 4.2. Delete Prefix Function
```c
int del(unsigned int base, char mask)
```

**Description**: Removes a prefix from the collection

**Parameters**:
- `base`: Base address of the prefix (32-bit unsigned)
- `mask`: Mask length (0-32)

**Return Value**:
- `0`: Operation successful
- `-1`: Invalid arguments

**Behavior**:
- Removing a prefix that contains smaller prefixes does not remove them

### 4.3. Check Address Function
```c
char check(unsigned int ip)
```

**Description**: Checks if an IP address is contained in the prefix collection

**Parameters**:
- `ip`: IP address to check (32-bit unsigned)

**Return Value**:
- Mask value (0-32): Length of the mask of the smallest prefix (with highest mask value) containing the address
- `-1`: Address is not contained in any prefix in the collection

## 5. Non-Functional Requirements

### 5.1. Performance

- The `check()` function will be called most frequently - requires optimization
- Computational complexity of all operations should be optimized
- Memory usage should be efficient

### 5.2. Data Structure

- Must support an arbitrarily large collection of prefixes
- Implementation from scratch (no ready-made data structure libraries)

### 5.3. Code Quality

- Code should be concise, aesthetic, and readable
- Portable (compliant with C language standard)
- Includes unit tests for all operations

