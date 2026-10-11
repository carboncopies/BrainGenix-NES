//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: Unit tests for ScanRegion and WorldInfo (dev_frame NT-13).
*/

#include <gtest/gtest.h>

#include <VSDA/Common/Structs/ScanRegion.h>
#include <VSDA/Common/Structs/WorldInfo.h>

using BG::NES::Simulator::ScanRegion;
using BG::NES::VSDA::WorldInfo;

namespace {
ScanRegion Region(float X1, float Y1, float Z1, float X2, float Y2, float Z2) {
    ScanRegion R{};  // value-initialise: the float members have no default initialisers
    R.Point1X_um = X1; R.Point1Y_um = Y1; R.Point1Z_um = Z1;
    R.Point2X_um = X2; R.Point2Y_um = Y2; R.Point2Z_um = Z2;
    return R;
}
}

TEST(ScanRegion, SizesAreAbsoluteDifferences) {
    ScanRegion R = Region(1, 2, 3, 5, 10, 4);
    EXPECT_DOUBLE_EQ(R.SizeX(), 4.0);
    EXPECT_DOUBLE_EQ(R.SizeY(), 8.0);
    EXPECT_DOUBLE_EQ(R.SizeZ(), 1.0);
}

TEST(ScanRegion, ReversedPointsGiveSameSizes) {
    ScanRegion R = Region(5, 10, 4, 1, 2, 3);
    EXPECT_DOUBLE_EQ(R.SizeX(), 4.0);
    EXPECT_DOUBLE_EQ(R.SizeY(), 8.0);
    EXPECT_DOUBLE_EQ(R.SizeZ(), 1.0);
    EXPECT_EQ(R.GetVoxelSize(1.0f), 32u);
    EXPECT_EQ(R.Dimensions(), Region(1, 2, 3, 5, 10, 4).Dimensions());
}

TEST(ScanRegion, NegativeCoordinatesAndZeroSize) {
    ScanRegion R = Region(-2, -2, -2, 2, 2, 2);
    EXPECT_DOUBLE_EQ(R.SizeX(), 4.0);
    ScanRegion Z = Region(3, 3, 3, 3, 3, 3);
    EXPECT_DOUBLE_EQ(Z.SizeX(), 0.0);
    EXPECT_EQ(Z.GetVoxelSize(1.0f), 0u);
}

TEST(ScanRegion, DimensionsStringFormat) {
    EXPECT_EQ(Region(0, 0, 0, 2.5f, 1, 4).Dimensions(), "2.500000X, 1.000000Y, 4.000000Z");
}

TEST(ScanRegion, ToStringListsBothPoints) {
    EXPECT_EQ(Region(1, 2, 3, 4, 5, 6).ToString(),
              "1.000000X1, 2.000000Y1, 3.000000Z1 | 4.000000X2 5.000000Y2, 6.000000Z2");
}

TEST(ScanRegion, GetVoxelSizeIsProductOfPerAxisVoxelCounts) {
    EXPECT_EQ(Region(0, 0, 0, 4, 3, 2).GetVoxelSize(1.0f), 24u);
    EXPECT_EQ(Region(0, 0, 0, 4, 3, 2).GetVoxelSize(0.5f), 8u * 6u * 4u);
    EXPECT_EQ(Region(0, 0, 0, 4, 3, 2).GetVoxelSize(2.0f), 2u * 1u * 1u);  // 3/2 truncates to 1
}

TEST(ScanRegion, NonIntegerRatiosTruncatePerAxisBeforeMultiplying) {
    // 1.0 / 0.3 = 3.33 -> 3 voxels per axis
    ScanRegion R = Region(0, 0, 0, 1, 1, 1);
    EXPECT_EQ(R.GetVoxelSize(0.3f), 27u);
    EXPECT_EQ(R.GetDimensionsInVoxels(0.3f), "3XVox, 3YVox, 3ZVox");
    // smaller than one voxel on one axis gives an empty array
    EXPECT_EQ(Region(0, 0, 0, 10, 10, 0.4f).GetVoxelSize(1.0f), 0u);
}

TEST(ScanRegion, GetDimensionsInVoxelsFormatAndReversedPoints) {
    EXPECT_EQ(Region(0, 0, 0, 4, 3, 2).GetDimensionsInVoxels(0.5f), "8XVox, 6YVox, 4ZVox");
    EXPECT_EQ(Region(4, 3, 2, 0, 0, 0).GetDimensionsInVoxels(0.5f), "8XVox, 6YVox, 4ZVox");
}

TEST(WorldInfo, DefaultsAreIdentityRotationAndHundredthMicronVoxels) {
    WorldInfo W{};
    EXPECT_FLOAT_EQ(W.WorldRotationOffsetX_rad, 0.0f);
    EXPECT_FLOAT_EQ(W.WorldRotationOffsetY_rad, 0.0f);
    EXPECT_FLOAT_EQ(W.WorldRotationOffsetZ_rad, 0.0f);
    EXPECT_FLOAT_EQ(W.VoxelScale_um, 0.01f);
}
