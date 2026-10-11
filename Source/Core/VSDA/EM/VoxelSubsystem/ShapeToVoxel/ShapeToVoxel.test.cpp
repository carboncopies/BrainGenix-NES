//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: Unit tests for the EM ShapeToVoxel rasterisers (dev_frame NT-13).
    Arrays are at most 14^3 voxels. Tests in *KnownBug suites assert the correct behaviour
    and are excluded from the gate (ctest -LE known_bug).
*/

#include <cmath>
#include <memory>

#include <gtest/gtest.h>
#include <noise/noise.h>

#include <TestUtil/TestUtil.h>
#include <VSDA/EM/VoxelSubsystem/ShapeToVoxel/ShapeToVoxel.h>

using namespace BG::NES::Simulator;
using namespace BG::NES::Simulator::VoxelArrayGenerator;
using BG::NES::VSDA::WorldInfo;

namespace {

BoundingBox MakeBB(float Lo, float Hi) {
    BoundingBox BB{};
    for (int i = 0; i < 3; i++) { BB.bb_point1[i] = Lo; BB.bb_point2[i] = Hi; }
    return BB;
}

__attribute__((noinline)) void PoisonStack() {
    volatile unsigned char Buf[16384];
    for (size_t i = 0; i < sizeof(Buf); i++) Buf[i] = 0xAB;
}

struct Fixture : testing::Test {
    std::unique_ptr<BG::Common::Logger::LoggingSystem> Logger = BG::NES::TestUtil::MakeSilentLogger();
    MicroscopeParameters Params{};
    noise::module::Perlin Perlin;
    WorldInfo World{};

    Fixture() { World.VoxelScale_um = 1.0f; }

    // Array covering [Lo,Hi]^3 at 1 um; voxel index i holds world position Lo + i.
    std::unique_ptr<VoxelArray> MakeArray(float Lo, float Hi) {
        return std::make_unique<VoxelArray>(Logger.get(), MakeBB(Lo, Hi), 1.0f);
    }
};

int CountNotEmpty(VoxelArray& A) {
    int N = 0;
    for (int X = 0; X < A.GetX(); X++) for (int Y = 0; Y < A.GetY(); Y++) for (int Z = 0; Z < A.GetZ(); Z++)
        if (A.GetVoxel(X, Y, Z).State_ != VoxelState_EMPTY) N++;
    return N;
}

bool SameContents(VoxelArray& A, VoxelArray& B) {
    if (A.GetX() != B.GetX() || A.GetY() != B.GetY() || A.GetZ() != B.GetZ()) return false;
    for (int X = 0; X < A.GetX(); X++) for (int Y = 0; Y < A.GetY(); Y++) for (int Z = 0; Z < A.GetZ(); Z++) {
        VoxelType P = A.GetVoxel(X, Y, Z), Q = B.GetVoxel(X, Y, Z);
        if (P.State_ != Q.State_ || P.DistanceToEdge_vox_ != Q.DistanceToEdge_vox_ || P.ParentUID != Q.ParentUID) return false;
    }
    return true;
}

// Distance from point P to segment AB.
float DistToSegment(Geometries::Vec3D P, Geometries::Vec3D A, Geometries::Vec3D B) {
    Geometries::Vec3D AB = B - A, AP = P - A;
    float L2 = AB.Dot(AB);
    float T = L2 == 0 ? 0 : std::fmax(0.f, std::fmin(1.f, AP.Dot(AB) / L2));
    Geometries::Vec3D C = A + AB * T;
    return P.Distance(C);
}

Geometries::Cylinder MakeCylinder(Geometries::Vec3D P0, float R0, Geometries::Vec3D P1, float R1, uint64_t UID) {
    Geometries::Cylinder C(R0, P0, R1, P1);
    C.ParentID = UID;
    return C;
}

} // namespace

// ---------------------------------------------------------------- spheres

TEST_F(Fixture, SphereFillsInteriorWithParentAndDistance) {
    auto A = MakeArray(-6, 6);  // 12^3
    Geometries::Sphere S(Geometries::Vec3D(0, 0, 0), 3.0f);
    S.ParentID = 42;
    EXPECT_TRUE(FillSpherePart(1, 0, A.get(), &S, World, &Params, &Perlin));
    VoxelType Centre = A->GetVoxel(6, 6, 6);
    EXPECT_EQ(Centre.State_, VoxelState_INTERIOR);
    EXPECT_EQ(Centre.ParentUID, 42u);
    EXPECT_EQ(Centre.DistanceToEdge_vox_, 3);  // radius - 0
    EXPECT_EQ(A->GetVoxel(6, 6, 8).State_, VoxelState_INTERIOR);
    EXPECT_EQ(A->GetVoxel(6, 6, 8).DistanceToEdge_vox_, 1);  // radius - 2
    EXPECT_EQ(A->GetVoxel(0, 0, 0).State_, VoxelState_EMPTY);
    EXPECT_EQ(A->GetVoxel(6, 6, 11).State_, VoxelState_EMPTY);
}

TEST_F(Fixture, SphereWritesOnlyVoxelsWithinRadius) {
    auto A = MakeArray(-6, 6);
    Geometries::Sphere S(Geometries::Vec3D(0, 0, 0), 3.0f);
    S.ParentID = 1;
    FillSpherePart(1, 0, A.get(), &S, World, &Params, &Perlin);
    int N = 0;
    for (int X = 0; X < 12; X++) for (int Y = 0; Y < 12; Y++) for (int Z = 0; Z < 12; Z++) {
        if (A->GetVoxel(X, Y, Z).State_ == VoxelState_EMPTY) continue;
        N++;
        float D = std::sqrt(float((X - 6) * (X - 6) + (Y - 6) * (Y - 6) + (Z - 6) * (Z - 6)));
        EXPECT_LE(D, 3.0f + 1e-3) << X << "," << Y << "," << Z;
    }
    EXPECT_GT(N, 50);  // ~4/3 pi r^3 = 113
    EXPECT_LT(N, 130);
}

TEST_F(Fixture, SphereSplitIntoPartsEqualsSinglePart) {
    for (int Parts : {2, 3, 5}) {
        auto Whole = MakeArray(-6, 6), Split = MakeArray(-6, 6);
        Geometries::Sphere S(Geometries::Vec3D(0.5f, -0.5f, 0), 3.5f);
        S.ParentID = 9;
        FillSpherePart(1, 0, Whole.get(), &S, World, &Params, &Perlin);
        for (int P = 0; P < Parts; P++) FillSpherePart(Parts, P, Split.get(), &S, World, &Params, &Perlin);
        EXPECT_GT(CountNotEmpty(*Whole), 0);
        EXPECT_TRUE(SameContents(*Whole, *Split)) << Parts << " parts";
    }
}

TEST_F(Fixture, SpherePartialPartsEachWriteASubset) {
    auto Whole = MakeArray(-6, 6), Part = MakeArray(-6, 6);
    Geometries::Sphere S(Geometries::Vec3D(0, 0, 0), 3.0f);
    S.ParentID = 1;
    FillSpherePart(1, 0, Whole.get(), &S, World, &Params, &Perlin);
    FillSpherePart(3, 1, Part.get(), &S, World, &Params, &Perlin);
    EXPECT_GT(CountNotEmpty(*Part), 0);
    EXPECT_LT(CountNotEmpty(*Part), CountNotEmpty(*Whole));
}

TEST_F(Fixture, SphereClippedAtArrayEdgeOnlyWritesInside) {
    auto A = MakeArray(0, 6);
    Geometries::Sphere S(Geometries::Vec3D(0, 0, 0), 3.0f);  // an octant is inside the array
    S.ParentID = 3;
    EXPECT_TRUE(FillSpherePart(1, 0, A.get(), &S, World, &Params, &Perlin));
    EXPECT_EQ(A->GetVoxel(0, 0, 0).State_, VoxelState_INTERIOR);
    EXPECT_EQ(A->GetVoxel(2, 0, 0).State_, VoxelState_INTERIOR);
    EXPECT_EQ(A->GetVoxel(4, 4, 4).State_, VoxelState_EMPTY);
    EXPECT_GT(CountNotEmpty(*A), 0);
    EXPECT_LT(CountNotEmpty(*A), 60);  // roughly 1/8 of 113
}

TEST_F(Fixture, SphereEntirelyOutsideArrayWritesNothing) {
    auto A = MakeArray(0, 6);
    Geometries::Sphere S(Geometries::Vec3D(50, 50, 50), 3.0f);
    S.ParentID = 3;
    EXPECT_TRUE(FillSpherePart(1, 0, A.get(), &S, World, &Params, &Perlin));
    EXPECT_EQ(CountNotEmpty(*A), 0);
}

TEST_F(Fixture, SphereBiggerThanArrayFillsEveryVoxel) {
    auto A = MakeArray(0, 4);
    Geometries::Sphere S(Geometries::Vec3D(2, 2, 2), 20.0f);
    S.ParentID = 3;
    FillSpherePart(1, 0, A.get(), &S, World, &Params, &Perlin);
    EXPECT_EQ(CountNotEmpty(*A), 64);
}

TEST_F(Fixture, ZeroRadiusSphereWritesNothing) {
    auto A = MakeArray(-3, 3);
    Geometries::Sphere S(Geometries::Vec3D(0, 0, 0), 0.0f);
    S.ParentID = 3;
    EXPECT_TRUE(FillSpherePart(1, 0, A.get(), &S, World, &Params, &Perlin));
    EXPECT_EQ(CountNotEmpty(*A), 0);
}

// -------------------------------------------------------------- cylinders

TEST_F(Fixture, CylinderFillsVoxelsAroundItsAxis) {
    auto A = MakeArray(-7, 7);  // 14^3
    Geometries::Vec3D P0(-3, 0, 0), P1(3, 0, 0);
    auto C = MakeCylinder(P0, 2.0f, P1, 2.0f, 77);
    EXPECT_TRUE(FillCylinderPart(1, 0, A.get(), &C, World, &Params, &Perlin));
    int N = 0;
    for (int X = 0; X < 14; X++) for (int Y = 0; Y < 14; Y++) for (int Z = 0; Z < 14; Z++) {
        VoxelType V = A->GetVoxel(X, Y, Z);
        if (V.State_ == VoxelState_EMPTY) continue;
        N++;
        EXPECT_EQ(V.State_, VoxelState_INTERIOR);
        EXPECT_EQ(V.ParentUID, 77u);
        float D = DistToSegment(Geometries::Vec3D(X - 7, Y - 7, Z - 7), P0, P1);
        EXPECT_LE(D, 2.0f + 0.75f) << X << "," << Y << "," << Z;  // radius + rounding to the 1 um grid
    }
    EXPECT_GT(N, 40);
    // axis voxels between the ends are filled, voxels well beyond the end caps and radius are not
    for (int X = -3; X <= 3; X++) EXPECT_EQ(A->GetVoxel(X + 7, 7, 7).State_, VoxelState_INTERIOR) << X;
    EXPECT_EQ(A->GetVoxel(6 + 7, 7, 7).State_, VoxelState_EMPTY);
    EXPECT_EQ(A->GetVoxel(7, 7 + 5, 7).State_, VoxelState_EMPTY);
    EXPECT_EQ(A->GetVoxel(7, 7, 7 + 5).State_, VoxelState_EMPTY);
}

TEST_F(Fixture, CylinderAxisVoxelHasLargestDistanceToEdge) {
    auto A = MakeArray(-7, 7);
    auto C = MakeCylinder(Geometries::Vec3D(-3, 0, 0), 3.0f, Geometries::Vec3D(3, 0, 0), 3.0f, 1);
    FillCylinderPart(1, 0, A.get(), &C, World, &Params, &Perlin);
    EXPECT_EQ(A->GetVoxel(7, 7, 7).DistanceToEdge_vox_, 3);
    EXPECT_LT(A->GetVoxel(7, 7 + 2, 7).DistanceToEdge_vox_, 3);
}

TEST_F(Fixture, CylinderSplitIntoPartsEqualsSinglePart) {
    for (int Parts : {2, 3, 4}) {
        auto Whole = MakeArray(-7, 7), Split = MakeArray(-7, 7);
        auto C = MakeCylinder(Geometries::Vec3D(-3, -1, 0), 2.0f, Geometries::Vec3D(3, 2, 1), 1.0f, 5);
        FillCylinderPart(1, 0, Whole.get(), &C, World, &Params, &Perlin);
        for (int P = 0; P < Parts; P++) FillCylinderPart(Parts, P, Split.get(), &C, World, &Params, &Perlin);
        EXPECT_GT(CountNotEmpty(*Whole), 0);
        EXPECT_TRUE(SameContents(*Whole, *Split)) << Parts << " parts";
    }
}

TEST_F(Fixture, CylinderClippedAtArrayEdgeStaysInsideArray) {
    auto A = MakeArray(0, 6);
    auto C = MakeCylinder(Geometries::Vec3D(-4, 3, 3), 2.0f, Geometries::Vec3D(10, 3, 3), 2.0f, 5);
    EXPECT_TRUE(FillCylinderPart(1, 0, A.get(), &C, World, &Params, &Perlin));
    EXPECT_EQ(A->GetVoxel(0, 3, 3).State_, VoxelState_INTERIOR);
    EXPECT_EQ(A->GetVoxel(5, 3, 3).State_, VoxelState_INTERIOR);
    EXPECT_EQ(A->GetVoxel(3, 0, 3).State_, VoxelState_EMPTY);
    EXPECT_EQ(A->GetVoxel(6, 3, 3).State_, VoxelState_OUT_OF_BOUNDS);
}

TEST_F(Fixture, ZeroRadiusCylinderWritesOnlyTheAxis) {
    auto A = MakeArray(-7, 7);
    Geometries::Vec3D P0(-3, 0, 0), P1(3, 0, 0);
    auto C = MakeCylinder(P0, 0.0f, P1, 0.0f, 8);
    EXPECT_TRUE(FillCylinderPart(1, 0, A.get(), &C, World, &Params, &Perlin));
    EXPECT_GT(CountNotEmpty(*A), 0);
    for (int X = 0; X < 14; X++) for (int Y = 0; Y < 14; Y++) for (int Z = 0; Z < 14; Z++) {
        VoxelType V = A->GetVoxel(X, Y, Z);
        if (V.State_ == VoxelState_EMPTY) continue;
        EXPECT_LE(DistToSegment(Geometries::Vec3D(X - 7, Y - 7, Z - 7), P0, P1), 0.75f);
        EXPECT_EQ(V.DistanceToEdge_vox_, 0);
    }
}

TEST_F(Fixture, ZeroLengthCylinderTerminatesAndStaysNearItsPoint) {
    auto A = MakeArray(-7, 7);
    Geometries::Vec3D P(1, 1, 1);
    auto C = MakeCylinder(P, 2.0f, P, 2.0f, 8);
    EXPECT_TRUE(FillCylinderPart(1, 0, A.get(), &C, World, &Params, &Perlin));
    for (int X = 0; X < 14; X++) for (int Y = 0; Y < 14; Y++) for (int Z = 0; Z < 14; Z++)
        if (A->GetVoxel(X, Y, Z).State_ != VoxelState_EMPTY)
            EXPECT_LE(Geometries::Vec3D(X - 7, Y - 7, Z - 7).Distance(P), 2.75f);
}

// ------------------------------------------------------------------- box

TEST_F(Fixture, BoxAtOriginFillsBlackVoxelsWithParent) {
    auto A = MakeArray(-4, 4);  // 8^3, index i <-> world -4 + i
    Geometries::Box B(Geometries::Vec3D(0, 0, 0), Geometries::Vec3D(4, 4, 4));
    B.ParentID = 11;
    EXPECT_TRUE(FillBox(A.get(), &B, World, &Params, &Perlin));
    for (int X = 2; X <= 6; X++) for (int Y = 2; Y <= 6; Y++) for (int Z = 2; Z <= 6; Z++) {
        VoxelType V = A->GetVoxel(X, Y, Z);
        EXPECT_EQ(V.State_, VoxelState_BLACK);
        EXPECT_EQ(V.ParentUID, 11u);
        EXPECT_EQ(V.DistanceToEdge_vox_, 0);
    }
    EXPECT_EQ(A->GetVoxel(1, 4, 4).State_, VoxelState_EMPTY);
    EXPECT_EQ(A->GetVoxel(7, 4, 4).State_, VoxelState_EMPTY);
    EXPECT_EQ(A->GetVoxel(4, 4, 7).State_, VoxelState_EMPTY);
    EXPECT_EQ(CountNotEmpty(*A), 125);
}

TEST_F(Fixture, DegenerateBoxWritesExactlyOneVoxel) {
    auto A = MakeArray(-4, 4);
    Geometries::Box B(Geometries::Vec3D(0, 0, 0), Geometries::Vec3D(0.0f, 0.0f, 0.0f));
    B.ParentID = 11;
    EXPECT_TRUE(FillBox(A.get(), &B, World, &Params, &Perlin));
    EXPECT_EQ(CountNotEmpty(*A), 1);  // zero dims: one sample at the origin
}

// Bug: dev_frame #123
TEST(ShapeToVoxelKnownBug, FillBoxHonoursBoxCentre) {
    auto Logger = BG::NES::TestUtil::MakeSilentLogger();
    MicroscopeParameters Params{}; noise::module::Perlin Perlin; WorldInfo World{}; World.VoxelScale_um = 1.0f;
    VoxelArray A(Logger.get(), MakeBB(-4, 4), 1.0f);
    Geometries::Box B(Geometries::Vec3D(2, 2, 2), Geometries::Vec3D(2, 2, 2));  // world [1,3]^3 -> indices 5..7
    B.ParentID = 11;
    FillBox(&A, &B, World, &Params, &Perlin);
    EXPECT_EQ(A.GetVoxel(6, 6, 6).State_, VoxelState_BLACK);   // world (2,2,2), the centre
    EXPECT_EQ(A.GetVoxel(4, 4, 4).State_, VoxelState_EMPTY);   // world origin, outside the box
}

// Bug: dev_frame #123
TEST(ShapeToVoxelKnownBug, FillBoxHonoursBoxRotation) {
    auto Logger = BG::NES::TestUtil::MakeSilentLogger();
    MicroscopeParameters Params{}; noise::module::Perlin Perlin; WorldInfo World{}; World.VoxelScale_um = 1.0f;
    VoxelArray A(Logger.get(), MakeBB(-4, 4), 1.0f);
    // 4 x 0 x 0 box along X, rotated 90 degrees about Z -> lies along Y
    Geometries::Box B(Geometries::Vec3D(0, 0, 0), Geometries::Vec3D(4, 0, 0), Geometries::Vec3D(0, 0, M_PI / 2));
    B.ParentID = 11;
    FillBox(&A, &B, World, &Params, &Perlin);
    EXPECT_EQ(A.GetVoxel(4, 6, 4).State_, VoxelState_BLACK);  // world (0,2,0)
    EXPECT_EQ(A.GetVoxel(6, 4, 4).State_, VoxelState_EMPTY);  // world (2,0,0)
}

// ----------------------------------------------------------------- wedge

TEST_F(Fixture, WedgeFillsBlackVoxelsAlongItsAxis) {
    auto A = MakeArray(-7, 7);
    Geometries::Wedge W(Geometries::Vec3D(-3, 0, 0), Geometries::Vec3D(3, 0, 0), 2.0f, 2.0f, 2.0f, 2.0f);
    EXPECT_TRUE(FillWedge(A.get(), &W, World, &Params, &Perlin));
    EXPECT_GT(CountNotEmpty(*A), 10);
    for (int X = -3; X <= 3; X++) EXPECT_EQ(A->GetVoxel(X + 7, 7, 7).State_, VoxelState_BLACK) << X;
    for (int X = 0; X < 14; X++) for (int Y = 0; Y < 14; Y++) for (int Z = 0; Z < 14; Z++) {
        VoxelType V = A->GetVoxel(X, Y, Z);
        if (V.State_ == VoxelState_EMPTY) continue;
        EXPECT_EQ(V.State_, VoxelState_BLACK);
        EXPECT_LE(DistToSegment(Geometries::Vec3D(X - 7, Y - 7, Z - 7), Geometries::Vec3D(-3, 0, 0), Geometries::Vec3D(3, 0, 0)), 2.0f);
    }
    EXPECT_EQ(A->GetVoxel(7 + 6, 7, 7).State_, VoxelState_EMPTY);
}

TEST_F(Fixture, WedgeCrossSectionTapersFromEndToEnd) {
    auto A = MakeArray(-7, 7);
    Geometries::Wedge W(Geometries::Vec3D(-4, 0, 0), Geometries::Vec3D(4, 0, 0), 0.0f, 0.0f, 4.0f, 4.0f);
    FillWedge(A.get(), &W, World, &Params, &Perlin);
    auto Slice = [&](int X) { int N = 0; for (int Y = 0; Y < 14; Y++) for (int Z = 0; Z < 14; Z++) if (A->GetVoxel(X + 7, Y, Z).State_ != VoxelState_EMPTY) N++; return N; };
    EXPECT_LT(Slice(-3), Slice(3));
}

// Bug: dev_frame #124
TEST(ShapeToVoxelKnownBug, FillWedgeLeavesNoUninitialisedVoxelFields) {
    auto Logger = BG::NES::TestUtil::MakeSilentLogger();
    MicroscopeParameters Params{}; noise::module::Perlin Perlin; WorldInfo World{}; World.VoxelScale_um = 1.0f;
    VoxelArray A(Logger.get(), MakeBB(-7, 7), 1.0f);
    Geometries::Wedge W(Geometries::Vec3D(-3, 0, 0), Geometries::Vec3D(3, 0, 0), 2.0f, 2.0f, 2.0f, 2.0f);
    PoisonStack();  // FillWedge's VoxelType locals only set State_
    FillWedge(&A, &W, World, &Params, &Perlin);
    int Checked = 0;
    for (int X = 0; X < 14; X++) for (int Y = 0; Y < 14; Y++) for (int Z = 0; Z < 14; Z++) {
        VoxelType V = A.GetVoxel(X, Y, Z);
        if (V.State_ == VoxelState_EMPTY) continue;
        Checked++;
        EXPECT_EQ(V.ParentUID, 0u);               // a wedge has no parent id
        EXPECT_EQ(V.DistanceToEdge_vox_, 0);
    }
    EXPECT_GT(Checked, 0);
}
