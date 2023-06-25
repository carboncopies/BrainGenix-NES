# Install Compiler Tools
COMPILER_DEPS="git wget cmake g++"

# vcpkg Dependencies
VCPKG_DEPS="curl zip unzip tar pkg-config autoconf flex bison"

# Install Everything
INSTALL_COMMAND="sudo apt install $VCPKG_DEPS $COMPILER_DEPS -y"
echo "Running Install Command: $INSTALL_COMMAND"
$INSTALL_COMMAND

# Update Submodules
echo "Updating Submodules"
git submodule update --init

# Bootstrap vcpkg
echo "Setting Up vcpkg"
./../ThirdParty/vcpkg/bootstrap-vcpkg.sh
echo "Done, you can now run Build.sh"
