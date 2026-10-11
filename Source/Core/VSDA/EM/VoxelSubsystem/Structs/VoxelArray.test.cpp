//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: Unit tests for the EM VoxelArray (dev_frame NT-13).
    Arrays are tiny (VoxelType is 16 B). Tests marked *KnownBug assert the correct behaviour
    and are excluded from the gate (ctest -LE known_bug), see TestUtil/README.md.
*/

#include <cstdlib>
#include <cstring>

#include <gtest/gtest.h>

#include <TestUtil/TestUtil.h>
#include <VSDA/EM/VoxelSubsystem/Structs/VoxelArray.h>

using namespace BG::NES::Simulator;

namespace {

BoundingBox MakeBB(float X1, float Y1, float Z1, float X2, float Y2, float Z2) {
    BoundingBox BB{};
    BB.bb_point1[0] = X1; BB.bb_point1[1] = Y1; BB.bb_point1[2] = Z1;
    BB.bb_point2[0] = X2; BB.bb_point2[1] = Y2; BB.bb_point2[2] = Z2;
    return BB;
}

ScanRegion MakeRegion(float X1, float Y1, float Z1, float X2, float Y2, float Z2) {
    ScanRegion R{};
    R.Point1X_um = X1; R.Point1Y_um = Y1; R.Point1Z_um = Z1;
    R.Point2X_um = X2; R.Point2Y_um = Y2; R.Point2Z_um = Z2;
    return R;
}

VoxelType MakeVoxel(VoxelState S, uint8_t D, uint64_t UID) {
    VoxelType V{};
    V.State_ = S;
    V.DistanceToEdge_vox_ = D;
    V.ParentUID = UID;
    return V;
}

// Fills every voxel with a recognisable non-empty value.
void FillAll(VoxelArray& A, uint64_t UID = 7) {
    for (int X = 0; X < A.GetX(); X++)
        for (int Y = 0; Y < A.GetY(); Y++)
            for (int Z = 0; Z < A.GetZ(); Z++)
                A.SetVoxelAtIndex(X, Y, Z, MakeVoxel(VoxelState_INTERIOR, 3, UID));
}

int CountNotEmpty(VoxelArray& A) {
    int N = 0;
    for (int X = 0; X < A.GetX(); X++)
        for (int Y = 0; Y < A.GetY(); Y++)
            for (int Z = 0; Z < A.GetZ(); Z++)
                if (A.GetVoxel(X, Y, Z).State_ != VoxelState_EMPTY) N++;
    return N;
}

// Leaves a recognisable pattern in the stack area the next call will reuse.
__attribute__((noinline)) void PoisonStack() {
    volatile unsigned char Buf[16384];
    for (size_t i = 0; i < sizeof(Buf); i++) Buf[i] = 0xAB;
}

} // namespace

class VoxelArrayTest : public testing::Test {
protected:
    std::unique_ptr<BG::Common::Logger::LoggingSystem> Logger = BG::NES::TestUtil::MakeSilentLogger();
    // 4 x 3 x 2 voxels at 1 um, origin at (0,0,0)
    VoxelArray Make() { return VoxelArray(Logger.get(), MakeBB(0, 0, 0, 4, 3, 2), 1.0f); }
};

TEST_F(VoxelArrayTest, BoundingBoxCtorSizesFromBoxAndScale) {
    VoxelArray A = Make();
    int X, Y, Z;
    A.GetSize(&X, &Y, &Z);
    EXPECT_EQ(X, 4); EXPECT_EQ(Y, 3); EXPECT_EQ(Z, 2);
    EXPECT_EQ(A.GetX(), 4); EXPECT_EQ(A.GetY(), 3); EXPECT_EQ(A.GetZ(), 2);
    EXPECT_EQ(A.GetSize(), 24u);
    EXPECT_FLOAT_EQ(A.GetResolution(), 1.0f);
}

TEST_F(VoxelArrayTest, BoundingBoxCtorFractionalScale) {
    VoxelArray A(Logger.get(), MakeBB(0, 0, 0, 2, 1, 0.5f), 0.5f);
    EXPECT_EQ(A.GetX(), 4); EXPECT_EQ(A.GetY(), 2); EXPECT_EQ(A.GetZ(), 1);
    EXPECT_EQ(A.GetSize(), 8u);
}

TEST_F(VoxelArrayTest, BoundingBoxCtorSwappedCornersGiveSameSize) {
    VoxelArray A(Logger.get(), MakeBB(4, 3, 2, 0, 0, 0), 1.0f);
    EXPECT_EQ(A.GetX(), 4); EXPECT_EQ(A.GetY(), 3); EXPECT_EQ(A.GetZ(), 2);
    EXPECT_EQ(A.GetSize(), 24u);
}

TEST_F(VoxelArrayTest, BoundingBoxCtorFreshArrayIsEmpty) {
    VoxelArray A = Make();
    for (int X = 0; X < 4; X++) for (int Y = 0; Y < 3; Y++) for (int Z = 0; Z < 2; Z++) {
        VoxelType V = A.GetVoxel(X, Y, Z);
        EXPECT_EQ(V.State_, VoxelState_EMPTY);
        EXPECT_EQ(V.ParentUID, 0u);
        EXPECT_EQ(V.DistanceToEdge_vox_, 0);
    }
}

TEST_F(VoxelArrayTest, ZeroSizeArrayIsAllOutOfBounds) {
    VoxelArray A(Logger.get(), MakeBB(1, 1, 1, 1, 1, 1), 1.0f);
    EXPECT_EQ(A.GetSize(), 0u);
    EXPECT_EQ(A.GetVoxel(0, 0, 0).State_, VoxelState_OUT_OF_BOUNDS);
    EXPECT_THROW(A.SetVoxel(0, 0, 0, MakeVoxel(VoxelState_INTERIOR, 0, 1)), std::out_of_range);
    EXPECT_NO_THROW(A.SetVoxelAtIndex(0, 0, 0, MakeVoxel(VoxelState_INTERIOR, 0, 1)));
    EXPECT_NO_THROW(A.ClearArray());
}

TEST_F(VoxelArrayTest, ScanRegionCtorSizesMatchRegion) {
    VoxelArray A(Logger.get(), MakeRegion(0, 0, 0, 4, 3, 2), 1.0f);
    EXPECT_EQ(A.GetX(), 4); EXPECT_EQ(A.GetY(), 3); EXPECT_EQ(A.GetZ(), 2);
    EXPECT_EQ(A.GetSize(), 24u);
    BoundingBox BB = A.GetBoundingBox();
    EXPECT_FLOAT_EQ(BB.bb_point2[0], 4.0f);
    EXPECT_FLOAT_EQ(BB.bb_point1[2], 0.0f);
}

TEST_F(VoxelArrayTest, ScanRegionCtorSwappedCornersGiveSameSize) {
    VoxelArray A(Logger.get(), MakeRegion(4, 3, 2, 0, 0, 0), 1.0f);
    EXPECT_EQ(A.GetSize(), 24u);
}

TEST_F(VoxelArrayTest, GetVoxelBoundariesReturnOutOfBounds) {
    VoxelArray A = Make();
    FillAll(A);
    EXPECT_EQ(A.GetVoxel(0, 0, 0).State_, VoxelState_INTERIOR);
    EXPECT_EQ(A.GetVoxel(3, 2, 1).State_, VoxelState_INTERIOR);
    const int Bad[][3] = {{-1, 0, 0}, {0, -1, 0}, {0, 0, -1}, {4, 0, 0}, {0, 3, 0}, {0, 0, 2}, {4, 3, 2}};
    for (auto& B : Bad) {
        VoxelType V = A.GetVoxel(B[0], B[1], B[2]);
        EXPECT_EQ(V.State_, VoxelState_OUT_OF_BOUNDS) << B[0] << "," << B[1] << "," << B[2];
        EXPECT_EQ(V.ParentUID, 0u);
        EXPECT_EQ(V.DistanceToEdge_vox_, 0);
    }
}

TEST_F(VoxelArrayTest, SetGetRoundTripAndLayoutDoNotAlias) {
    VoxelArray A = Make();
    uint64_t Tag = 1;
    for (int X = 0; X < 4; X++) for (int Y = 0; Y < 3; Y++) for (int Z = 0; Z < 2; Z++)
        A.SetVoxel(X, Y, Z, MakeVoxel(VoxelState_INTERIOR, 1, Tag++));
    Tag = 1;
    for (int X = 0; X < 4; X++) for (int Y = 0; Y < 3; Y++) for (int Z = 0; Z < 2; Z++)
        EXPECT_EQ(A.GetVoxel(X, Y, Z).ParentUID, Tag++);
}

TEST_F(VoxelArrayTest, SetVoxelThrowsPastTheEnd) {
    VoxelArray A = Make();
    EXPECT_THROW(A.SetVoxel(4, 0, 0, MakeVoxel(VoxelState_INTERIOR, 0, 1)), std::out_of_range);
}

TEST_F(VoxelArrayTest, SetVoxelThrowsOnNegativeIndex) {
    VoxelArray A = Make();
    EXPECT_THROW(A.SetVoxel(-1, 0, 0, MakeVoxel(VoxelState_INTERIOR, 0, 1)), std::out_of_range);
    EXPECT_EQ(CountNotEmpty(A), 0);
}

TEST_F(VoxelArrayTest, SetVoxelAtIndexOutOfRangeIsNoOp) {
    VoxelArray A = Make();
    EXPECT_NO_THROW(A.SetVoxelAtIndex(4, 0, 0, MakeVoxel(VoxelState_INTERIOR, 0, 1)));
    EXPECT_NO_THROW(A.SetVoxelAtIndex(-1, 0, 0, MakeVoxel(VoxelState_INTERIOR, 0, 1)));
    EXPECT_NO_THROW(A.SetVoxelAtIndex(100, 100, 100, MakeVoxel(VoxelState_INTERIOR, 0, 1)));
    EXPECT_EQ(CountNotEmpty(A), 0);
    A.SetVoxelAtIndex(1, 1, 1, MakeVoxel(VoxelState_BORDER, 2, 9));
    EXPECT_EQ(A.GetVoxel(1, 1, 1).State_, VoxelState_BORDER);
    EXPECT_EQ(A.GetVoxel(1, 1, 1).ParentUID, 9u);
    EXPECT_EQ(CountNotEmpty(A), 1);
}


TEST_F(VoxelArrayTest, SetVoxelAtPositionRoundsAndIgnoresOutOfRange) {
    VoxelArray A(Logger.get(), MakeBB(-2, -2, -2, 2, 2, 2), 1.0f);
    A.SetVoxelAtPosition(-2.0f, -2.0f, -2.0f, MakeVoxel(VoxelState_INTERIOR, 0, 5));
    EXPECT_EQ(A.GetVoxel(0, 0, 0).ParentUID, 5u);
    A.SetVoxelAtPosition(0.4f, 0.0f, 0.0f, MakeVoxel(VoxelState_INTERIOR, 0, 6));
    EXPECT_EQ(A.GetVoxel(2, 2, 2).ParentUID, 6u);
    A.SetVoxelAtPosition(1.4f, 1.4f, 1.4f, MakeVoxel(VoxelState_INTERIOR, 0, 8));
    EXPECT_EQ(A.GetVoxel(3, 3, 3).ParentUID, 8u);
    int Before = CountNotEmpty(A);
    A.SetVoxelAtPosition(2.6f, 0.0f, 0.0f, MakeVoxel(VoxelState_INTERIOR, 0, 99));
    A.SetVoxelAtPosition(-2.6f, 0.0f, 0.0f, MakeVoxel(VoxelState_INTERIOR, 0, 99));
    A.SetVoxelAtPosition(0.0f, 0.0f, 100.0f, MakeVoxel(VoxelState_INTERIOR, 0, 99));
    EXPECT_EQ(CountNotEmpty(A), Before);
}

TEST_F(VoxelArrayTest, IndexAtPositionRoundsHalfAwayFromZero) {
    VoxelArray A(Logger.get(), MakeBB(0, 0, 0, 4, 4, 4), 1.0f);
    EXPECT_EQ(A.GetXIndexAtPosition(0.49f), 0);
    EXPECT_EQ(A.GetXIndexAtPosition(0.5f), 1);
    EXPECT_EQ(A.GetYIndexAtPosition(1.5f), 2);
    EXPECT_EQ(A.GetZIndexAtPosition(2.49f), 2);
    EXPECT_EQ(A.GetXIndexAtPosition(-0.49f), 0);
    EXPECT_EQ(A.GetXIndexAtPosition(-0.5f), -1);
    EXPECT_EQ(A.GetYIndexAtPosition(-3.0f), -3);
    EXPECT_EQ(A.GetZIndexAtPosition(10.0f), 10);
}

TEST_F(VoxelArrayTest, IndexAtPositionUsesOriginAndScale) {
    VoxelArray A(Logger.get(), MakeBB(-2, 10, 0, 2, 14, 2), 0.5f);
    EXPECT_EQ(A.GetXIndexAtPosition(-2.0f), 0);
    EXPECT_EQ(A.GetXIndexAtPosition(-1.5f), 1);
    EXPECT_EQ(A.GetXIndexAtPosition(-1.75f), 1);  // half voxel rounds up
    EXPECT_EQ(A.GetYIndexAtPosition(11.0f), 2);
    EXPECT_EQ(A.GetYIndexAtPosition(9.0f), -2);
    EXPECT_EQ(A.GetZIndexAtPosition(1.0f), 2);
}

TEST_F(VoxelArrayTest, PositionAtIndexIsInverseOfIndexAtPosition) {
    VoxelArray A(Logger.get(), MakeBB(-2, 10, 0, 2, 14, 2), 0.5f);
    for (int I = 0; I < 8; I++) {
        EXPECT_NEAR(A.GetXPositionAtIndex(I), -2.0f + 0.5f * I, 1e-6);
        EXPECT_EQ(A.GetXIndexAtPosition(A.GetXPositionAtIndex(I)), I);
        EXPECT_EQ(A.GetYIndexAtPosition(A.GetYPositionAtIndex(I)), I);
        EXPECT_EQ(A.GetZIndexAtPosition(A.GetZPositionAtIndex(I)), I);
    }
    auto P = A.GetPositionAtIndex(2, 4, 1);
    EXPECT_NEAR(P.x, -1.0f, 1e-6); EXPECT_NEAR(P.y, 12.0f, 1e-6); EXPECT_NEAR(P.z, 0.5f, 1e-6);
}

TEST_F(VoxelArrayTest, IsInRangeIsInclusiveOfBothEnds) {
    VoxelArray A = Make();
    EXPECT_TRUE(A.IsInRangeX(0.0f)); EXPECT_TRUE(A.IsInRangeX(4.0f));
    EXPECT_FALSE(A.IsInRangeX(-0.01f)); EXPECT_FALSE(A.IsInRangeX(4.01f));
    EXPECT_TRUE(A.IsInRangeY(3.0f)); EXPECT_FALSE(A.IsInRangeY(3.5f));
    EXPECT_TRUE(A.IsInRangeZ(1.0f)); EXPECT_FALSE(A.IsInRangeZ(-1.0f));
}

TEST_F(VoxelArrayTest, CompositeSetsStateDistanceAndParent) {
    VoxelArray A = Make();
    A.CompositeVoxelAtIndex(1, 1, 1, VoxelState_INTERIOR, 2.0f, 42);
    VoxelType V = A.GetVoxel(1, 1, 1);
    EXPECT_EQ(V.State_, VoxelState_INTERIOR);
    EXPECT_EQ(V.DistanceToEdge_vox_, 2);
    EXPECT_EQ(V.ParentUID, 42u);
}

TEST_F(VoxelArrayTest, CompositeDistanceIsInVoxelsAndTruncated) {
    VoxelArray A(Logger.get(), MakeBB(0, 0, 0, 4, 4, 4), 0.5f);
    A.CompositeVoxelAtIndex(0, 0, 0, VoxelState_INTERIOR, 1.9f, 1);  // 3.8 vox -> 3
    EXPECT_EQ(A.GetVoxel(0, 0, 0).DistanceToEdge_vox_, 3);
}

TEST_F(VoxelArrayTest, CompositeLargerDistanceWins) {
    VoxelArray A = Make();
    A.CompositeVoxelAtIndex(0, 0, 0, VoxelState_INTERIOR, 5.0f, 1);
    A.CompositeVoxelAtIndex(0, 0, 0, VoxelState_INTERIOR, 2.0f, 1);
    EXPECT_EQ(A.GetVoxel(0, 0, 0).DistanceToEdge_vox_, 5);
    A.CompositeVoxelAtIndex(0, 0, 0, VoxelState_INTERIOR, 9.0f, 1);
    EXPECT_EQ(A.GetVoxel(0, 0, 0).DistanceToEdge_vox_, 9);
}

TEST_F(VoxelArrayTest, CompositeDistanceSaturatesAt255) {
    VoxelArray A = Make();
    A.CompositeVoxelAtIndex(0, 0, 0, VoxelState_INTERIOR, 255.0f, 1);
    EXPECT_EQ(A.GetVoxel(0, 0, 0).DistanceToEdge_vox_, 255);
    A.CompositeVoxelAtIndex(1, 0, 0, VoxelState_INTERIOR, 1000.0f, 1);
    EXPECT_EQ(A.GetVoxel(1, 0, 0).DistanceToEdge_vox_, 255);
    A.CompositeVoxelAtIndex(1, 0, 0, VoxelState_INTERIOR, 1.0f, 1);
    EXPECT_EQ(A.GetVoxel(1, 0, 0).DistanceToEdge_vox_, 255);
}

TEST_F(VoxelArrayTest, CompositeBlackAndWhiteAreNotOverwritten) {
    VoxelArray A = Make();
    A.CompositeVoxelAtIndex(0, 0, 0, VoxelState_BLACK, 0.0f, 1);
    A.CompositeVoxelAtIndex(0, 0, 0, VoxelState_INTERIOR, 0.0f, 2);
    EXPECT_EQ(A.GetVoxel(0, 0, 0).State_, VoxelState_BLACK);
    A.CompositeVoxelAtIndex(1, 0, 0, VoxelState_WHITE, 0.0f, 1);
    A.CompositeVoxelAtIndex(1, 0, 0, VoxelState_BORDER, 0.0f, 2);
    EXPECT_EQ(A.GetVoxel(1, 0, 0).State_, VoxelState_WHITE);
}

TEST_F(VoxelArrayTest, CompositeEmptyAndLowStatesAreOverwritten) {
    VoxelArray A = Make();
    A.CompositeVoxelAtIndex(0, 0, 0, VoxelState_INTERIOR, 0.0f, 1);
    EXPECT_EQ(A.GetVoxel(0, 0, 0).State_, VoxelState_INTERIOR);
    A.CompositeVoxelAtIndex(0, 0, 0, VoxelState_BORDER, 0.0f, 1);
    EXPECT_EQ(A.GetVoxel(0, 0, 0).State_, VoxelState_BORDER);
    A.CompositeVoxelAtIndex(0, 0, 0, VoxelState_BLACK, 0.0f, 1);
    EXPECT_EQ(A.GetVoxel(0, 0, 0).State_, VoxelState_BLACK);
}

TEST_F(VoxelArrayTest, CompositeParentUIDIsOverwrittenByLastWriter) {
    VoxelArray A = Make();
    A.CompositeVoxelAtIndex(0, 0, 0, VoxelState_INTERIOR, 5.0f, 1);
    A.CompositeVoxelAtIndex(0, 0, 0, VoxelState_INTERIOR, 1.0f, 2);
    EXPECT_EQ(A.GetVoxel(0, 0, 0).ParentUID, 2u);
    EXPECT_EQ(A.GetVoxel(0, 0, 0).DistanceToEdge_vox_, 5);  // distance keeps the max even though the UID changed
}

TEST_F(VoxelArrayTest, CompositeOutOfRangeIsNoOp) {
    VoxelArray A = Make();
    A.CompositeVoxelAtIndex(4, 0, 0, VoxelState_INTERIOR, 1.0f, 1);
    A.CompositeVoxelAtIndex(0, -1, 0, VoxelState_INTERIOR, 1.0f, 1);
    A.CompositeVoxelAtIndex(0, 0, 2, VoxelState_INTERIOR, 1.0f, 1);
    A.CompositeVoxel(100.0f, 0.0f, 0.0f, VoxelState_INTERIOR, 1.0f, 1);
    A.CompositeVoxel(-3.0f, 0.0f, 0.0f, VoxelState_INTERIOR, 1.0f, 1);
    EXPECT_EQ(CountNotEmpty(A), 0);
}

TEST_F(VoxelArrayTest, CompositeAtPositionRoundsToIndex) {
    VoxelArray A(Logger.get(), MakeBB(-2, -2, -2, 2, 2, 2), 1.0f);
    A.CompositeVoxel(0.4f, -0.4f, 1.0f, VoxelState_INTERIOR, 1.0f, 3);
    EXPECT_EQ(A.GetVoxel(2, 2, 3).ParentUID, 3u);
    EXPECT_EQ(CountNotEmpty(A), 1);
}

TEST_F(VoxelArrayTest, SetSizeShrinkChangesDimensions) {
    VoxelArray A = Make();
    EXPECT_TRUE(A.SetSize(2, 2, 2));
    int X, Y, Z;
    A.GetSize(&X, &Y, &Z);
    EXPECT_EQ(X, 2); EXPECT_EQ(Y, 2); EXPECT_EQ(Z, 2);
    EXPECT_EQ(A.GetSize(), 24u) << "allocated length does not change";
    EXPECT_EQ(A.GetVoxel(2, 0, 0).State_, VoxelState_OUT_OF_BOUNDS);
    A.SetVoxel(1, 1, 1, MakeVoxel(VoxelState_INTERIOR, 0, 3));
    EXPECT_EQ(A.GetVoxel(1, 1, 1).ParentUID, 3u);
}

TEST_F(VoxelArrayTest, SetSizeGrowIsRejectedAndKeepsSize) {
    VoxelArray A = Make();
    EXPECT_FALSE(A.SetSize(5, 3, 2));
    EXPECT_EQ(A.GetX(), 4); EXPECT_EQ(A.GetY(), 3); EXPECT_EQ(A.GetZ(), 2);
}

// Bug: dev_frame #121
TEST(VoxelArrayKnownBug, SetSizeToExactlyTheAllocatedSizeIsAccepted) {
    auto Logger = BG::NES::TestUtil::MakeSilentLogger();
    VoxelArray A(Logger.get(), MakeBB(0, 0, 0, 4, 3, 2), 1.0f);
    // Header: "set the size ... if it's less than or equal to the max array size"
    EXPECT_TRUE(A.SetSize(4, 3, 2));
    EXPECT_TRUE(A.SetSize(2, 3, 4));
}

TEST_F(VoxelArrayTest, SetSizeFromScanRegionUsesScale) {
    VoxelArray A(Logger.get(), MakeBB(0, 0, 0, 4, 4, 4), 1.0f);
    EXPECT_TRUE(A.SetSize(MakeRegion(0, 0, 0, 2, 2, 2), 1.0f));
    EXPECT_EQ(A.GetX(), 2);
    EXPECT_FALSE(A.SetSize(MakeRegion(0, 0, 0, 8, 8, 8), 1.0f));
    EXPECT_EQ(A.GetX(), 2);
}

TEST_F(VoxelArrayTest, ClearArrayResetsEveryField) {
    VoxelArray A = Make();
    FillAll(A);
    A.ClearArray();
    for (int X = 0; X < 4; X++) for (int Y = 0; Y < 3; Y++) for (int Z = 0; Z < 2; Z++) {
        VoxelType V = A.GetVoxel(X, Y, Z);
        EXPECT_EQ(V.State_, VoxelState_EMPTY);
        EXPECT_EQ(V.ParentUID, 0u);
        EXPECT_EQ(V.DistanceToEdge_vox_, 0);
    }
}

TEST_F(VoxelArrayTest, ClearArrayThreadedEvenSplitClearsState) {
    VoxelArray A = Make();  // 24 voxels
    FillAll(A);
    A.ClearArrayThreaded(4);
    EXPECT_EQ(CountNotEmpty(A), 0);
}

TEST_F(VoxelArrayTest, ClearArrayThreadedSingleThreadClearsState) {
    VoxelArray A = Make();
    FillAll(A);
    A.ClearArrayThreaded(1);
    EXPECT_EQ(CountNotEmpty(A), 0);
}

// Bug: dev_frame #117
TEST(VoxelArrayKnownBug, ClearArrayThreadedClearsTailWhenSizeNotDivisibleByThreads) {
    auto Logger = BG::NES::TestUtil::MakeSilentLogger();
    VoxelArray A(Logger.get(), MakeBB(0, 0, 0, 4, 3, 2), 1.0f);  // 24 voxels, 5 threads -> 4 left over
    FillAll(A);
    A.ClearArrayThreaded(5);
    EXPECT_EQ(CountNotEmpty(A), 0);
}

// Bug: dev_frame #117
TEST(VoxelArrayKnownBug, ClearArrayThreadedClearsEverythingWhenThreadsExceedVoxels) {
    auto Logger = BG::NES::TestUtil::MakeSilentLogger();
    VoxelArray A(Logger.get(), MakeBB(0, 0, 0, 4, 3, 2), 1.0f);  // 24 voxels, 30 threads -> step 0
    FillAll(A);
    A.ClearArrayThreaded(30);
    EXPECT_EQ(CountNotEmpty(A), 0);
}

// Bug: dev_frame #117
TEST(VoxelArrayKnownBug, ClearArrayThreadedZeroesDistanceAndParent) {
    auto Logger = BG::NES::TestUtil::MakeSilentLogger();
    VoxelArray A(Logger.get(), MakeBB(0, 0, 0, 4, 3, 2), 1.0f);
    FillAll(A);
    PoisonStack();
    A.ClearArrayThreaded(4);
    for (int X = 0; X < 4; X++) for (int Y = 0; Y < 3; Y++) for (int Z = 0; Z < 2; Z++) {
        VoxelType V = A.GetVoxel(X, Y, Z);
        EXPECT_EQ(V.ParentUID, 0u);
        EXPECT_EQ(V.DistanceToEdge_vox_, 0);
    }
}

// Bug: dev_frame #119
TEST(VoxelArrayKnownBug, SetBBWithSameSizeIsAccepted) {
    GTEST_FLAG_SET(death_test_style, "threadsafe");
    // assert(size != DataMaxLength_) fires when the sizes are equal (Debug builds abort).
    EXPECT_EXIT({
        auto Logger = BG::NES::TestUtil::MakeSilentLogger();
        VoxelArray A(Logger.get(), MakeBB(0, 0, 0, 4, 3, 2), 1.0f);
        bool Ok = A.SetBB(MakeBB(10, 10, 10, 14, 13, 12));
        std::exit(Ok ? 0 : 3);
    }, testing::ExitedWithCode(0), "");
}

// Bug: dev_frame #120
TEST(VoxelArrayKnownBug, SetVoxelAtIndexPastOneAxisDoesNotWriteElsewhere) {
    auto Logger = BG::NES::TestUtil::MakeSilentLogger();
    VoxelArray A(Logger.get(), MakeBB(0, 0, 0, 4, 3, 2), 1.0f);
    // Z == 2 is out of range; the flat index (0*6 + 0*2 + 2 = 2) is still inside the buffer.
    A.SetVoxelAtIndex(0, 0, 2, MakeVoxel(VoxelState_INTERIOR, 1, 9));
    EXPECT_EQ(CountNotEmpty(A), 0);
}

// Bug: dev_frame #120
TEST(VoxelArrayKnownBug, SetVoxelThrowsWhenOneAxisIsOutOfRange) {
    auto Logger = BG::NES::TestUtil::MakeSilentLogger();
    VoxelArray A(Logger.get(), MakeBB(0, 0, 0, 4, 3, 2), 1.0f);
    EXPECT_THROW(A.SetVoxel(0, 0, 2, MakeVoxel(VoxelState_INTERIOR, 1, 9)), std::out_of_range);
}

// Bug: dev_frame #122
TEST(VoxelArrayKnownBug, CompositeNegativeDistanceDoesNotWrapAround) {
    auto Logger = BG::NES::TestUtil::MakeSilentLogger();
    VoxelArray A(Logger.get(), MakeBB(0, 0, 0, 4, 3, 2), 1.0f);
    A.CompositeVoxelAtIndex(0, 0, 0, VoxelState_INTERIOR, -5.0f, 1);
    EXPECT_LE(A.GetVoxel(0, 0, 0).DistanceToEdge_vox_, 5);
}
