#!/bin/bash

# Build and run analysis container
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
IMAGE_NAME="ipv4-prefix-analysis"
CONTAINER_NAME="ipv4-prefix-container"

echo "======================================"
echo "Building Docker image..."
echo "======================================"
docker build -t ${IMAGE_NAME} -f "${SCRIPT_DIR}/Dockerfile" "${SCRIPT_DIR}"

echo ""
echo "======================================"
echo "Starting container..."
echo "======================================"
docker run -it --rm \
    --name ${CONTAINER_NAME} \
    -v "${SCRIPT_DIR}:/workspace" \
    -w /workspace \
    ${IMAGE_NAME} \
    bash
