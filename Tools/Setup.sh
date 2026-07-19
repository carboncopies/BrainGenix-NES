#!/bin/bash

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
VENV_DIR="$REPO_ROOT/venv"

run_cmd() {
    echo "Running: $*"
    "$@"
}

ensure_sudo() {
    if [ "${EUID}" -eq 0 ]; then
        return 0
    fi

    if command -v sudo >/dev/null 2>&1; then
        sudo -v
        return 0
    fi

    echo "This setup path needs elevated package installation privileges, but sudo is unavailable."
    echo "Install the required packages manually and rerun the script."
    exit 1
}

install_linux_packages() {
    local distro
    distro="$(. /etc/os-release && printf '%s' "${ID:-}")"

    ensure_sudo

    case "$distro" in
        ubuntu|debian)
            run_cmd sudo apt update
            run_cmd sudo apt install -y \
                git wget cmake g++ ninja-build \
                curl zip unzip tar pkg-config autoconf autoconf-archive automake libtool flex bison \
                python3 python3-pip python3-venv \
                libvulkan-dev vulkan-tools libxcb-xfixes0-dev libx11-dev libxrandr-dev \
                libxi-dev libxinerama-dev libxcursor-dev
            ;;
        fedora)
            run_cmd sudo dnf install -y \
                git wget cmake gcc-c++ ninja-build \
                curl zip unzip tar pkgconf-pkg-config autoconf autoconf-archive automake libtool flex bison \
                python3 python3-pip \
                vulkan-loader-devel vulkan-tools vulkan-validation-layers-devel \
                libX11-devel libXrandr-devel libXcursor-devel libXi-devel libXinerama-devel libxcb-devel
            ;;
        arch)
            run_cmd sudo pacman -Sy --noconfirm \
                git wget cmake gcc ninja \
                curl zip unzip tar pkgconf autoconf autoconf-archive automake libtool flex bison \
                python \
                vulkan-headers vulkan-tools vulkan-validation-layers \
                libx11 libxrandr libxcursor libxi libxinerama libxcb
            ;;
        *)
            echo "Unsupported Linux distribution: ${distro:-unknown}"
            echo "Install compiler, Vulkan, Python venv, and vcpkg prerequisite packages manually, then rerun."
            exit 1
            ;;
    esac
}

install_macos_packages() {
    if ! command -v brew >/dev/null 2>&1; then
        echo "Homebrew is required on macOS. Install it from https://brew.sh/ and rerun this script."
        exit 1
    fi

    run_cmd brew install \
        curl zip unzip git wget cmake \
        autoconf autoconf-archive automake libtool flex bison pkg-config \
        molten-vk vulkan-headers vulkan-loader vulkan-tools vulkan-validationlayers vulkan-utility-libraries \
        python@3
}

setup_venv() {
    if [ ! -d "$VENV_DIR" ]; then
        echo "Creating Python virtual environment at $VENV_DIR"
        run_cmd python3 -m venv "$VENV_DIR"
    fi

    run_cmd "$VENV_DIR/bin/python" -m pip install --upgrade pip
    run_cmd "$VENV_DIR/bin/python" -m pip install igneous-pipeline graphifyy
}

echo "Entering repository root: $REPO_ROOT"
cd "$REPO_ROOT"

if [ "$(uname)" = "Darwin" ]; then
    echo "Detected macOS, using Homebrew setup path"
    install_macos_packages
else
    echo "Detected Linux, installing dependencies via system package manager"
    install_linux_packages
fi

setup_venv

echo "Updating submodules"
run_cmd git submodule sync --recursive
run_cmd git submodule update --init --recursive

# Netmorph's Apple Silicon fixes live in a dedicated, pinned revision. Keep the
# superproject's recorded cross-platform revision for every other platform so a
# Linux checkout cannot be changed by this setup path.
if [ "$(uname)" = "Darwin" ] && [ "$(uname -m)" = "arm64" ]; then
    NETMORPH_MAC_SILICON_COMMIT="5c26c4aa84ad08a7f8f2bba7d650c0581dba7ace"
    NETMORPH_DIR="$REPO_ROOT/ThirdParty/NetmorphCMake"

    echo "Selecting pinned Apple Silicon Netmorph revision: $NETMORPH_MAC_SILICON_COMMIT"
    run_cmd git -C "$NETMORPH_DIR" fetch origin mac-silicon-support
    run_cmd git -C "$NETMORPH_DIR" checkout --detach "$NETMORPH_MAC_SILICON_COMMIT"
fi

echo "Bootstrapping vcpkg"
run_cmd "$REPO_ROOT/ThirdParty/vcpkg/bootstrap-vcpkg.sh" -disableMetrics

echo "Done, you can now run Build.sh"
