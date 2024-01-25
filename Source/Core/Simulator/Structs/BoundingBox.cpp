#include <algorithm>
#include <iostream>

#include <Simulator/Structs/BoundingBox.h>



namespace BG {
namespace NES {
namespace Simulator {


std::string BoundingBox::Dimensions() {

    std::string Out = "";
    Out += std::to_string(bb_point2[0] - bb_point1[0]) + "X, ";
    Out += std::to_string(bb_point2[1] - bb_point1[1]) + "Y, ";
    Out += std::to_string(bb_point2[2] - bb_point1[2]) + "Z";

    return Out;
}

std::string BoundingBox::ToString() {
    std::string Out = "";
    Out += std::to_string(bb_point1[0]) + "X1, ";
    Out += std::to_string(bb_point1[1]) + "Y1, ";
    Out += std::to_string(bb_point1[2]) + "Z1 | ";

    Out += std::to_string(bb_point2[0]) + "X2 ";
    Out += std::to_string(bb_point2[1]) + "Y2, ";
    Out += std::to_string(bb_point2[2]) + "Z2";

    return Out;
}


bool IsOverlapping1D(float Max1, float Min1, float Max2, float Min2) {
    return (Max1 >= Min2) && (Max2 >= Min1);
}

bool BoundingBox::IsIntersecting(BoundingBox _Box) {

    float XMax1 = std::max(bb_point1[0], bb_point2[0]);
    float YMax1 = std::max(bb_point1[1], bb_point2[1]);
    float ZMax1 = std::max(bb_point1[2], bb_point2[2]);
    float XMax2 = std::max(_Box.bb_point1[0], _Box.bb_point2[0]);
    float YMax2 = std::max(_Box.bb_point1[1], _Box.bb_point2[1]);
    float ZMax2 = std::max(_Box.bb_point1[2], _Box.bb_point2[2]);

    float XMin1 = std::min(bb_point1[0], bb_point2[0]);
    float YMin1 = std::min(bb_point1[1], bb_point2[1]);
    float ZMin1 = std::min(bb_point1[2], bb_point2[2]);
    float XMin2 = std::min(_Box.bb_point1[0], _Box.bb_point2[0]);
    float YMin2 = std::min(_Box.bb_point1[1], _Box.bb_point2[1]);
    float ZMin2 = std::min(_Box.bb_point1[2], _Box.bb_point2[2]);

    // Now, do the max min stuff
    return IsOverlapping1D(XMax1, XMin1, XMax2, XMin2) &&
           IsOverlapping1D(YMax1, YMin1, YMax2, YMin2) &&
           IsOverlapping1D(ZMax1, ZMin1, ZMax2, ZMin2);

}

uint64_t BoundingBox::GetVoxelSize(float _VoxelScale_um) {

    float SizeX = abs(bb_point1[0] - bb_point2[0]);
    float SizeY = abs(bb_point1[1] - bb_point2[1]);
    float SizeZ = abs(bb_point1[2] - bb_point2[2]);
    uint64_t SizeX_ = SizeX/_VoxelScale_um;
    uint64_t SizeY_ = SizeY/_VoxelScale_um;
    uint64_t SizeZ_ = SizeZ/_VoxelScale_um;

    return (uint64_t)SizeX_ * (uint64_t)SizeY_ * (uint64_t)SizeZ_;
}


}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
