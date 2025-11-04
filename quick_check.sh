#!/bin/bash

# Quick analysis script - run specific tool
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build_quick"
TOOL="$1"

usage() {
    echo "Usage: $0 <tool>"
    echo ""
    echo "Available tools:"
    echo "  cppcheck   - Static analysis"
    echo "  valgrind   - Memory leak detection"
    echo "  asan       - Address sanitizer"
    echo "  ubsan      - Undefined behavior sanitizer"
    echo "  clang-tidy - Clang-based linter"
    echo "  gcc        - Build with GCC"
    echo "  clang      - Build with Clang"
    echo "  compiler   - Show current compiler"
    exit 1
}

if [ -z "$TOOL" ]; then
    usage
fi

case "$TOOL" in
    compiler)
        echo "=== Current Compiler Detection ==="
        if [ -f "${BUILD_DIR}/CMakeCache.txt" ]; then
            echo "CMake cache found:"
            grep "CMAKE_C_COMPILER:" "${BUILD_DIR}/CMakeCache.txt"
            grep "CMAKE_CXX_COMPILER:" "${BUILD_DIR}/CMakeCache.txt"
        else
            echo "No CMake cache - using defaults"
        fi
        echo ""
        echo "System compilers:"
        gcc --version | head -n1
        clang --version | head -n1
        ;;
        
    cppcheck)
        echo "=== Running Cppcheck ==="
        cppcheck \
            --enable=all \
            --suppress=missingIncludeSystem \
            --std=c99 \
            --language=c \
            -I "${SCRIPT_DIR}/include" \
            "${SCRIPT_DIR}/src"
        ;;
        
    valgrind)
        echo "=== Running Valgrind ==="
        rm -rf "${BUILD_DIR}"
        mkdir -p "${BUILD_DIR}"
        cd "${BUILD_DIR}"
        cmake -DCMAKE_BUILD_TYPE=Debug \
              -DCMAKE_C_FLAGS="-g -O0" \
              -DBUILD_TESTS=ON \
              ..
        make -j$(nproc)
        valgrind --leak-check=full --show-leak-kinds=all ./tests/test_runner
        ;;
        
    asan)
        echo "=== Running Address Sanitizer ==="
        rm -rf "${BUILD_DIR}"
        mkdir -p "${BUILD_DIR}"
        cd "${BUILD_DIR}"
        cmake -DCMAKE_BUILD_TYPE=Debug \
              -DCMAKE_C_FLAGS="-fsanitize=address -g" \
              -DCMAKE_CXX_FLAGS="-fsanitize=address -g" \
              -DBUILD_TESTS=ON \
              ..
        make -j$(nproc)
        ./tests/test_runner
        ;;
        
    ubsan)
        echo "=== Running Undefined Behavior Sanitizer ==="
        rm -rf "${BUILD_DIR}"
        mkdir -p "${BUILD_DIR}"
        cd "${BUILD_DIR}"
        cmake -DCMAKE_BUILD_TYPE=Debug \
              -DCMAKE_C_FLAGS="-fsanitize=undefined -g" \
              -DCMAKE_CXX_FLAGS="-fsanitize=undefined -g" \
              -DBUILD_TESTS=ON \
              ..
        make -j$(nproc)
        ./tests/test_runner
        ;;
        
    clang-tidy)
        echo "=== Running Clang-Tidy ==="
        rm -rf "${BUILD_DIR}"
        mkdir -p "${BUILD_DIR}"
        cd "${BUILD_DIR}"
        cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
              -DCMAKE_C_COMPILER=clang \
              -DCMAKE_CXX_COMPILER=clang++ \
              -DBUILD_TESTS=ON \
              ..
        clang-tidy -p . "${SCRIPT_DIR}/src/prefix_mgmt.c"
        ;;
        
    gcc)
        echo "=== Building with GCC ==="
        rm -rf "${BUILD_DIR}"
        mkdir -p "${BUILD_DIR}"
        cd "${BUILD_DIR}"
        cmake -DCMAKE_C_COMPILER=gcc \
              -DCMAKE_CXX_COMPILER=g++ \
              -DBUILD_TESTS=ON \
              ..
        make -j$(nproc)
        ./tests/test_runner
        ;;
        
    clang)
        echo "=== Building with Clang ==="
        rm -rf "${BUILD_DIR}"
        mkdir -p "${BUILD_DIR}"
        cd "${BUILD_DIR}"
        cmake -DCMAKE_C_COMPILER=clang \
              -DCMAKE_CXX_COMPILER=clang++ \
              -DBUILD_TESTS=ON \
              ..
        make -j$(nproc)
        ./tests/test_runner
        ;;
        
    *)
        echo "Unknown tool: $TOOL"
        usage
        ;;
esac
