#!/bin/bash

# Enter Artifact/Binary Dir
cd ..
cd Binaries

# Activate the Python virtual environment
source ../venv/bin/activate

if [ "$(uname)" = "Darwin" ]; then
    HOMEBREW_PREFIX=$(brew --prefix 2>/dev/null || echo "/opt/homebrew")
    MOLTENVK_ICD="$HOMEBREW_PREFIX/etc/vulkan/icd.d/MoltenVK_icd.json"

    if [ -f "$MOLTENVK_ICD" ]; then
        export VK_ICD_FILENAMES="$MOLTENVK_ICD"
    else
        echo "Warning: MoltenVK ICD not found at $MOLTENVK_ICD. VSDA rendering may be disabled."
    fi

    if [ -d "$HOMEBREW_PREFIX/lib" ]; then
        export DYLD_LIBRARY_PATH="$HOMEBREW_PREFIX/lib${DYLD_LIBRARY_PATH:+:$DYLD_LIBRARY_PATH}"
    fi
fi

# Run the application
./BrainGenix-NES $1 $2 $3 $4
