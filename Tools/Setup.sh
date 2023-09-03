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
    "arch") PACKAGE_MANAGER_INSTALL_CMD="pacman -S";
            PACKAGE_MANAGER_UPDATE_CMD="pacman -Syu";;
    "ubuntu") PACKAGE_MANAGER_INSTALL_CMD="apt install -y";
              PACKAGE_MANAGER_UPDATE_CMD="apt update";;
    "fedora") PACKAGE_MANAGER_INSTALL_CMD="dnf install -y";
              PACKAGE_MANAGER_UPDATE_CMD="dnf update";;
    *) PACKAGE_MANAGER_INSTALL_CMD="apt install";
       PACKAGE_MANAGER_UPDATE_CMD="dnf update";;
esac

# Setup, update package registry
echo "Setting Up Packages Needed For Compilation"
$SUDO_PREFIX $PACKAGE_MANAGER_UPDATE_CMD || exit 1


# Install Compiler Tools
COMPILER_DEPS="git wget cmake g++"

# vcpkg Dependencies
VCPKG_DEPS="curl zip unzip tar pkg-config autoconf flex bison"

# Install Everything
INSTALL_COMMAND="$SUDO_PREFIX $PACKAGE_MANAGER_INSTALL_CMD $VCPKG_DEPS $COMPILER_DEPS"
echo "Running Install Command: $INSTALL_COMMAND"
# $INSTALL_COMMAND || exit 1
$INSTALL_COMMAND

# Update Submodules
echo "Updating Submodules"
git submodule update --init

# Bootstrap vcpkg
echo "Setting Up vcpkg"
./../ThirdParty/vcpkg/bootstrap-vcpkg.sh
echo "Done, you can now run Build.sh"
