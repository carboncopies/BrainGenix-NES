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

# Detect version
Var=$(lsb_release -r)
NumOnly=$(cut -f2 <<< "$Var")


# Define Python dependencies based on distro
PYTHON_DEPS=""
case $DISTRO in
    "arch") PYTHON_DEPS="python" ;;  # Arch includes venv in the python package
    "ubuntu") PYTHON_DEPS="python3 python3-venv" ;;
    "fedora") PYTHON_DEPS="python3 python3-virtualenv" ;;
    *) PYTHON_DEPS="python3 python3-venv" ;;
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
    # Check if it's ubuntu 24.04 since they did some dumb shit and changed the library name
    if [ "$NumOnly" = "24.04" ]; then
        VULKAN_DEPS="libvulkan-dev vulkan-utility-libraries-dev vulkan-tools libxcb-xfixes0-dev libx11-dev libxrandr-dev"  
    else
        VULKAN_DEPS="libvulkan-dev vulkan-validationlayers-dev vulkan-tools libxcb-xfixes0-dev libx11-dev libxrandr-dev"  
    fi
fi


# Install Everything (add PYTHON_DEPS)
INSTALL_COMMAND="$SUDO_PREFIX $PACKAGE_MANAGER_INSTALL_CMD $VCPKG_DEPS $COMPILER_DEPS $VULKAN_DEPS $PYTHON_DEPS"
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
git submodule update --init
cd ../ThirdParty/NetmorphCMake && git pull origin main && cd ../../Tools

# Bootstrap vcpkg
echo "Setting Up vcpkg"
./../ThirdParty/vcpkg/bootstrap-vcpkg.sh -disableMetrics
echo "Done, you can now run Build.sh"
