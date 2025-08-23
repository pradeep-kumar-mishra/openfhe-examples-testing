#!/bin/bash
set -e

image_name="fhe-examples"
CLEAN_BUILD=false
BUILD_IMAGE=true

# Get host user info
HOST_UID=$(id -u)
HOST_GID=$(id -g)

# Script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
echo "Script directory: ${SCRIPT_DIR}"

# System RAM in GB
TOTAL_RAM_GB=$(free -g | awk '/Mem:/ {print $2}')
TOTAL_RAM_GB=${TOTAL_RAM_GB:-16}

# Parse arguments
while [[ $# -gt 0 ]]; do
  case "$1" in
    -n|--name)
      image_name="$2"
      shift 2
      ;;
    -clean|--clean)
      CLEAN_BUILD=true
      shift
      ;;
    -h|--help)
      echo "Usage: $0 [-n|--name IMAGE_NAME] [-clean|--clean]"
      exit 0
      ;;
    *)
      echo "Unknown option: $1"
      echo "Use -h or --help for usage."
      exit 1
      ;;
  esac
done

# Build image
echo "🔧 Building Docker image: $image_name"
if [ "$CLEAN_BUILD" = true ]; then
  docker build \
    --no-cache \
    --build-arg USER_UID=${HOST_UID} \
    --build-arg USER_GID=${HOST_GID} \
    -t "$image_name" .
else
  docker build \
    --build-arg USER_UID=${HOST_UID} \
    --build-arg USER_GID=${HOST_GID} \
    -t "$image_name" .
fi

# Run container
echo "🚀 Starting container from image: $image_name"
docker run \
  --workdir /fhe-examples \
  -it --rm \
  --shm-size="${TOTAL_RAM_GB}g" \
  -v "$PWD":/fhe-examples \
  -e USER_UID=${HOST_UID} \
  -e USER_GID=${HOST_GID} \
  "$image_name" \
  bash
