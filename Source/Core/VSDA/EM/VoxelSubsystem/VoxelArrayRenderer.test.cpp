//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: Unit tests for RenderSliceFromArray (dev_frame NT-14). The ImageProcessorPool has 0 threads, so
    it is only a capture queue: tasks are recorded in VSDAData::Tasks_ and counted, never executed.
*/

#include <memory>
#include <string>

#include <gtest/gtest.h>

#include <TestUtil/EMTestHelpers.h>
#include <TestUtil/TestUtil.h>
#include <VSDA/EM/VoxelSubsystem/VoxelArrayRenderer.h>

using namespace BG::NES::Simulator;

namespace {

struct RendererFixture : BG::NES::TestUtil::TempOutputRoot {
    std::unique_ptr<VoxelArray> Array;
    std::unique_ptr<ImageProcessorPool> Pool;  // 0 threads: capture queue
    VSDAData Data;

    void SetUp() override {
        TempOutputRoot::SetUp();
        Pool = std::make_unique<ImageProcessorPool>(Logger_.get(), 0);
        Data.Params_ = BG::NES::TestUtil::MakePlainParams();
        Data.Regions_.resize(1);
        Data.Regions_[0] = ScanRegion{};
        Data.ActiveRegionID_ = 0;
        Data.OutputUsername_ = "tester";
        MakeArray(10, 10, 4, 1.0f);
    }

    void MakeArray(float X, float Y, float Z, float Res) {
        BoundingBox BB{};
        BB.bb_point2[0] = X; BB.bb_point2[1] = Y; BB.bb_point2[2] = Z;
        Array = std::make_unique<VoxelArray>(Logger_.get(), BB, Res);
    }

    int Render(int Slice, int MaxX = 1000, int MaxY = 1000, double OffX = 0, double OffY = 0, double RegOffX = 0,
               double RegOffY = 0, int SliceOffset = 0) {
        return RenderSliceFromArray(Logger_.get(), MaxX, MaxY, &Data, Array.get(), "pfx", Slice, 1, Pool.get(), OffX, OffY,
                                    RegOffX, RegOffY, SliceOffset, nullptr);
    }
    ScanRegion& Region() { return Data.Regions_[0]; }
};

} // namespace

// Suites whose name ends in KnownBug carry the ctest label known_bug and never gate (dev_frame D8).
struct RendererFixtureKnownBug : RendererFixture {};

TEST_F(RendererFixture, ImageCountIsStepsInXTimesStepsInY) {
    // 10 voxels / 4 voxels per step = ceil(2.5) = 3 steps per axis.
    EXPECT_EQ(Render(1), 9);
    EXPECT_EQ(Data.Tasks_.size(), 9u);
    EXPECT_EQ(Pool->GetQueueSize(), 9);
    EXPECT_EQ(Region().ImageFilenames_.size(), 9u);
}

TEST_F(RendererFixture, FilenameAndIndexListsStayTheSameLength) {
    Data.Params_.ScanRegionOverlap_percent = 50.0f;
    Render(0);
    Render(1);
    EXPECT_EQ(Region().ImageFilenames_.size(), Region().ImageVoxelIndexes_.size());
    EXPECT_EQ(Region().ImageFilenames_.size(), 50u);  // 5 x 5 steps per slice, 2 slices
}

TEST_F(RendererFixture, MaxImagesCapsTheGridPerAxis) {
    EXPECT_EQ(Render(1, 2, 3), 6);
    Data.Tasks_.clear();
    EXPECT_EQ(Render(1, 1, 1), 1);
}

TEST_F(RendererFixture, OverlapShrinksTheStepNotTheImage) {
    Data.Params_.ScanRegionOverlap_percent = 50.0f;
    EXPECT_EQ(Render(1), 25);  // step = ceil(4 * 0.5) = 2 voxels, ceil(10 / 2) = 5 steps per axis
    EXPECT_EQ(Data.Tasks_[0]->VoxelEndingX - Data.Tasks_[0]->VoxelStartingX, 4);
    EXPECT_EQ(Data.Tasks_[5]->VoxelStartingX, 2);   // second X step, first Y step
    EXPECT_EQ(Data.Tasks_[1]->VoxelStartingY, 2);   // first X step, second Y step
}

TEST_F(RendererFixture, FilenameEncodesVoxelRangesAndSlice) {
    Render(2);
    // X-major order: index = XStep * TotalYSteps + YStep.
    EXPECT_EQ(Region().ImageFilenames_[0], "Renders/pfx/Slice2/0-4_0-4_2-3.png");
    EXPECT_EQ(Region().ImageFilenames_[1], "Renders/pfx/Slice2/0-4_4-8_2-3.png");
    EXPECT_EQ(Region().ImageFilenames_[5], "Renders/pfx/Slice2/4-8_8-12_2-3.png");
    EXPECT_EQ(Region().ImageFilenames_[8], "Renders/pfx/Slice2/8-12_8-12_2-3.png");
    const VoxelIndexInfo& I = Region().ImageVoxelIndexes_[5];
    EXPECT_EQ(I.StartX, 4); EXPECT_EQ(I.EndX, 8);
    EXPECT_EQ(I.StartY, 8); EXPECT_EQ(I.EndY, 12);
    EXPECT_EQ(I.StartZ, 2); EXPECT_EQ(I.EndZ, 3);
}

TEST_F(RendererFixture, TasksPointAtTheArrayAndAbsoluteOutputDirectory) {
    Render(1);
    ProcessingTask& T = *Data.Tasks_[0];
    EXPECT_EQ(T.Array_, Array.get());
    EXPECT_FALSE(T.IsSegmentation_);
    EXPECT_EQ(T.Width_px, 4);
    EXPECT_EQ(T.Height_px, 4);
    EXPECT_EQ(T.VoxelZ, 1);
    EXPECT_EQ(T.TargetFileName_, "0-4_0-4_1-2.png");
    EXPECT_EQ(T.TargetDirectory_, RootPath() + "/tester/Renders/pfx/Slice1/");
    EXPECT_TRUE(std::filesystem::is_directory(T.TargetDirectory_));  // created while planning
}

TEST_F(RendererFixture, NegativeOffsetsShiftIndicesBelowZero) {
    // Offsets are in um and divided by the voxel size: (-3 + -2) / 0.5 um = -10 voxels.
    Data.Params_.VoxelResolution_um = 0.5f;
    MakeArray(5, 5, 2, 0.5f);
    Render(0, 1, 1, -3.0, -3.0, -2.0, -2.0);
    ASSERT_EQ(Region().ImageFilenames_.size(), 1u);
    EXPECT_EQ(Region().ImageFilenames_[0], "Renders/pfx/Slice0/-10--6_-10--6_0-1.png");
    EXPECT_EQ(Region().ImageVoxelIndexes_[0].StartX, -10);
    EXPECT_EQ(Region().ImageVoxelIndexes_[0].EndX, -6);
    EXPECT_EQ(Data.Tasks_[0]->VoxelStartingX, 0);  // the task itself reads the array from voxel 0
}

TEST_F(RendererFixture, PixelsPerVoxelScalesIndexInfoButNotVoxelRanges) {
    Data.Params_.NumPixelsPerVoxel_px = 2;  // 4 px image = 2 voxels
    Render(0, 1, 1);
    EXPECT_EQ(Data.Tasks_[0]->VoxelEndingX, 2);
    EXPECT_EQ(Region().ImageVoxelIndexes_[0].EndX, 4);  // index info is in pixels
    EXPECT_EQ(Region().ImageVoxelIndexes_[0].EndY, 4);
}

TEST_F(RendererFixture, SliceThicknessDividesTheSliceNumber) {
    Data.Params_.SliceThickness_um = 2.0f;  // 2 voxels thick at 1 um
    Render(4, 1, 1, 0, 0, 0, 0, 2);          // (4 + 2) / 2 = slice 3
    EXPECT_EQ(Region().ImageFilenames_[0], "Renders/pfx/Slice3/0-4_0-4_3-4.png");
    EXPECT_EQ(Data.Tasks_[0]->VoxelZ, 4);  // the array is still read at the raw slice number
}

TEST_F(RendererFixture, NoSegmentationTasksWhenSegmentationIsOff) {
    EXPECT_EQ(Render(0), 9);
    for (auto& T : Data.Tasks_) EXPECT_FALSE(T->IsSegmentation_);
    EXPECT_TRUE(Region().SegmentationFilenames_.empty());
}

TEST_F(RendererFixture, SegmentationTasksOnlyOnEvenSlices) {
    Data.Params_.GenerateSegmentation = true;
    Region().RegionIndexInfo_.EndZ = 3;

    EXPECT_EQ(Render(0, 1, 1), 2);  // one image + one segmentation task
    ASSERT_EQ(Data.Tasks_.size(), 2u);
    EXPECT_FALSE(Data.Tasks_[0]->IsSegmentation_);
    EXPECT_TRUE(Data.Tasks_[1]->IsSegmentation_);
    EXPECT_EQ(Data.Tasks_[1]->TargetFileName_, "0-4_0-4_0-2.seg");
    ASSERT_EQ(Region().SegmentationVoxelIndexes_.size(), 1u);
    EXPECT_EQ(Region().SegmentationVoxelIndexes_[0].StartZ, 0);
    EXPECT_EQ(Region().SegmentationVoxelIndexes_[0].EndZ, 2);

    EXPECT_EQ(Render(1, 1, 1), 1);  // odd slice: image only
    EXPECT_EQ(Region().SegmentationFilenames_.size(), 1u);

    EXPECT_EQ(Render(2, 1, 1), 2);
    // The last block is clamped to the region end (EndZ = 3), not slice + 2.
    EXPECT_EQ(Region().SegmentationVoxelIndexes_[1].StartZ, 2);
    EXPECT_EQ(Region().SegmentationVoxelIndexes_[1].EndZ, 3);
    // Image list and segmentation list are independent.
    EXPECT_EQ(Region().ImageFilenames_.size(), 3u);
    EXPECT_EQ(Region().ImageFilenames_.size(), Region().ImageVoxelIndexes_.size());
}

TEST_F(RendererFixture, SegmentationTasksReadTheRegionArrayAndWriteNextToTheImages) {
    Data.Params_.GenerateSegmentation = true;
    Region().RegionIndexInfo_.EndZ = 4;
    Data.Array_ = std::make_unique<VoxelArray>(Logger_.get(), BoundingBox{}, 1.0f);
    Render(0, 1, 1);
    ProcessingTask& Seg = *Data.Tasks_[1];
    EXPECT_EQ(Seg.Array_, Data.Array_.get());
    EXPECT_EQ(Seg.TargetDirectory_, RootPath() + "/tester/Renders/pfx/Slice0/");
    EXPECT_EQ(Seg.OutputPath_, Seg.TargetDirectory_);
    EXPECT_EQ(Seg.VoxelEndingX - Seg.VoxelStartingX, 4);
    EXPECT_FALSE(Seg.IsDone_);
}

// Bug: dev_frame #142
TEST_F(RendererFixtureKnownBug, SegmentationFilenamesUseTheSameFormAsImageFilenames) {
    Data.Params_.GenerateSegmentation = true;
    Region().RegionIndexInfo_.EndZ = 4;
    Render(0, 1, 1);
    EXPECT_EQ(Region().SegmentationFilenames_[0], "Renders/pfx/Slice0/0-4_0-4_0-2.seg");
}

// Bug: dev_frame #142
TEST_F(RendererFixtureKnownBug, ImageCoversPartialVoxels) {
    // A 10 px image at 4 px per voxel spans 2.5 voxels; the code means ceil() = 3 but divides as integers first.
    Data.Params_.ImageWidth_px = 10;
    Data.Params_.ImageHeight_px = 10;
    Data.Params_.NumPixelsPerVoxel_px = 4;
    Render(0, 1, 1);
    EXPECT_EQ(Data.Tasks_[0]->VoxelEndingX - Data.Tasks_[0]->VoxelStartingX, 3);
    EXPECT_EQ(Data.Tasks_[0]->VoxelEndingY - Data.Tasks_[0]->VoxelStartingY, 3);
}
