#include <iostream>
#include <cstring>
#include <future>
#include <vector>

#include <VSDA/Ca/VoxelSubsystem/Structs/CaVoxelArray.h>



namespace BG {
namespace NES {
namespace VSDA {
namespace Calcium {


VoxelArray::VoxelArray(Simulator::BoundingBox _BB, float _VoxelScale_um) {

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
    DataMaxLength_ = (uint64_t)SizeX_ * (uint64_t)SizeY_ * (uint64_t)SizeZ_;
    Data_ = std::make_unique<VoxelType[]>(DataMaxLength_);

    // We don't need to clear this because make unique does it for us
    // Reset the array so we don't get a bunch of crap in it
    // ClearArray();
}
VoxelArray::VoxelArray(Simulator::ScanRegion _Region, float _VoxelScale_um) {

    // Create Bounding Box From Region, Then Call Other Constructor
    Simulator::BoundingBox BB;
    BB.bb_point1[0] = _Region.Point1X_um;
    BB.bb_point1[1] = _Region.Point1Y_um;
    BB.bb_point1[2] = _Region.Point1Z_um;

    BB.bb_point2[0] = _Region.Point2X_um;
    BB.bb_point2[1] = _Region.Point2Y_um;
    BB.bb_point2[2] = _Region.Point2Z_um;

    // Calculate Dimensions
    float SizeX = abs(BB.bb_point1[0] - BB.bb_point2[0]);
    float SizeY = abs(BB.bb_point1[1] - BB.bb_point2[1]);
    float SizeZ = abs(BB.bb_point1[2] - BB.bb_point2[2]);
    SizeX_ = SizeX/_VoxelScale_um;
    SizeY_ = SizeY/_VoxelScale_um;
    SizeZ_ = SizeZ/_VoxelScale_um;


    // Store Parameters
    BoundingBox_ = BB;
    VoxelScale_um = _VoxelScale_um;


    // Malloc array
    DataMaxLength_ = (uint64_t)SizeX_ * (uint64_t)SizeY_ * (uint64_t)SizeZ_;
    Data_ = std::make_unique<VoxelType[]>(DataMaxLength_);

    // make unique already clears memory, so we're doing it twice.
    // Reset the array so we don't get a bunch of crap in it
    // ClearArray();
}


VoxelArray::~VoxelArray() {

    // delete[] Data_;
}


void VoxelArray::ClearArray() {

    std::memset(Data_.get(), 0, DataMaxLength_*sizeof(VoxelType));

    // // Reset everything to 0s
    // for (uint64_t i = 0; i < DataMaxLength_; i++) {
    //     Data_.get()[i] = 0;
    // }

}

void VoxelArray::ClearArrayThreaded(int _NumThreads) {

    // Calculate Start Ptr, StepSize
    uint64_t StepSize = DataMaxLength_ / _NumThreads;
    VoxelType* StartAddress = Data_.get();

    // Create a bunch of memset tasks
    std::vector<std::future<int>> AsyncTasks;
    for (size_t i = 0; i < _NumThreads; i++) {
        VoxelType* ThreadStartAddress = StartAddress + (StepSize * i);
        assert(ThreadStartAddress + StepSize <= StartAddress + DataMaxLength_);
        AsyncTasks.push_back(std::async(std::launch::async, [ThreadStartAddress, StepSize]{
            std::memset(ThreadStartAddress, 0, StepSize);
            return 0;
        }));
    }

    // Now, wait for this to finish
    for (size_t i = 0; i < AsyncTasks.size(); i++) {
        AsyncTasks[i].get();
    }

}

bool VoxelArray::SetBB(Simulator::BoundingBox _NewBoundingBox) {
    assert(_NewBoundingBox.GetVoxelSize(VoxelScale_um) != DataMaxLength_);
    BoundingBox_ = _NewBoundingBox;
    return true;
}

bool VoxelArray::SetBB(Simulator::ScanRegion _NewBoundingBox) {

    // Create Bounding Box From Region, Then Call Other Constructor
    Simulator::BoundingBox BB;
    BB.bb_point1[0] = _NewBoundingBox.Point1X_um;
    BB.bb_point1[1] = _NewBoundingBox.Point1Y_um;
    BB.bb_point1[2] = _NewBoundingBox.Point1Z_um;

    BB.bb_point2[0] = _NewBoundingBox.Point2X_um;
    BB.bb_point2[1] = _NewBoundingBox.Point2Y_um;
    BB.bb_point2[2] = _NewBoundingBox.Point2Z_um;

    assert(BB.GetVoxelSize(VoxelScale_um) != DataMaxLength_);
    BoundingBox_ = BB;
    return true;
}

uint64_t VoxelArray::GetIndex(int _X, int _Y, int _Z) {
    return uint64_t(_X)*(SizeY_*SizeZ_) + uint64_t(_Y)*SizeZ_ + uint64_t(_Z);
}

VoxelType VoxelArray::GetVoxel(int _X, int _Y, int _Z, bool* _Status) {

    // Check Bounds
    if ((_X < 0 || _X >= SizeX_) || (_Y < 0 || _Y >= SizeY_) || (_Z < 0 || _Z >= SizeZ_)) {
        if (_Status != nullptr) {
            (*_Status) = false;
        }
        return VoxelType();
    }

    // Hope this works (please work dear god don't segfault)
    uint64_t Index = GetIndex(_X, _Y, _Z);
    if (Index < DataMaxLength_) {
        if (_Status != nullptr) {
            (*_Status) = true;
        }
        return Data_.get()[Index];
    }
    if (_Status != nullptr) {
        (*_Status) = false;
    }
    return VoxelType();

}

void VoxelArray::SetVoxel(int _X, int _Y, int _Z, VoxelType _Value) {
    uint64_t CurrentIndex = GetIndex(_X, _Y, _Z);
    if (CurrentIndex < 0 || CurrentIndex >= DataMaxLength_) {
        std::string ErrorMsg = std::string("E: Cannot Set Voxel At ") + std::to_string(_X);
        ErrorMsg += std::string(" ") + std::to_string(_Y) + std::string(" ") + std::to_string(_Z);
        ErrorMsg += std::string(" As This Would Be Out Of Range (index): ") + std::to_string(CurrentIndex) + "!";
        throw std::out_of_range(ErrorMsg.c_str());
    }
    Data_[CurrentIndex] = _Value;
}

void VoxelArray::SetVoxelAtPosition(float _X, float _Y, float _Z, VoxelType _Value) {

    // This is dangerous - there's a round call since this can lead to truncation errors
    int XIndex = round((_X - BoundingBox_.bb_point1[0])/VoxelScale_um);
    int YIndex = round((_Y - BoundingBox_.bb_point1[1])/VoxelScale_um);
    int ZIndex = round((_Z - BoundingBox_.bb_point1[2])/VoxelScale_um);

    // Check Bounds (so if it's out of bounds, we print a warning and do nothing!)
    if ((XIndex < 0 || XIndex >= SizeX_) || (YIndex < 0 || YIndex >= SizeY_) || (ZIndex < 0 || ZIndex >= SizeZ_)) {
        return;
    }

    SetVoxel(XIndex, YIndex, ZIndex, _Value);

}


void VoxelArray::GetSize(int* _X, int* _Y, int* _Z) {
    (*_X) = int(SizeX_);
    (*_Y) = int(SizeY_);
    (*_Z) = int(SizeZ_);
}

uint64_t VoxelArray::GetSize() {
    return DataMaxLength_;
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

Simulator::BoundingBox VoxelArray::GetBoundingBox() {
    return BoundingBox_;
}



}; // Close Namespace Calcium
}; // Close Namespace VSDA
}; // Close Namespace Common
}; // Close Namespace BG
