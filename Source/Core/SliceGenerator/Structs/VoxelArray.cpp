#include <SliceGenerator/Structs/VoxelArray.h>




namespace BG {
namespace NES {
namespace Simulator {



VoxelArray::VoxelArray(Geometries::BoundingBox _BB, float _VoxelScale_um) {

    // Calculate Dimensions
    float SizeX = abs(_BB.bb_point1[0] - _BB.bb_point2[0]);
    float SizeY = abs(_BB.bb_point1[1] - _BB.bb_point2[1]);
    float SizeZ = abs(_BB.bb_point1[2] - _BB.bb_point2[2]);
    SizeX_ = SizeX/_VoxelScale_um;
    SizeY_ = SizeY/_VoxelScale_um;
    SizeZ_ = SizeZ/_VoxelScale_um;


    // Store Parameters
    BoundingBox_ = _BB;
    VoxelScale_um = _VoxelScale_um;


    // Malloc array
    Data_ = new VoxelType[SizeX_ * SizeY_ * SizeZ_];



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

void VoxelArray::SetVoxelAtPosition(float _X, float _Y, float _Z, VoxelType _Value) {

    int XIndex = (_X - BoundingBox_.bb_point1[0])/VoxelScale_um;
    int YIndex = (_Y - BoundingBox_.bb_point1[1])/VoxelScale_um;
    int ZIndex = (_Z - BoundingBox_.bb_point1[2])/VoxelScale_um;
    SetVoxel(XIndex, YIndex, ZIndex, _Value);

}


void VoxelArray::GetSize(int* _X, int* _Y, int* _Z) {
    (*_X) = SizeX_;
    (*_Y) = SizeY_;
    (*_Z) = SizeZ_;
}

int VoxelArray::GetX() {
    return SizeX_;
}

int VoxelArray::GetY() {
    return SizeY_;
}

int VoxelArray::GetZ() {
    return SizeZ_;
}



}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG
