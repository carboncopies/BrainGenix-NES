#include <iostream>
#include <cstring>
#include <future>
#include <cstdlib>
#include <vector>

#include <VSDA/EM/VoxelSubsystem/Structs/VoxelArray.h>



namespace BG {
namespace NES {
namespace Simulator {



VoxelArray::VoxelArray(BG::Common::Logger::LoggingSystem* _Logger, BoundingBox _BB, float _VoxelScale_um) {
    Logger_ = _Logger;

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
    float SizeMiB = (sizeof(VoxelType) * DataMaxLength_) / 1024. / 1024.;
    _Logger->Log("Allocating Array Of Size " + std::to_string(SizeMiB) + "MiB In System RAM", 2);
    Data_ = std::make_unique<VoxelType[]>(DataMaxLength_);

    // We don't need to clear this because make unique does it for us
    // Reset the array so we don't get a bunch of crap in it
    // ClearArray();
}
VoxelArray::VoxelArray(BG::Common::Logger::LoggingSystem* _Logger, ScanRegion _Region, float _VoxelScale_um) {
    Logger_ = _Logger;

    // Create Bounding Box From Region, Then Call Other Constructor
    BoundingBox BB;
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
    float SizeMiB = (sizeof(VoxelType) * DataMaxLength_) / 1024. / 1024.;
    _Logger->Log("Allocating Array Of Size " + std::to_string(SizeMiB) + "MiB In System RAM", 2);
    VoxelType* VoxelArrayPtr = (VoxelType*)std::malloc(DataMaxLength_ * sizeof(VoxelType));
    Data_ = std::unique_ptr<VoxelType[]>(VoxelArrayPtr);

    ClearArrayThreaded(std::thread::hardware_concurrency());
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

    uint64_t ElementStepSize = DataMaxLength_ / _NumThreads;
    // VoxelType* StartAddress = Data_.get();

    // Initializer
    VoxelType Empty;
    Empty.State_ = VoxelState_EMPTY;

    // Create a bunch of memset tasks
    std::vector<std::future<int>> AsyncTasks;
    for (size_t i = 0; i < _NumThreads; i++) {
        // VoxelType* ThreadStartAddress = StartAddress + (ElementStepSize * i);
        uint64_t ThreadStartIndex = (ElementStepSize * i);
        uint64_t ThreadEndIndex = (ElementStepSize * i) + ElementStepSize;
        VoxelType* Array = Data_.get();

        AsyncTasks.push_back(std::async(std::launch::async, [Array, ThreadStartIndex, ThreadEndIndex, Empty]{
            for (uint64_t i = ThreadStartIndex; i < ThreadEndIndex; i++) {
                Array[i] = Empty;
            }
            return 0;
        }));
    }

    // Now, wait for this to finish
    for (size_t i = 0; i < AsyncTasks.size(); i++) {
        AsyncTasks[i].get();
    }


}

bool VoxelArray::SetBB(BoundingBox _NewBoundingBox) {
    assert(_NewBoundingBox.GetVoxelSize(VoxelScale_um) != DataMaxLength_);
    BoundingBox_ = _NewBoundingBox;
    return true;
}

bool VoxelArray::SetBB(ScanRegion _NewBoundingBox) {

    // Create Bounding Box From Region, Then Call Other Constructor
    BoundingBox BB;
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

VoxelType VoxelArray::GetVoxel(int _X, int _Y, int _Z) {

    // Check Bounds
    if ((_X < 0 || _X >= SizeX_) || (_Y < 0 || _Y >= SizeY_) || (_Z < 0 || _Z >= SizeZ_)) {
        VoxelType Ret;
        Ret.ParentUID = 0;
        Ret.DistanceToEdge_vox_ = 0;
        Ret.State_ = VoxelState_OUT_OF_BOUNDS;
        return Ret;
    }

    // Hope this works (please work dear god don't segfault)
    uint64_t Index = GetIndex(_X, _Y, _Z);
    if (Index < DataMaxLength_) {
        return Data_.get()[Index];
    }
    
    VoxelType Ret;
    Ret.ParentUID = 0;
    Ret.DistanceToEdge_vox_ = 0;
    Ret.State_ = VoxelState_OUT_OF_BOUNDS;
    return Ret;

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

void VoxelArray::SetVoxelAtIndex(int _XIndex, int _YIndex, int _ZIndex, VoxelType _Value) {
    
    uint64_t CurrentIndex = GetIndex(_XIndex, _YIndex, _ZIndex);
    if (CurrentIndex < 0 || CurrentIndex >= DataMaxLength_) {
        return;
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

Geometries::Vec3D VoxelArray::GetPositionAtIndex(int _XIndex, int _YIndex, int _ZIndex) {

    float XPos_um = _XIndex * VoxelScale_um + BoundingBox_.bb_point1[0];
    float YPos_um = _YIndex * VoxelScale_um + BoundingBox_.bb_point1[1];
    float ZPos_um = _ZIndex * VoxelScale_um + BoundingBox_.bb_point1[2];
    
    return Geometries::Vec3D(XPos_um, YPos_um, ZPos_um);

}

int VoxelArray::GetXIndexAtPosition(float _X_Worldspace_um) {
    return round((_X_Worldspace_um - BoundingBox_.bb_point1[0])/VoxelScale_um);
}
int VoxelArray::GetYIndexAtPosition(float _Y_Worldspace_um) {
    return round((_Y_Worldspace_um - BoundingBox_.bb_point1[1])/VoxelScale_um);
}
int VoxelArray::GetZIndexAtPosition(float _Z_Worldspace_um) {
    return round((_Z_Worldspace_um - BoundingBox_.bb_point1[2])/VoxelScale_um);
}

// void VoxelArray::SetVoxelIfNotDarker(float _X, float _Y, float _Z, VoxelType _Value) {

//     // This is dangerous - there's a round call since this can lead to truncation errors
//     int XIndex = round((_X - BoundingBox_.bb_point1[0])/VoxelScale_um);
//     int YIndex = round((_Y - BoundingBox_.bb_point1[1])/VoxelScale_um);
//     int ZIndex = round((_Z - BoundingBox_.bb_point1[2])/VoxelScale_um);

//     // Check Bounds (so if it's out of bounds, we print a warning and do nothing!)
//     if ((XIndex < 0 || XIndex >= SizeX_) || (YIndex < 0 || YIndex >= SizeY_) || (ZIndex < 0 || ZIndex >= SizeZ_)) {
//         return;
//     }

//     // Get Voxel At Index, Check That It's Not Darker, Then Set, Otherwise Don't Set
//     VoxelType ThisVoxel = GetVoxel(XIndex, YIndex, ZIndex);

//     // Only set the color if it's not in the enum range, and it's darker than the current value (except if it's empty)
//     if (_Value.State_ == VoxelState_INTERIOR) {
//         if ((ThisVoxel.Intensity_ > _Value.Intensity_) || (ThisVoxel.State_ == VoxelState_BORDER)) {
//             SetVoxel(XIndex, YIndex, ZIndex, _Value);
//         }
//     } else if (_Value.State_ == VoxelState_BORDER) {
//         if ((ThisVoxel.State_ != VoxelState_INTERIOR) && (ThisVoxel.Intensity_ > _Value.Intensity_)) {
//             SetVoxel(XIndex, YIndex, ZIndex, _Value);
//         }
//     }

// }
// void VoxelArray::SetVoxelIfNotDarkerAtIndex(int _X, int _Y, int _Z, VoxelType _Value) {

//     // This is dangerous - there's a round call since this can lead to truncation errors
//     int XIndex = _X;
//     int YIndex = _Y;
//     int ZIndex = _Z;

//     // Check Bounds (so if it's out of bounds, we print a warning and do nothing!)
//     if ((XIndex < 0 || XIndex >= SizeX_) || (YIndex < 0 || YIndex >= SizeY_) || (ZIndex < 0 || ZIndex >= SizeZ_)) {
//         return;
//     }

//     // Get Voxel At Index, Check That It's Not Darker, Then Set, Otherwise Don't Set
//     VoxelType ThisVoxel = GetVoxel(XIndex, YIndex, ZIndex);

//     // Only set the color if it's not in the enum range, and it's darker than the current value (except if it's empty)
//     if (_Value.State_ == VoxelState_INTERIOR) {
//         if ((ThisVoxel.Intensity_ > _Value.Intensity_) || (ThisVoxel.State_ == VoxelState_BORDER)) {
//             SetVoxel(XIndex, YIndex, ZIndex, _Value);
//         }
//     } else if (_Value.State_ == VoxelState_BORDER) {
//         if ((ThisVoxel.State_ != VoxelState_INTERIOR) && (ThisVoxel.Intensity_ > _Value.Intensity_)) {
//             SetVoxel(XIndex, YIndex, ZIndex, _Value);
//         }
//     }

// }

void VoxelArray::GetSize(int* _X, int* _Y, int* _Z) {
    (*_X) = int(SizeX_);
    (*_Y) = int(SizeY_);
    (*_Z) = int(SizeZ_);
}
bool VoxelArray::SetSize(int _X, int _Y, int _Z) {

    uint64_t ProposedSize = uint64_t(_X) * uint64_t(_Y) * uint64_t(_Z);
    
    if (ProposedSize < DataMaxLength_) {

        std::string ResizePercent = std::to_string((double(ProposedSize) / double(DataMaxLength_)) * 100.);
        Logger_->Log("Resizing Voxel Array To " + std::to_string(_X) + "XVox, " + std::to_string(_Y) + "YVox, " + std::to_string(_Z) + "ZVox, ~" + ResizePercent + "% of Allocated Size", 4);

        SizeX_ = _X;
        SizeY_ = _Y;
        SizeZ_ = _Z;

        return true;
    } else {
        Logger_->Log("Cannot Resize Voxel Array To Requested Size, It Exceeds Currently Allocated Size", 10);
    }

    return false;
}
bool VoxelArray::SetSize(ScanRegion _TargetSize, float _VoxelScale_um) {
    
    // Calc size in voxels for x, y, z
    int VoxelSizeX = _TargetSize.SizeX() / _VoxelScale_um;
    int VoxelSizeY = _TargetSize.SizeY() / _VoxelScale_um;
    int VoxelSizeZ = _TargetSize.SizeZ() / _VoxelScale_um;

    return SetSize(VoxelSizeX, VoxelSizeY, VoxelSizeZ);

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

BoundingBox VoxelArray::GetBoundingBox() {
    return BoundingBox_;
}

bool VoxelArray::IsInRangeX(float _X) {
    return _X >= BoundingBox_.bb_point1[0] && _X <= BoundingBox_.bb_point2[0];
}
bool VoxelArray::IsInRangeY(float _Y) {
    return _Y >= BoundingBox_.bb_point1[1] && _Y <= BoundingBox_.bb_point2[1];
}
bool VoxelArray::IsInRangeZ(float _Z) {
    return _Z >= BoundingBox_.bb_point1[2] && _Z <= BoundingBox_.bb_point2[2];
}

float VoxelArray::GetXPositionAtIndex(int _XIndex) {
    return BoundingBox_.bb_point1[0] + _XIndex * VoxelScale_um;
}
float VoxelArray::GetYPositionAtIndex(int _YIndex) {
    return BoundingBox_.bb_point1[1] + _YIndex * VoxelScale_um;
}
float VoxelArray::GetZPositionAtIndex(int _ZIndex) {
    return BoundingBox_.bb_point1[2] + _ZIndex * VoxelScale_um;
}

void VoxelArray::CompositeVoxel(float _X, float _Y, float _Z, VoxelState _State, float _DistanceToEdge, uint64_t _ParentUID) {
    // Convert worldspace coordinates to voxel indices
    int XIndex = round((_X - BoundingBox_.bb_point1[0]) / VoxelScale_um);
    int YIndex = round((_Y - BoundingBox_.bb_point1[1]) / VoxelScale_um);
    int ZIndex = round((_Z - BoundingBox_.bb_point1[2]) / VoxelScale_um);

    // Call the index-based function
    CompositeVoxelAtIndex(XIndex, YIndex, ZIndex, _State, _DistanceToEdge, _ParentUID);
}
void VoxelArray::CompositeVoxelAtIndex(int _X, int _Y, int _Z, VoxelState _State, float _DistanceToEdge_um, uint64_t _ParentUID) {
    int XIndex = _X;
    int YIndex = _Y;
    int ZIndex = _Z;

    // Check bounds
    if ((XIndex < 0 || XIndex >= SizeX_) || (YIndex < 0 || YIndex >= SizeY_) || (ZIndex < 0 || ZIndex >= SizeZ_)) {
        return;
    }

    // Get the current voxel at the index
    VoxelType ThisVoxel = GetVoxel(XIndex, YIndex, ZIndex);

    // Update the voxel state and distance to edge
    uint8_t CorrectedDistanceToEdge_vox = std::min(255, int(_DistanceToEdge_um / VoxelScale_um));
    if (ThisVoxel.DistanceToEdge_vox_ < CorrectedDistanceToEdge_vox) {
        ThisVoxel.DistanceToEdge_vox_ = CorrectedDistanceToEdge_vox;
    }
    if (ThisVoxel.State_ < VoxelState_BLACK) {
        ThisVoxel.State_ = _State;
    }
    ThisVoxel.ParentUID = _ParentUID;
    SetVoxel(XIndex, YIndex, ZIndex, ThisVoxel);

}


}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG
