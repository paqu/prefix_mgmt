#include "prefix_mgmt/prefix_mgmt.h"
#include <stdbool.h>
#include <stdlib.h>

/**
 * @file prefix_mgmt.c
 * @brief Implementation of IPv4 Prefix Management System functions.
 *
 */

/**
 * @brief Root node of the radix tree.
 *
 * This is the starting point for all operations.
 */
static radix_node_t *g_root = NULL;

/**
 * @brief Creates a new radix node.
 *
 * Allocates memory and sets all fields to default values.
 *
 * @return Pointer to new node, or NULL if allocation fails
 */
static radix_node_t *create_node(void) {
    radix_node_t *node = (radix_node_t *)calloc(1, sizeof(radix_node_t));
    if (node == NULL) {
        return NULL;
    }
    node->left = NULL;
    node->right = NULL;
    node->prefix = 0;
    node->skip = 0;
    node->is_prefix = false;
    node->mask = -1;
    return node;
}

/**
 * @brief Frees a node and all its children.
 *
 * @param node Node to free (can be NULL)
 */
static void free_node(radix_node_t *node) {
    if (node == NULL) {
        return;
    }
    free_node(node->left);
    free_node(node->right);
    free(node);
}

/**
 * @brief Gets a single bit from an IP address.
 *
 * Bits are numbered 0-31, where 0 is the leftmost bit.
 *
 * @param ip IP address
 * @param bit_pos Bit position (0-31)
 * @return 0 or 1
 */
static int get_bit(unsigned int ip, int bit_pos) {
    return (ip >> (31 - bit_pos)) & 1;
}

/**
 * @brief Extracts multiple bits from an IP address.
 *
 * @param ip IP address
 * @param start_bit Starting position
 * @param num_bits How many bits to extract
 * @return Extracted bits
 */
static unsigned int extract_bits(unsigned int ip, int start_bit, int num_bits) {
    if (num_bits == 0)
        return 0;
    if (num_bits == 32 && start_bit == 0)
        return ip;
    unsigned int mask = (1U << num_bits) - 1;
    return (ip >> (32 - start_bit - num_bits)) & mask;
}

/**
 * @brief Counts how many bits match between two prefixes.
 *
 * Compares bits starting from start_bit and stops at first difference.
 *
 * @param prefix1 First prefix
 * @param prefix2 Second prefix
 * @param start_bit Where to start comparing
 * @param max_bits Maximum bits to compare
 * @return Number of matching bits
 */
static int count_matching_bits(unsigned int prefix1, unsigned int prefix2,
                               int start_bit, int max_bits) {
    int match = 0;
    for (int i = 0; i < max_bits; i++) {
        if (get_bit(prefix1, start_bit + i) !=
            get_bit(prefix2, start_bit + i)) {
            break;
        }
        match++;
    }
    return match;
}

#if 0
// TODO - Check in case the current implementation is too slow.
static inline int clz_portable(unsigned int x) {
    if (x == 0)
        return sizeof(unsigned int) * 8;

#if defined(__GNUC__) || defined(__clang__)
    // GCC/Clang intrinsic
    return __builtin_clz(x);
#else
    // Fallback: binary search
    int n = 0;
    if (x <= 0x0000FFFF) {
        n += 16;
        x <<= 16;
    }
    if (x <= 0x00FFFFFF) {
        n += 8;
        x <<= 8;
    }
    if (x <= 0x0FFFFFFF) {
        n += 4;
        x <<= 4;
    }
    if (x <= 0x3FFFFFFF) {
        n += 2;
        x <<= 2;
    }
    if (x <= 0x7FFFFFFF) {
        n += 1;
    }
    return n;
#endif
}

static int count_matching_bits(unsigned int prefix1, unsigned int prefix2,
                               int start_bit, int max_bits) {
    if (max_bits == 0)
        return 0;

    unsigned int p1 = prefix1 << start_bit;
    unsigned int p2 = prefix2 << start_bit;
    unsigned int diff = p1 ^ p2;

    if (diff == 0)
        return max_bits;

    int match = clz_portable(diff);
    return (match < max_bits) ? match : max_bits;
}
#endif
/**
 * @brief Checks if mask length is valid.
 *
 * @param mask Mask to check
 * @return true if mask is 0-32, false otherwise
 */
static bool is_valid_mask(char mask) { return mask >= 0 && mask <= 32; }

/**
 * @brief Checks if base address is correctly aligned.
 *
 * Base is aligned if all host bits are zero.
 * Example: 192.168.1.0/24 is aligned, 192.168.1.5/24 is not.
 *
 * @param base Base address
 * @param mask Mask length
 * @return true if aligned, false otherwise
 */
static bool is_aligned(unsigned int base, char mask) {
    if (mask == 0) {
        return base == 0;
    }
    if (mask == 32) {
        return true;
    }
    unsigned int host_mask = (1U << (32 - mask)) - 1;
    return (base & host_mask) == 0;
}

int add(unsigned int base, char mask) {
    if (!is_valid_mask(mask)) {
        return -1;
    }

    if (!is_aligned(base, mask)) {
        return -1;
    }

    if (g_root == NULL) {
        return -1;
    }

    // Special case: /0 prefix at root
    if (mask == 0) {
        g_root->is_prefix = true;
        g_root->mask = 0;
        return 0;
    }

    radix_node_t *current = g_root;
    int bit_pos = 0;

    while (bit_pos < mask) {
        int remaining = mask - bit_pos;

        // Determine which child to follow
        int first_bit = get_bit(base, bit_pos);
        radix_node_t **child_ptr =
            (first_bit == 0) ? &current->left : &current->right;

        if (*child_ptr == NULL) {
            // Create new node with compressed path
            radix_node_t *new_node = create_node();
            if (new_node == NULL) {
                return -1;
            }

            new_node->skip = remaining;
            new_node->prefix = extract_bits(base, bit_pos, remaining);
            new_node->is_prefix = true;
            new_node->mask = mask;

            *child_ptr = new_node;
            return 0;
        }

        // Node exists - check for path compression match
        radix_node_t *child = *child_ptr;
        int match_bits = count_matching_bits(
            base, child->prefix << (32 - bit_pos - child->skip), bit_pos,
            (remaining < child->skip) ? remaining : child->skip);

        if (match_bits == child->skip && match_bits == remaining) {
            // Perfect match - mark as prefix
            if (child->is_prefix && child->mask == mask) {
                return 0; // Already exists
            }
            child->is_prefix = true;
            child->mask = mask;
            return 0;
        }

        if (match_bits == child->skip) {
            // Full match with child's skip - continue traversal
            bit_pos += child->skip;
            current = child;
            continue;
        }
        // Partial match - need to split the node
        radix_node_t *split = create_node();
        if (split == NULL) {
            return -1;
        }

        // Split node contains matched portion
        split->skip = match_bits;
        split->prefix = extract_bits(base, bit_pos, match_bits);

        // Adjust child node
        int child_remaining = child->skip - match_bits;
        unsigned int child_new_prefix =
            child->prefix & ((1U << child_remaining) - 1);

        child->skip = child_remaining;
        child->prefix = child_new_prefix;

        // Determine where child goes under split
        int child_bit = (child->prefix >> (child_remaining - 1)) & 1;

        if (child_bit == 0) {
            split->left = child;
        } else {
            split->right = child;
        }

        // Insert split into tree
        *child_ptr = split;

        // Check if we need to add new branch
        int new_remaining = remaining - match_bits;
        if (new_remaining == 0) {
            // Our prefix ends at split point
            split->is_prefix = true;
            split->mask = mask;
        } else {
            // Create new branch for remaining bits
            radix_node_t *new_branch = create_node();
            if (new_branch == NULL) {
                return -1;
            }

            new_branch->skip = new_remaining;
            new_branch->prefix =
                extract_bits(base, bit_pos + match_bits, new_remaining);
            new_branch->is_prefix = true;
            new_branch->mask = mask;

            int new_bit = get_bit(base, bit_pos + match_bits);
            if (new_bit == 0) {
                split->left = new_branch;
            } else {
                split->right = new_branch;
            }
        }
        return 0;
    }

    return 0;
}

int del(unsigned int base, char mask) {
    if (!is_valid_mask(mask)) {
        return -1;
    }

    if (!is_aligned(base, mask)) {
        return -1;
    }

    if (g_root == NULL) {
        return -1;
    }

    // Special case: /0 prefix
    if (mask == 0) {
        g_root->is_prefix = false;
        g_root->mask = -1;
        return 0;
    }

    // Traverse to find the prefix
    radix_node_t *current = g_root;
    int bit_pos = 0;

    while (bit_pos < mask) {
        int bit = get_bit(base, bit_pos);
        radix_node_t *child = (bit == 0) ? current->left : current->right;

        if (child == NULL) {
            return 0; // Prefix doesn't exist
        }

        // Check path match
        int remaining = mask - bit_pos;
        if (child->skip > remaining) {
            return 0; // Can't reach this prefix
        }

        unsigned int base_bits = extract_bits(base, bit_pos, child->skip);
        if (base_bits != child->prefix) {
            return 0; // Path doesn't match
        }

        bit_pos += child->skip;

        if (bit_pos == mask) {
            // Found the target node
            if (!child->is_prefix || child->mask != mask) {
                return 0; // Prefix wasn't set
            }

            // Mark as deleted
            child->is_prefix = false;
            child->mask = -1;

            return 0;
        }

        current = child;
    }

    return 0;
}

char check(unsigned int ip) {
    if (g_root == NULL) {
        return -1;
    }

    radix_node_t *current = g_root;
    char best_match = -1;

    // Check root
    if (current->is_prefix) {
        best_match = current->mask;
    }
    int bit_pos = 0;
    while (bit_pos < 32) {
        int bit = get_bit(ip, bit_pos);
        radix_node_t *child = (bit == 0) ? current->left : current->right;

        if (child == NULL) {
            break;
        }

        // Check if IP matches the compressed path
        unsigned int ip_bits = extract_bits(ip, bit_pos, child->skip);
        if (ip_bits != child->prefix) {
            break; // Path doesn't match
        }

        bit_pos += child->skip;
        current = child;

        // Update best match
        if (current->is_prefix) {
            best_match = current->mask;
        }
    }

    return best_match;
}

radix_node_t *get_root_addr(void) { return g_root; }

int prefix_mgmt_init(void) {
    if (g_root != NULL) {
        prefix_mgmt_cleanup();
    }

    g_root = create_node();
    if (g_root == NULL) {
        return -1;
    }

    return 0;
}

void prefix_mgmt_cleanup(void) {
    if (g_root != NULL) {
        free_node(g_root);
        g_root = NULL;
    }
}
