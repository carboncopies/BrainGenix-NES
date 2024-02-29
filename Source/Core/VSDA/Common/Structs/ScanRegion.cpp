#include <math.h>

#include <VSDA/Common/Structs/ScanRegion.h>




namespace BG {
namespace NES {
namespace Simulator {



std::string ScanRegion::Dimensions() {

    std::string Out = "";
    Out += std::to_string(fabs(Point1X_um - Point2X_um)) + "X, ";
    Out += std::to_string(fabs(Point1Y_um - Point2Y_um)) + "Y, ";
    Out += std::to_string(fabs(Point1Z_um - Point2Z_um)) + "Z";

    return Out;
}

std::string ScanRegion::ToString() {
    std::string Out = "";
    Out += std::to_string(Point1X_um) + "X1, ";
    Out += std::to_string(Point1Y_um) + "Y1, ";
    Out += std::to_string(Point1Z_um) + "Z1 | ";

    Out += std::to_string(Point2X_um) + "X2 ";
    Out += std::to_string(Point2Y_um) + "Y2, ";
    Out += std::to_string(Point2Z_um) + "Z2";

    return Out;
}

std::string ScanRegion::GetDimensionsInVoxels(float _VoxelScale_um) {
    float SizeX = fabs(Point1X_um - Point2X_um); // apparently abs *only* returns an int!!!!! WTF!>!:??!?!?! WHY DO I HAVE TO USE fabs INSTEAD?>!?!?!?!!
    float SizeY = fabs(Point1Y_um - Point2Y_um);
    float SizeZ = fabs(Point1Z_um - Point2Z_um);
    uint64_t SizeX_ = SizeX/_VoxelScale_um;
    uint64_t SizeY_ = SizeY/_VoxelScale_um;
    uint64_t SizeZ_ = SizeZ/_VoxelScale_um;

    std::string Out = "";
    Out += std::to_string(SizeX_) + "XVox, ";
    Out += std::to_string(SizeY_) + "YVox, ";
    Out += std::to_string(SizeZ_) + "ZVox";

    return Out;

}

double ScanRegion::SizeX() {
    return fabs(Point1X_um - Point2X_um);
}

double ScanRegion::SizeY() {
    return fabs(Point1Y_um - Point2Y_um);
}

double ScanRegion::SizeZ() {
    return fabs(Point1Z_um - Point2Z_um);
}


uint64_t ScanRegion::GetVoxelSize(float _VoxelScale_um) {
    float SizeX = fabs(Point1X_um - Point2X_um);
    float SizeY = fabs(Point1Y_um - Point2Y_um);
    float SizeZ = fabs(Point1Z_um - Point2Z_um);
    uint64_t SizeX_ = SizeX/_VoxelScale_um;
    uint64_t SizeY_ = SizeY/_VoxelScale_um;
    uint64_t SizeZ_ = SizeZ/_VoxelScale_um;

    return (uint64_t)SizeX_ * (uint64_t)SizeY_ * (uint64_t)SizeZ_;
}


}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG
