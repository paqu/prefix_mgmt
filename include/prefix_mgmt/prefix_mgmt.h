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
 * @struct radix_node
 * @brief Node in radix tree with path compression.
 *
 * This structure stores IPv4 prefixes efficiently. Each node can
 * store multiple bits to save memory.
 *
 * @var radix_node::left
 * Child for paths starting with 0
 *
 * @var radix_node::right
 * Child for paths starting with 1
 *
 * @var radix_node::prefix
 * The bit sequence stored here
 *
 * @var radix_node::skip
 * How many bits this node stores
 *
 * @var radix_node::is_prefix
 * True if this is a complete prefix
 *
 * @var radix_node::mask
 * Prefix length (0-32) if is_prefix is true, -1 otherwise
 */
typedef struct radix_node {
    struct radix_node *left;  /**< Child for bit sequence starting with 0 */
    struct radix_node *right; /**< Child for bit sequence starting with 1 */

    unsigned int prefix; /**< Prefix bits stored in this node */
    unsigned char skip;  /**< Number of bits to skip (path compression) */

    bool is_prefix; /**< True if this represents a complete prefix */
    char mask;      /**< Mask length if is_prefix is true */
} radix_node_t;

/**
 * @brief Gets the root node of the radix tree.
 *
 * @return Pointer to root node, or NULL if not initialized
 */
radix_node_t *get_root_addr(void);

/**
 * @brief Initializes the prefix management system.
 *
 * Creates the root node. If root already exists, cleans up first.
 *
 * @return 0 on success, -1 if memory allocation fails
 */
int prefix_mgmt_init(void);

/**
 * @brief Cleans up and frees all memory.
 *
 * Frees all nodes in the tree and sets root to NULL.
 */
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
