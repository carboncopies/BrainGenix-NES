# Get Into Root Repo Dir
echo "[NES BUILD HELPER] - Entering Root Repo Directory"
cd ..


# Delete Binary If Exists 
echo "[NES BUILD HELPER] - Checking If Binary Exists"
if [ -f "Binaries/BrainGenix-NES" ]
then
    echo "[NES BUILD HELPER] - Removing Last Executable"
    rm Binaries/BrainGenix-NES
else
    echo "[NES BUILD HELPER] - No Binaries To Clean"
fi


# Check If Configuration Needs To Be Run
echo "[NES BUILD HELPER] - Checking If Build Directory Already Exists"
if [ -d "Build" ]
then 
    echo "[NES BUILD HELPER] - Build Directory Already Exists, Skipping Generation"
    cd Build
else 
    # Create Build Dir
    echo "[NES BUILD HELPER] - Creating Build Directory"
    mkdir -p Build

    # Enter Build Dir
    echo "[NES BUILD HELPER] - Entering Build Directory"
    cd Build

    # Make Only BrainGenix-NES
    echo "[NES BUILD HELPER] - Configuring Build Files"
    cmake ..
fi


# Build Files
echo "[NES BUILD HELPER] - Building, Please Wait. This may take some time"
cmake --build . -j $1
make test

# # Run Program
# cd ../Binaries
# echo "[NES BUILD HELPER] - Checking If Binary Exists"
# if [ -f "BrainGenix-NES" ]
# then
#     echo "[NES BUILD HELPER] - Running Tests"
#     ./BrainGenix-NES
# else
#     echo "[NES BUILD HELPER] - Build Failed! Exiting"
# fi
# cd ..
