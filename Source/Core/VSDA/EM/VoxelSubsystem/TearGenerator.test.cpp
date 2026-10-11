//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: Unit tests for GenerateTear (dev_frame NT-13). The pool has 0 threads so queued
    tasks are only recorded (they are also in the caller's task list); every RNG use is seeded.
*/

#include <memory>
#include <vector>

#include <gtest/gtest.h>

#include <TestUtil/TestUtil.h>
#include <VSDA/EM/VoxelSubsystem/TearGenerator.h>

using namespace BG::NES::Simulator;
using VoxelArrayGenerator::ArrayGeneratorPool;
using VoxelArrayGenerator::Task;

namespace {

struct TearFixture : testing::Test {
    std::unique_ptr<BG::Common::Logger::LoggingSystem> Logger = BG::NES::TestUtil::MakeSilentLogger();
    ArrayGeneratorPool Pool{Logger.get(), 0};  // capture queue, no worker threads
    MicroscopeParameters Params{};
    ScanRegion Region{};
    BG::NES::VSDA::WorldInfo Info{};
    std::unique_ptr<VoxelArray> Array;
    std::vector<std::unique_ptr<Task>> Tasks;

    // 100 x 100 x 1 voxels at 1 um.
    TearFixture() {
        BoundingBox BB{};
        BB.bb_point2[0] = 100; BB.bb_point2[1] = 100; BB.bb_point2[2] = 1;
        Array = std::make_unique<VoxelArray>(Logger.get(), BB, 1.0f);
        Params.VoxelResolution_um = 1.0f;
        Params.TearNumSegments = 5;
        Params.TearMinimumLength_um = 0;
        Params.TearMaxDeltaX_um = 1000;
        Params.TearMaxDeltaY_um = 1000;
        Params.TearPointJitterXMin_um = 0; Params.TearPointJitterXMax_um = 0;
        Params.TearPointJitterYMin_um = 0; Params.TearPointJitterYMax_um = 0;
        Params.TearStartSize_um = 4.0f;
        Params.TearEndSize_um = 0.0f;
        Region.Point1X_um = 10; Region.Point1Y_um = 20; Region.Point1Z_um = 30;
        Region.Point2X_um = 110; Region.Point2Y_um = 120; Region.Point2Z_um = 31;
        Info.VoxelScale_um = 1.0f;
    }

    int Run(int Seed, int ZHeight = 0) {
        return GenerateTear(Logger.get(), Tasks, &Pool, Region, &Params, Array.get(), Info, ZHeight, Seed);
    }
};

// Flattened end points (and heights) of every wedge, for comparing runs.
std::vector<float> Signature(const std::vector<std::unique_ptr<Task>>& T) {
    std::vector<float> S;
    for (auto& P : T) {
        auto& W = P->ThisWedge;
        for (float F : {W.End0Pos_um.x, W.End0Pos_um.y, W.End0Pos_um.z, W.End1Pos_um.x, W.End1Pos_um.y, W.End1Pos_um.z,
                        W.End0Height_um, W.End1Height_um, W.End0Width_um, W.End1Width_um})
            S.push_back(F);
    }
    return S;
}

} // namespace

TEST_F(TearFixture, ReturnsOneTaskPerSegmentAndQueuesThemAll) {
    EXPECT_EQ(Run(1), 5);
    EXPECT_EQ(Tasks.size(), 5u);
    EXPECT_EQ(Pool.GetQueueSize(), 5);
}

TEST_F(TearFixture, TaskCountFollowsNumSegments) {
    Params.TearNumSegments = 12;
    EXPECT_EQ(Run(3), 12);
    EXPECT_EQ(Tasks.size(), 12u);
    Params.TearNumSegments = 1;
    EXPECT_EQ(Run(3), 1);
    EXPECT_EQ(Tasks.size(), 13u);  // appends to the caller's list
}

TEST_F(TearFixture, AllTasksAreCustomWedgesOnTheGivenArray) {
    Run(2);
    for (auto& T : Tasks) {
        EXPECT_EQ(T->CustomShape_, VoxelArrayGenerator::CUSTOM_WEDGE);
        EXPECT_EQ(T->Array_, Array.get());
        EXPECT_EQ(T->Parameters_, &Params);
        EXPECT_FLOAT_EQ(T->WorldInfo_.VoxelScale_um, 1.0f);
    }
}

TEST_F(TearFixture, SameSeedGivesSameTasks) {
    Run(1234);
    std::vector<std::unique_ptr<Task>> First = std::move(Tasks);
    Tasks.clear();
    Run(1234);
    EXPECT_EQ(Signature(First), Signature(Tasks));
}

TEST_F(TearFixture, DifferentSeedsGiveDifferentTears) {
    Run(1);
    auto A = Signature(Tasks);
    Tasks.clear();
    Run(2);
    EXPECT_NE(A, Signature(Tasks));
}

TEST_F(TearFixture, WedgesAreChainedAndSitAtTheRequestedZ) {
    Run(7, 3);
    ASSERT_EQ(Tasks.size(), 5u);
    for (size_t i = 0; i < Tasks.size(); i++) {
        auto& W = Tasks[i]->ThisWedge;
        EXPECT_FLOAT_EQ(W.End0Pos_um.z, 30 + 3);
        EXPECT_FLOAT_EQ(W.End1Pos_um.z, 30 + 3);
        if (i > 0) {
            auto& Prev = Tasks[i - 1]->ThisWedge;
            EXPECT_FLOAT_EQ(W.End0Pos_um.x, Prev.End1Pos_um.x);
            EXPECT_FLOAT_EQ(W.End0Pos_um.y, Prev.End1Pos_um.y);
        }
    }
}

TEST_F(TearFixture, TearStartsJustOutsideTopOrBottomAndFirstSizeIsStartSize) {
    for (int Seed = 0; Seed < 20; Seed++) {
        Tasks.clear();
        Run(Seed);
        auto& W = Tasks.front()->ThisWedge;
        float StartY = W.End0Pos_um.y - Region.Point1Y_um;
        EXPECT_TRUE(StartY == -100.0f || StartY == 100.0f + Array->GetY()) << "seed " << Seed << " y=" << StartY;
        EXPECT_FLOAT_EQ(W.End0Height_um, 4.0f);
        EXPECT_NEAR(W.End0Width_um, 1.1f, 1e-5);
        EXPECT_NEAR(W.End1Width_um, 1.1f, 1e-5);
    }
}

TEST_F(TearFixture, HeightsShrinkMonotonicallyFromStartToEnd) {
    Run(5);
    for (auto& T : Tasks) EXPECT_GT(T->ThisWedge.End0Height_um, T->ThisWedge.End1Height_um);
}

TEST_F(TearFixture, UnsatisfiableConstraintsStillTerminateWithAllTasks) {
    // StartY is pinned 100 voxels outside the array, so a 1 voxel MaxDeltaY can never be met:
    // the 1,000,000 iteration cap is hit and the tear is generated anyway.
    Params.TearMaxDeltaY_um = 1;
    EXPECT_EQ(Run(9), 5);
    EXPECT_EQ(Tasks.size(), 5u);
}

// Bug: dev_frame #125
TEST(TearGeneratorKnownBug, LastSegmentEndsAtTearEndSize) {
    auto Logger = BG::NES::TestUtil::MakeSilentLogger();
    ArrayGeneratorPool Pool(Logger.get(), 0);
    MicroscopeParameters Params{};
    BoundingBox BB{}; BB.bb_point2[0] = 100; BB.bb_point2[1] = 100; BB.bb_point2[2] = 1;
    VoxelArray Array(Logger.get(), BB, 1.0f);
    Params.VoxelResolution_um = 1.0f; Params.TearNumSegments = 5; Params.TearMaxDeltaX_um = 1000; Params.TearMaxDeltaY_um = 1000;
    Params.TearStartSize_um = 4.0f; Params.TearEndSize_um = 1.0f;
    ScanRegion Region{}; BG::NES::VSDA::WorldInfo Info{}; Info.VoxelScale_um = 1.0f;
    std::vector<std::unique_ptr<Task>> Tasks;
    GenerateTear(Logger.get(), Tasks, &Pool, Region, &Params, &Array, Info, 0, 1);
    ASSERT_EQ(Tasks.size(), 5u);
    EXPECT_NEAR(Tasks.back()->ThisWedge.End1Height_um, 1.0f, 1e-4);
}

// Bug: dev_frame #125
TEST(TearGeneratorKnownBug, EndPointStaysInsideTheArray) {
    auto Logger = BG::NES::TestUtil::MakeSilentLogger();
    ArrayGeneratorPool Pool(Logger.get(), 0);
    MicroscopeParameters Params{};
    BoundingBox BB{}; BB.bb_point2[0] = 4; BB.bb_point2[1] = 4; BB.bb_point2[2] = 1;
    VoxelArray Array(Logger.get(), BB, 1.0f);  // 4 x 4 voxels: valid indices 0..3
    Params.VoxelResolution_um = 1.0f; Params.TearNumSegments = 1; Params.TearMaxDeltaX_um = 1000; Params.TearMaxDeltaY_um = 1000;
    Params.TearStartSize_um = 1.0f; Params.TearEndSize_um = 1.0f;  // jitter is zero-initialised
    ScanRegion Region{}; BG::NES::VSDA::WorldInfo Info{}; Info.VoxelScale_um = 1.0f;
    for (int Seed = 0; Seed < 200; Seed++) {
        std::vector<std::unique_ptr<Task>> Tasks;
        GenerateTear(Logger.get(), Tasks, &Pool, Region, &Params, &Array, Info, 0, Seed);
        ASSERT_EQ(Tasks.size(), 1u);
        EXPECT_LT(Tasks[0]->ThisWedge.End1Pos_um.x, 4.0f) << "seed " << Seed;
    }
}
