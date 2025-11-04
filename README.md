# IPv4 Prefix Management System

A system for managing a collection of IPv4 prefixes implemented in C language.

## Project Structure

```
project/
├── src/                # Source files (.c)
├── include/            # Header files (.h)
├── tests/              # Tests (Google Test)
├── docs/               # Doxygen documentation
├── build/              # Build directory (generated)
├── CMakeLists.txt      # CMake configuration
└── LICENSE.txt         # License terms
```

## Requirements

- CMake 3.16 or higher
- C99 compiler (GCC, Clang)
- C++17 compiler (for tests)
- Google Test (automatically downloaded)
- Doxygen (optional, for documentation)
- lcov/genhtml (optional, for coverage)

## Building the Project

### Basic Build

```bash
mkdir build
cd build
cmake ..
make
```

### Build Types

```bash
# Debug build (default)
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Release build (optimized)
cmake -DCMAKE_BUILD_TYPE=Release ..

# Build without tests
cmake -DBUILD_TESTS=OFF ..

# Build with documentation
cmake -DBUILD_DOCS=ON ..
```

## Running Tests

After building the project:

```bash
cd build
ctest --output-on-failure
```

Or run the test executable directly:

```bash
./build/tests/test_runner
```

## Generating Documentation

Make sure Doxygen is installed on your system.

```bash
mkdir build
cd build
cmake -DBUILD_DOCS=ON ..
make docs
```

The documentation will be generated in `build/docs/doxygen/html/`.
Open `index.html` in your browser to view it.

## Generating Coverage Report

Build the project with coverage enabled:

```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON ..
make
make coverage
```

The coverage report will be generated in `build/coverage_html/`.
Open `index.html` to view the results.

To clean coverage data:

```bash
make coverage-clean
```

## API Usage

### Initialize the system

```c
#include "prefix_mgmt/prefix_mgmt.h"

int main() {
    // Initialize
    if (prefix_mgmt_init() != 0) {
        return -1;
    }

    // Add prefix: 10.0.0.0/8
    add(0x0A000000, 8);

    // Check if IP is in collection
    char mask = check(0x0A0A0A0A);  // Returns 8

    // Delete prefix
    del(0x0A000000, 8);

    // Cleanup
    prefix_mgmt_cleanup();

    return 0;
}
```

## Licensing

This software is proprietary and protected by copyright. See License.txt.

## Contact

**Author:** Paweł Kusz  
**Company:** Pakusoft  
**Email:** pawel.kusz1993@gmail.com

## Copyright

Copyright (c) 2025 Paweł Kusz / Pakusoft. All rights reserved.
