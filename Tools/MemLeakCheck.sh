
# Enter Artifact/Binary Dir
cd ..
cd Binaries

# Depending On Platform, Run
export VK_ICD_FILENAMES=/usr/share/vulkan/icd.d/nvidia_icd.json
valgrind --leak-check=full --show-leak-kinds=all ./BrainGenix-NES $1 $2 $3 $4
