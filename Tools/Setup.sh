#!/bin/bash

# Detect if running as root
SUDO_PREFIX=""
echo "Detecting If Script Running As root"
if [ "$EUID" -ne 0 ]; then
    echo "Detected Install Script Is Not Running As Root, Adding 'sudo' Prefix To Commands"
    SUDO_PREFIX="sudo"
else
    echo "Command Running As Root, Running Commands Without 'sudo'"
fi

# Setting up package manager depending on Linux Distro
DISTRO=$(cat /etc/os-release | grep -w ID | awk -F'=' '{print $2}')
PACKAGE_MANAGER_INSTALL_CMD=""
PACKAGE_MANAGER_UPDATE_CMD=""

case $DISTRO in
    "arch") PACKAGE_MANAGER_INSTALL_CMD="pacman -S --noconfirm";
            PACKAGE_MANAGER_UPDATE_CMD="pacman -Sy --noconfirm";;
    "ubuntu") PACKAGE_MANAGER_INSTALL_CMD="apt install -y";
              PACKAGE_MANAGER_UPDATE_CMD="apt update -y";;
    "fedora") PACKAGE_MANAGER_INSTALL_CMD="dnf install -y";
              PACKAGE_MANAGER_UPDATE_CMD="dnf update -y";;
    *) PACKAGE_MANAGER_INSTALL_CMD="apt install -y";
       PACKAGE_MANAGER_UPDATE_CMD="apt update -y";;
esac

# Setup, update package registry
echo "Setting Up Packages Needed For Compilation"
# $SUDO_PREFIX $PACKAGE_MANAGER_UPDATE_CMD || exit 1
$SUDO_PREFIX $PACKAGE_MANAGER_UPDATE_CMD


# Install Compiler Tools
COMPILER_DEPS="git wget cmake"

if [ "$DISTRO" = "arch" ]; then
    COMPILER_DEPS="$COMPILER_DEPS gcc"
else
    COMPILER_DEPS="$COMPILER_DEPS g++"   
fi

# vcpkg Dependencies
VCPKG_DEPS="curl zip unzip tar pkg-config autoconf flex bison"

# Vulkan Dependencies
if [ "$DISTRO" = "arch" ]; then
    VULKAN_DEPS="vulkan-devel"
elif [ "$DISTRO" = "fedora" ]; then
    VULKAN_DEPS="libvulkan-dev vulkan-validationlayers-dev vulkan-tools libxcb-xfixes0-dev libx11-dev libxrandr-dev"  
else
    VULKAN_DEPS="libvulkan-dev vulkan-validationlayers-dev vulkan-tools libxcb-xfixes0-dev libx11-dev libxrandr-dev"  
fi

# Install Everything
INSTALL_COMMAND="$SUDO_PREFIX $PACKAGE_MANAGER_INSTALL_CMD $VCPKG_DEPS $COMPILER_DEPS $VULKAN_DEPS"
echo "Running Install Command: $INSTALL_COMMAND"
# $INSTALL_COMMAND || exit 1
$INSTALL_COMMAND

# Update Submodules
echo "Updating Submodules"
git submodule update --init

# Bootstrap vcpkg
echo "Setting Up vcpkg"
./../ThirdParty/vcpkg/bootstrap-vcpkg.sh -disableMetrics
echo "Done, you can now run Build.sh"
