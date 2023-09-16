
# Enter Artifact/Binary Dir
cd ..
cd Binaries

# Depending On Platform, Run
valgrind --leak-check=full --show-leak-kinds=all ./BrainGenix-NES $1 $2 $3 $4
