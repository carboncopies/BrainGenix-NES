# Erase Existing Deb Builds From Artifacts Dir
rm ../Artifacts/*.deb

# Enter Artifact/Binary Dir
cd ..
cd Build

# Run CPack
cpack