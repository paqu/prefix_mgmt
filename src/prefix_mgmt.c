#include "prefix_mgmt/prefix_mgmt.h"
#include <stdbool.h>

/**
 * @file prefix_mgmt.c
 * @brief Implementation of IPv4 Prefix Management System functions.
 *
 */

/**
 * @brief Validate mask
 */
static bool is_valid_mask(char mask) { return mask >= 0 && mask <= 32; }

int add(unsigned int base, char mask) {
    if (!is_valid_mask(mask)) {
        return -1;
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
