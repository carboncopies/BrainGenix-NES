#include <SliceGenerator/Structs/VoxelArray.h>




namespace BG {
namespace NES {
namespace Simulator {



VoxelArray::VoxelArray(int _SizeX, int _SizeY, int _SizeZ) {

    // Malloc array
    Data_ = new VoxelType[_SizeX][_SizeY][_SizeZ];

    // Setup Dimensions
    SizeX_ = _SizeX;
    SizeY_ = _SizeY;
    SizeZ_ = _SizeZ;
}



VoxelArray::~VoxelArray() {

    delete[] Data_;
}



}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG
