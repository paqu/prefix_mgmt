# Code Analysis Tools for IPv4 Prefix Management System

Comprehensive toolkit for analyzing C code quality, memory safety, and correctness.

## Quick Start

### Using Docker

```bash
# Build and enter Docker container
chmod +x run_docker.sh
./run_docker.sh

# in Docker image
./quick_check 'tool_name'
```

## Available Tools

### 1. **Cppcheck** - Static Code Analysis

Checks for:

- Memory leaks
- Buffer overflows
- Null pointer dereferences
- Uninitialized variables
- Style issues

```bash
./quick_check.sh cppcheck
```

### 2. **Clang-Tidy** - Advanced Linter

Checks for:

- Bug-prone patterns
- Performance issues
- Readability problems
- CERT C compliance
- Modernization opportunities

```bash
./quick_check.sh clang-tidy
```

### 3. **AddressSanitizer (ASan)** - Memory Error Detection

Detects:

- Heap buffer overflows
- Stack buffer overflows
- Use after free
- Use after return
- Memory leaks

```bash
./quick_check.sh asan
```

### 4. **UndefinedBehaviorSanitizer (UBSan)** - UB Detection

Detects:

- Integer overflows
- Null pointer dereferences
- Misaligned access
- Division by zero
- Invalid shifts

```bash
./quick_check.sh ubsan
```

### 5. **Valgrind** - Memory Profiler

Detects:

- Memory leaks (definitely/possibly lost)
- Invalid memory access
- Uninitialized values
- Invalid frees

```bash
./quick_check.sh valgrind
```

### 6. **Compiler Testing**

Test with GCC:

```bash
./quick_check.sh gcc
```

Test with Clang:

```bash
./quick_check.sh clang
```

Check current compiler:

```bash
./quick_check.sh compiler
```
