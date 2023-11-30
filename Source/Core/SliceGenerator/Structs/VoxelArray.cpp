#include <SliceGenerator/Structs/VoxelArray.h>




namespace BG {
namespace NES {
namespace Simulator {



VoxelArray::VoxelArray(int _SizeX, int _SizeY, int _SizeZ) {

    // Malloc array
    Data_ = new VoxelType[_SizeX * _SizeY * _SizeZ];

    // Setup Dimensions
    SizeX_ = _SizeX;
    SizeY_ = _SizeY;
    SizeZ_ = _SizeZ;


    // Reset the array so we don't get a bunch of crap in it
    ClearArray();
}



VoxelArray::~VoxelArray() {

    delete[] Data_;
}


void VoxelArray::ClearArray() {

    // Reset everything to 0s
    for (uint64_t i = 0; i < SizeX_ * SizeY_ * SizeZ_; i++) {
        Data_[i] = 0;
    }

}

int VoxelArray::GetIndex(int _X, int _Y, int _Z) {
    return _X*(SizeY_*SizeZ_) + _Y*SizeZ_ + _Z;
}

VoxelType VoxelArray::GetVoxel(int _X, int _Y, int _Z) {

    // Hope this works
    return Data_[GetIndex(_X, _Y, _Z)];

}

void VoxelArray::SetVoxel(int _X, int _Y, int _Z, VoxelType _Value) {
    Data_[GetIndex(_X, _Y, _Z)] = _Value;
}

void VoxelArray::GetSize(int* _X, int* _Y, int* _Z) {
    (*_X) = SizeX_;
    (*_Y) = SizeY_;
    (*_Z) = SizeZ_;
}

}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG
