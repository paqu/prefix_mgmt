#include "prefix_mgmt/prefix_mgmt.h"
#include <stdbool.h>
#include <stdlib.h>

/**
 * @file prefix_mgmt.c
 * @brief Implementation of IPv4 Prefix Management System functions.
 *
 */

static radix_node_t *g_root = NULL;

radix_node_t *get_root_addr(void) { return g_root; }

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

static void free_node(radix_node_t *node) {
    if (node == NULL) {
        return;
    }
    free_node(node->left);
    free_node(node->right);
    free(node);
}

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

static int get_bit(unsigned int ip, int bit_pos) {
    return (ip >> (31 - bit_pos)) & 1;
}

static unsigned int extract_bits(unsigned int ip, int start_bit, int num_bits) {
    if (num_bits == 0)
        return 0;
    if (num_bits == 32 && start_bit == 0)
        return ip;
    unsigned int mask = (1U << num_bits) - 1;
    return (ip >> (32 - start_bit - num_bits)) & mask;
}

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

/**
 * @brief Validate mask
 */
static bool is_valid_mask(char mask) { return mask >= 0 && mask <= 32; }

/**
 * @brief Check base alignment
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

    // Initialize if needed
    if (g_root == NULL) {
        if (prefix_mgmt_init() != 0) {
            return -1;
        }
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
    // TODO: Implement prefix deletion logic
    return 0;
}

char check(unsigned int ip) {
    // TODO: Implement prefix containment check logic
    return -1;
}
