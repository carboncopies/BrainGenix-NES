# Enter Artifact/Binary Dir
cd ..
cd Binaries

# Set environment to use the Python virtual environment
export PYTHONHOME=../venv
export PATH="../venv/bin:$PATH"

# Depending On Platform, Run
# export VK_INSTANCE_LAYERS=VK_LAYER_LUNARG_api_dump
./BrainGenix-NES $1 $2 $3 $4