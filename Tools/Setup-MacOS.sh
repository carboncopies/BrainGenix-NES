#!/bin/bash

# Setup MacOS Environment via Homebrew
echo "Setting Up Packages Needed For Compilation on MacOS"

if ! command -v brew >/dev/null 2>&1; then
    echo "Homebrew is required on macOS. Install it from https://brew.sh/ and rerun this script."
    exit 1
fi

# Ensure Homebrew is updated
brew update

# Define Dependencies
COMPILER_DEPS="git wget cmake autoconf flex bison pkg-config"
VCPKG_DEPS="curl zip unzip"
VULKAN_DEPS="molten-vk vulkan-headers vulkan-loader vulkan-tools vulkan-validationlayers vulkan-utility-libraries"
PYTHON_DEPS="python3"

# Install Dependencies
INSTALL_COMMAND="brew install $VCPKG_DEPS $COMPILER_DEPS $VULKAN_DEPS $PYTHON_DEPS"
echo "Running Install Command: $INSTALL_COMMAND"
$INSTALL_COMMAND

# Create Python virtual environment and install igneous-pipeline
echo "Creating Python virtual environment..."
cd ..
python3 -m venv venv || exit 1
echo "Installing igneous-pipeline in the virtual environment..."
./venv/bin/pip install igneous-pipeline || exit 1
cd Tools

# Update Submodules
echo "Updating Submodules"
git submodule update --init --recursive

# Apply vcpkg patches
echo "Applying macOS patches to vcpkg..."
cd ../ThirdParty/vcpkg
if git apply --reverse --check ../../Tools/Patches/vcpkg/vcpkg-macos.patch >/dev/null 2>&1; then
    echo "macOS vcpkg patches already applied"
elif git apply --check ../../Tools/Patches/vcpkg/vcpkg-macos.patch; then
    git apply ../../Tools/Patches/vcpkg/vcpkg-macos.patch
else
    echo "Unable to apply macOS vcpkg patches"
    exit 1
fi
cd ../../Tools

# Bootstrap vcpkg
echo "Setting Up vcpkg"
./../ThirdParty/vcpkg/bootstrap-vcpkg.sh -disableMetrics
echo "Done, you can now run Build.sh"
