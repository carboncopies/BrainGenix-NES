# Cleans the build directories
echo "[BG BUILD SYSTEM] - Cleaning Build"

cd ..
rm -rf Build/
rm -rf build/
rm -rf Binaries/
rm -rf venv/

echo "[BG BUILD SYSTEM] - Build Cleaned"

# Return to Tools directory
cd Tools
