#ifndef PREFIX_MGMT_H
#define PREFIX_MGMT_H

#include <stdbool.h>

/**
 * @file prefix_mgmt.h
 * @brief Interface for IPv4 Prefix Management System.
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Radix trie node
 *
 * Unlike binary trie where each node represents one bit,
 * radix node can represent multiple bits (compressed path).
 */
typedef struct radix_node {
    struct radix_node *left;  /**< Child for bit sequence starting with 0 */
    struct radix_node *right; /**< Child for bit sequence starting with 1 */

    unsigned int prefix; /**< Prefix bits stored in this node */
    unsigned char skip;  /**< Number of bits to skip (path compression) */

    bool is_prefix; /**< True if this represents a complete prefix */
    char mask;      /**< Mask length if is_prefix is true */
} radix_node_t;

radix_node_t *get_root_addr(void);

int prefix_mgmt_init(void);
void prefix_mgmt_cleanup(void);

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
