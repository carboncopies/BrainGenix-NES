echo "This installs monado from source on all systems running <Ubuntu20.04. IF YOU ARE ON ANY HIGHER VERSION, DONT USE THIS SCRIPT!!"

sudo apt install build-essential git wget unzip cmake meson ninja-build libeigen3-dev curl patch python3 pkg-config libx11-dev libx11-xcb-dev libxxf86vm-dev libxrandr-dev libxcb-randr0-dev libvulkan-dev glslang-tools libglvnd-dev libgl1-mesa-dev ca-certificates libusb-1.0-0-dev libudev-dev libhidapi-dev libwayland-dev libuvc-dev libavcodec-dev libopencv-dev libv4l-dev libcjson-dev libsdl2-dev libegl1-mesa-dev

cd ~
git clone https://gitlab.freedesktop.org/monado/monado.git
cd monado
meson --prefix=/usr build
ninja -C build install
