# Install Compiler Tools
sudo apt install git wget cmake ninja-build g++ python3 python3-pip -y

# Backward Deps
sudo apt install binutils-dev libunwind-dev libdwarf-dev libdw-dev -y

# Install GLFW Deps
sudo apt install libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev -y

# Install MySQL Connector Deps
sudo apt install libssl-dev -y

# Install Monado Deps
pip3 install numpy
sudo apt install libusb-1.0-0-dev glslang-dev glslang-tools libeigen3-dev libudev-dev libgl1-mesa-dev -y

# Warn about openxr drivers
echo "-------------------------------------"
echo "YOU NEED TO INSTALL AN OPENXR RUNTIME TO USE VR! IF YOU ALREADY HAVE ONE INSTALLED, YOU CAN IGNORE THIS BUT OTHERWISE YOU NEED TO INSTALL ONE. Please Use The Associated Scripts (Monado) For That."
echo "-------------------------------------"