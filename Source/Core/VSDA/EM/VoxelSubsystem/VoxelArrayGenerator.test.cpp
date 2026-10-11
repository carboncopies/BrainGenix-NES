//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: Tests for CreateVoxelArrayFromSimulation (dev_frame NT-14). The function blocks until the pool has
    drained and finished every task, so a 0-thread pool would hang as soon as one task exists; these tests use a
    1-thread pool, count tasks through VSDAData::TotalVoxelQueueLength_ and check the voxels that were written.
    Each test that queues work pays the function's 1 s queue poll at most once.
*/

#include <memory>

#include <gtest/gtest.h>

#include <TestUtil/EMTestHelpers.h>
#include <TestUtil/SimulationTestHelpers.h>
#include <TestUtil/TestUtil.h>
#include <Simulator/Structs/RecordingElectrode.h>
#include <VSDA/EM/VoxelSubsystem/VoxelArrayGenerator.h>

using namespace BG::NES::Simulator;
using namespace BG::NES::TestUtil;
using BG::NES::Simulator::VoxelArrayGenerator::ArrayGeneratorPool;

namespace {

struct GenFixture : testing::Test {
    std::unique_ptr<BG::Common::Logger::LoggingSystem> Logger = MakeSilentLogger();
    std::unique_ptr<Simulation> Sim = std::make_unique<Simulation>(Logger.get());
    MicroscopeParameters Params = MakePlainParams();
    ScanRegion Region{};
    std::unique_ptr<VoxelArray> Array;
    std::unique_ptr<ArrayGeneratorPool> Pool = std::make_unique<ArrayGeneratorPool>(Logger.get(), 1);

    GenFixture() {
        Region.Point2X_um = 32; Region.Point2Y_um = 32; Region.Point2Z_um = 8;
        BoundingBox BB{};
        BB.bb_point2[0] = 32; BB.bb_point2[1] = 32; BB.bb_point2[2] = 8;
        Array = std::make_unique<VoxelArray>(Logger.get(), BB, 1.0f);
    }

    // Compartments cache a raw pointer into the geometry vector, so every shape is added first and the compartments
    // are created in Generate() (see SimulationTestHelpers.h).
    std::vector<int> ShapeIDs;
    bool CompartmentsAdded = false;

    int AddSphere(float X, float Y, float Z, float R, int ParentID = 0) {
        Geometries::Sphere S(Geometries::Vec3D(X, Y, Z), R);
        S.ParentID = ParentID;
        ShapeIDs.push_back(Sim->AddSphere(S));
        return ShapeIDs.back();
    }
    int AddCylinder(Geometries::Vec3D A, float RA, Geometries::Vec3D B, float RB, int ParentID = 0) {
        Geometries::Cylinder C(RA, A, RB, B);
        C.ParentID = ParentID;
        ShapeIDs.push_back(Sim->AddCylinder(C));
        return ShapeIDs.back();
    }
    bool Generate() {
        if (!CompartmentsAdded) {
            for (int ShapeID : ShapeIDs) {
                Compartments::BS C = SimTest::MakeBSCompartment(ShapeID);
                if (Sim->AddSCCompartment(C, BSNEURONS) < 0) ADD_FAILURE() << "compartment add failed";
            }
            CompartmentsAdded = true;
        }
        return CreateVoxelArrayFromSimulation(Logger.get(), Sim.get(), &Params, Array.get(), Region, Pool.get());
    }
    int Tasks() const { return Sim->VSDAData_->TotalVoxelQueueLength_; }
    size_t CountState(VoxelState S) {
        size_t N = 0;
        for (int X = 0; X < 32; X++) for (int Y = 0; Y < 32; Y++) for (int Z = 0; Z < 8; Z++)
            if (Array->GetVoxel(X, Y, Z).State_ == S) N++;
        return N;
    }
};

} // namespace

// Suites whose name ends in KnownBug carry the ctest label known_bug and never gate (dev_frame D8).
struct GenFixtureKnownBug : GenFixture {};

TEST_F(GenFixture, EmptySimulationQueuesNothingAndSucceeds) {
    EXPECT_TRUE(Generate());
    EXPECT_EQ(Tasks(), 0);
    EXPECT_EQ(Pool->GetQueueSize(), 0);
    EXPECT_EQ(CountState(VoxelState_EMPTY), 32u * 32u * 8u);
}

TEST_F(GenFixture, SphereInsideTheRegionIsRasterisedWithItsParentUid) {
    AddSphere(16, 16, 4, 3.0f, 7);
    EXPECT_TRUE(Generate());
    EXPECT_EQ(Tasks(), 1);
    VoxelType Centre = Array->GetVoxel(16, 16, 4);
    EXPECT_EQ(Centre.State_, VoxelState_INTERIOR);
    EXPECT_EQ(Centre.ParentUID, 7u);
    EXPECT_EQ(Array->GetVoxel(2, 2, 1).State_, VoxelState_EMPTY);
    EXPECT_EQ(Array->GetVoxel(16, 16, 7).State_, VoxelState_EMPTY);  // 3 um away in z
}

TEST_F(GenFixture, ShapesOutsideTheRegionAreSkipped) {
    AddSphere(1000, 1000, 1000, 5.0f);
    AddSphere(-500, 16, 4, 5.0f);
    AddCylinder(Geometries::Vec3D(500, 500, 500), 1, Geometries::Vec3D(500, 520, 500), 1);
    EXPECT_TRUE(Generate());
    EXPECT_EQ(Tasks(), 0);
    EXPECT_EQ(CountState(VoxelState_EMPTY), 32u * 32u * 8u);
}

TEST_F(GenFixture, OnlyShapesInsideTheRegionAreCounted) {
    AddSphere(1000, 1000, 1000, 5.0f);
    AddSphere(16, 16, 4, 2.0f);
    EXPECT_TRUE(Generate());
    EXPECT_EQ(Tasks(), 1);
}

TEST_F(GenFixture, BigSphereIsSplitIntoSeventyFiveThousandVoxelParts) {
    // (70 / 1)^3 = 343000 estimated voxels -> ceil(343000 / 75000) = 5 parts; together they cover the whole array.
    AddSphere(16, 16, 4, 70.0f);
    EXPECT_TRUE(Generate());
    EXPECT_EQ(Tasks(), 5);
    EXPECT_EQ(CountState(VoxelState_INTERIOR), 32u * 32u * 8u);
}

TEST_F(GenFixture, SphereJustUnderTheThresholdIsOnePart) {
    AddSphere(16, 16, 4, 42.0f);  // 42^3 = 74088 voxels
    EXPECT_TRUE(Generate());
    EXPECT_EQ(Tasks(), 1);
}

TEST_F(GenFixture, VoxelSizeChangesTheEstimatedVoxelCount) {
    Params.VoxelResolution_um = 0.5f;
    BoundingBox BB{};
    BB.bb_point2[0] = 32; BB.bb_point2[1] = 32; BB.bb_point2[2] = 8;
    Array = std::make_unique<VoxelArray>(Logger.get(), BB, 0.5f);
    AddSphere(16, 16, 4, 25.0f);  // (25 / 0.5)^3 = 125000 voxels -> 2 parts
    EXPECT_TRUE(Generate());
    EXPECT_EQ(Tasks(), 2);
}

TEST_F(GenFixture, SmallCylinderIsOneSphereCapPlusOneCylinderTask) {
    AddCylinder(Geometries::Vec3D(16, 5, 4), 1.0f, Geometries::Vec3D(16, 25, 4), 1.0f, 3);
    EXPECT_TRUE(Generate());
    EXPECT_EQ(Tasks(), 2);
    EXPECT_EQ(Array->GetVoxel(16, 15, 4).State_, VoxelState_INTERIOR);
    EXPECT_EQ(Array->GetVoxel(16, 15, 4).ParentUID, 3u);
    EXPECT_EQ(Array->GetVoxel(16, 5, 4).State_, VoxelState_INTERIOR);  // the end cap sphere
    EXPECT_EQ(Array->GetVoxel(4, 15, 4).State_, VoxelState_EMPTY);
}

TEST_F(GenFixture, LongCylinderTaskCountIsTwoPerSegment) {
    // Estimated volume = (20 * 3.14159)^2 * 30 / 1 = 118,435 voxels -> 2 segments -> 2 * 2 = 4 tasks.
    AddCylinder(Geometries::Vec3D(16, 1, 4), 20.0f, Geometries::Vec3D(16, 31, 4), 20.0f);
    EXPECT_TRUE(Generate());
    EXPECT_EQ(Tasks(), 4);
}

TEST_F(GenFixture, TasksFromSeveralShapesAdd) {
    AddSphere(8, 8, 4, 2.0f);
    AddSphere(24, 24, 4, 2.0f);
    AddCylinder(Geometries::Vec3D(16, 5, 4), 1.0f, Geometries::Vec3D(16, 25, 4), 1.0f);
    EXPECT_TRUE(Generate());
    EXPECT_EQ(Tasks(), 4);
}

TEST_F(GenFixture, TearingDisabledAddsNoWedgeTasks) {
    Params.TearingEnabled = false;
    Params.TearNumPerSlice = 3;
    EXPECT_TRUE(Generate());
    EXPECT_EQ(Tasks(), 0);
}

TEST_F(GenFixture, TearingAddsTearNumSegmentsWedgesPerTearPerSlice) {
    Params.TearingEnabled = true;
    Params.TearNumPerSlice = 1;
    Params.TearNumVariation = 0;  // exactly one tear per slice
    Params.TearNumSegments = 3;
    Params.TearMinimumLength_um = 0;
    Params.TearMaxDeltaX_um = 1000;
    Params.TearMaxDeltaY_um = 1000;
    Params.TearStartSize_um = 2.0f;
    Params.TearEndSize_um = 1.0f;
    EXPECT_TRUE(Generate());
    EXPECT_EQ(Tasks(), 8 * 3);  // 8 slices * 1 tear * 3 segments
}

// Bug: dev_frame #143
TEST_F(GenFixtureKnownBug, SubVoxelSphereStillGetsATask) {
    AddSphere(16, 16, 4, 0.4f);  // (0.4 / 1)^3 = 0.064 -> 0 voxels -> 0 segments
    EXPECT_TRUE(Generate());
    EXPECT_GE(Tasks(), 1);
}

// Bug: dev_frame #143
TEST_F(GenFixtureKnownBug, SubVoxelCylinderStillGetsTasks) {
    AddCylinder(Geometries::Vec3D(16, 10, 4), 0.05f, Geometries::Vec3D(16, 11, 4), 0.05f);
    EXPECT_TRUE(Generate());
    EXPECT_GE(Tasks(), 1);
}

// Bug: dev_frame #143
TEST_F(GenFixtureKnownBug, CylinderSizeEstimateUsesPiRSquared) {
    // True volume of this cylinder: pi * 20^2 * 30 = 37,699 voxels -> 1 segment. The code squares (pi * r)
    // instead, estimates 118,435 voxels and splits it in 2, so every cylinder is split ~pi times too finely.
    AddCylinder(Geometries::Vec3D(16, 1, 4), 20.0f, Geometries::Vec3D(16, 31, 4), 20.0f);
    EXPECT_TRUE(Generate());
    EXPECT_EQ(Tasks(), 2);  // 1 segment = 1 end-cap sphere + 1 cylinder part
}
