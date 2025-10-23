#ifndef PREFIX_MGMT_H
#define PREFIX_MGMT_H

/**
 * @file prefix_mgmt.h
 * @brief Interface for IPv4 Prefix Management System.
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Adds an IPv4 prefix to the collection.
 *
 * @param base  Base address of the prefix (32-bit unsigned integer)
 * @param mask  Mask length (0–32)
 * @return 0 on success, -1 on invalid arguments
 */
int add(unsigned int base, char mask);

/**
 * @brief Removes an IPv4 prefix from the collection.
 *
 * @param base  Base address of the prefix (32-bit unsigned integer)
 * @param mask  Mask length (0–32)
 * @return 0 on success, -1 on invalid arguments
 */
int del(unsigned int base, char mask);

/**
 * @brief Checks if the given IP address is contained within any stored prefix.
 *
 * @param ip  IPv4 address to check (32-bit unsigned integer)
 * @return
 *   - mask value (0–32): mask of the smallest prefix (with highest mask value)
 *     that contains the address
 *   - -1 if the address is not contained in any prefix
 */
char check(unsigned int ip);

#ifdef __cplusplus
}
#endif

#endif /* PREFIX_MGMT_H */
