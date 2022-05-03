#!/bin/bash

# Make Docs Generated Folder
echo "Making Generated Docs Folder"
cd ..
mkdir -p Generated

# Generate Docs
echo "Generating Documentation For BrainGenix-ERS"
cd Source
doxygen Doxyfile
