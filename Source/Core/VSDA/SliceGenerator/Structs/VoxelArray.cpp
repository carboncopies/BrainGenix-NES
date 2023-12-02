#include <VSDA/SliceGenerator/Structs/VoxelArray.h>


#include <iostream>

namespace BG {
namespace NES {
namespace Simulator {



VoxelArray::VoxelArray(BoundingBox _BB, float _VoxelScale_um) {

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
    Data_ = std::make_unique<VoxelType[]>(SizeX_ * SizeY_ * SizeZ_);
    DataMaxLength_ = SizeX_ * SizeY_ * SizeZ_;
    // std::unique_ptr<VoxelType[]> Data_(new VoxelType[SizeX_ * SizeY_ * SizeZ_]);



    // Reset the array so we don't get a bunch of crap in it
    ClearArray();
}



VoxelArray::~VoxelArray() {

    // delete[] Data_;
}


void VoxelArray::ClearArray() {

    // Reset everything to 0s
    for (uint64_t i = 0; i < SizeX_ * SizeY_ * SizeZ_; i++) {
        Data_.get()[i] = 0;
    }

}

int VoxelArray::GetIndex(int _X, int _Y, int _Z) {
    return _X*(SizeY_*SizeZ_) + _Y*SizeZ_ + _Z;
}

VoxelType VoxelArray::GetVoxel(int _X, int _Y, int _Z) {

    // Hope this works
    return Data_.get()[GetIndex(_X, _Y, _Z)];

}

void VoxelArray::SetVoxel(int _X, int _Y, int _Z, VoxelType _Value) {
    int CurrentIndex = GetIndex(_X, _Y, _Z);
    if (CurrentIndex < 0 || CurrentIndex >= DataMaxLength_) {
        std::string ErrorMsg = std::string("E: Cannot Set Voxel At ") + std::to_string(_X);
        ErrorMsg += std::string(" ") + std::to_string(_Y) + std::string(" ") + std::to_string(_Z);
        ErrorMsg += std::string(" As This Would Be Out Of Range (index): ") + std::to_string(CurrentIndex) + "!";
        throw std::out_of_range(ErrorMsg.c_str());
    }
    Data_.get()[CurrentIndex] = _Value;
}

void VoxelArray::SetVoxelAtPosition(float _X, float _Y, float _Z, VoxelType _Value) {

    // This is dangerous - there's a round call since this can lead to truncation errors
    int XIndex = round((_X - BoundingBox_.bb_point1[0])/VoxelScale_um);
    int YIndex = round((_Y - BoundingBox_.bb_point1[1])/VoxelScale_um);
    int ZIndex = round((_Z - BoundingBox_.bb_point1[2])/VoxelScale_um);

    // Check Bounds (so if it's out of bounds, we print a warning and do nothing!)
    if ((XIndex < 0 || XIndex > SizeX_) || (YIndex < 0 || YIndex > SizeY_) || (ZIndex < 0 || ZIndex > SizeZ_)) {
        std::cout<<"Warning: Voxel at "<<_X<<" "<<_Y<<" "<<_Z<<" Is Out Of Bounds!\n";
        return;
    }

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

float VoxelArray::GetResolution() {
    return VoxelScale_um;
}

BoundingBox VoxelArray::GetBoundingBox() {
    return BoundingBox_;
}

}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG
