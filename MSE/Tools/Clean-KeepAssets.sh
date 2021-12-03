# Cleans the build directories while keeping existing assets
echo "Cleaning Build (Keeping Assets)"

cd ..
rm -rf Build/
rm -rf build/
rm Binaries/*  #Note: The lack of -rf means it will only delete files and not any folders


echo "Build Cleaned"