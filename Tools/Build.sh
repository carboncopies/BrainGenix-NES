#!/bin/bash

# Get Into Root Repo Dir
echo "[BG BUILD HELPER] - Entering Root Repo Directory"
cd ..


# Configure Build Type
BuildType="Debug"
if (($# >= 2))
then
    BuildType=$2
    echo "[BG BUILD HELPER] - Building in user-specified $BuildType mode."
else
    echo "[BG BUILD HELPER] - Did not get specified build configuration, building in $BuildType mode."
fi

# Delete Binary If Exists 
echo "[BG BUILD HELPER] - Checking If Binary Exists"
if [ -f "Binaries/BrainGenix-NES" ]
then
    echo "[BG BUILD HELPER] - Removing Last Executable"
    rm Binaries/BrainGenix-NES
else
    echo "[BG BUILD HELPER] - No Binaries To Clean"
fi

# Check If Build Type Correct
if grep -q $BuildType "Build/BuildType"
then
    echo "[BG BUILD HELPER] - Detected Matching Build Type"
else
    echo "[BG BUILD HELPER] - Build Type Mismatch, Cleaning First"
    rm -rf Build/
fi


# Check If Configuration Needs To Be Run
echo "[BG BUILD HELPER] - Checking If Build Directory Already Exists"
if [ -d "Build" ]
then 
    echo "[BG BUILD HELPER] - Build Directory Already Exists, Skipping Generation"
    cd Build
else 
    # Create Build Dir
    echo "[BG BUILD HELPER] - Creating Build Directory"
    mkdir -p Build

    # Enter Build Dir
    echo "[BG BUILD HELPER] - Entering Build Directory"
    cd Build

    # Make Only BrainGenix-NES
    echo "[BG BUILD HELPER] - Configuring Build Files"
    cmake .. -D CMAKE_BUILD_TYPE=$BuildType

    # Set Config Var
    echo "[BG BUILD HELPER] - Saving Build Type Configuration Of $BuildType"
    echo "$BuildType" > "BuildType"
fi


# Build Files
echo "[BG BUILD HELPER] - Building, Please Wait. This may take some time"
cmake --build . -j $1

# Return status code of build (0=success, else=fail)
# exit $?

# Return to Tools directory
cd ../Tools
