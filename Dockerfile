# Dockerfile for comprehensive C/C++ code analysis
FROM ubuntu:24.04

# Prevent interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Install all required tools
RUN apt-get update && apt-get install -y \
    # Compilers
    gcc \
    g++ \
    clang \
    clang-tools \
    clang-tidy \
    # Build tools
    cmake \
    make \
    ninja-build \
    # Analysis tools
    valgrind \
    cppcheck \
    # Additional tools
    git \
    curl \
    wget \
    lcov \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /workspace

# Default command
CMD ["/bin/bash"]
