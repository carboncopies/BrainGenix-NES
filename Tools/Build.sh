# Get Into Root Repo Dir
echo "[ERS BUILD HELPER] - Entering Root Repo Directory"
cd ..


# Delete Binary If Exists 
echo "[ERS BUILD HELPER] - Checking If Binary Exists"
if [ -f "Binaries/BrainGenix-ERS" ]
then
    echo "[ERS BUILD HELPER] - Removing Last Executable"
    rm Binaries/BrainGenix-ERS
else
    echo "[ERS BUILD HELPER] - No Binaries To Clean"
fi


# Check If Configuration Needs To Be Run
echo "[ERS BUILD HELPER] - Checking If Build Directory Already Exists"
if [ -d "Build" ]
then 
    echo "[ERS BUILD HELPER] - Build Directory Already Exists, Skipping Generation"
    cd Build
else 
    # Create Build Dir
    echo "[ERS BUILD HELPER] - Creating Build Directory"
    mkdir -p Build

    # Enter Build Dir
    echo "[ERS BUILD HELPER] - Entering Build Directory"
    cd Build

    # Make Only BrainGenix-ERS
    echo "[ERS BUILD HELPER] - Configuring Build Files"
    cmake ..
fi


# Build Files
echo "[ERS BUILD HELPER] - Building, Please Wait. This may take some time"
cmake --build . --target BrainGenix-ERS -j $1


# Run Program
cd ../Binaries
echo "[ERS BUILD HELPER] - Checking If Binary Exists"
if [ -f "BrainGenix-ERS" ]
then
    echo "[ERS BUILD HELPER] - Running Program"
    ./BrainGenix-ERS
else
    echo "[ERS BUILD HELPER] - Build Failed! Exiting"
fi
cd ..
