//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: Unit tests for SegmentationCompressor (dev_frame NT-14): UID blocks are compressed, written to a
    file, and decoded again with an independent decoder (TestUtil/EMTestHelpers.h) that implements the neuroglancer
    compressed_segmentation format.
*/

#include <memory>
#include <vector>

#include <gtest/gtest.h>

#include <TestUtil/EMTestHelpers.h>
#include <TestUtil/TestUtil.h>
#include <VSDA/EM/VoxelSubsystem/ImageProcessorPool/SegmentationCompression.h>

using namespace BG::NES::Simulator;
using namespace BG::NES::TestUtil;

namespace {

struct SegFixture : TempOutputRoot {
    std::unique_ptr<VoxelArray> Array;
    MicroscopeParameters Params = MakePlainParams();

    void SetUp() override {
        TempOutputRoot::SetUp();
        BoundingBox BB{};
        BB.bb_point2[0] = 4; BB.bb_point2[1] = 4; BB.bb_point2[2] = 4;
        Array = std::make_unique<VoxelArray>(Logger_.get(), BB, 1.0f);
    }
    void Set(int X, int Y, int Z, uint64_t UID, VoxelState S = VoxelState_INTERIOR) {
        VoxelType V{};
        V.State_ = S;
        V.ParentUID = UID;
        Array->SetVoxel(X, Y, Z, V);
    }
    // Compress the block [0,4) x [0,4) x [Z, Z+2) through ProcessTask and return the decoded file.
    std::vector<uint64_t> RoundTrip(int Z, ProcessingTask* Out = nullptr) {
        ProcessingTask T;
        ConfigurePlainTask(T, Array.get(), &Params, 0, 4, 0, 4, Z, 4, 4);
        T.IsSegmentation_ = true;
        T.OutputPath_ = RootPath() + "/seg/";
        T.TargetDirectory_ = T.OutputPath_;
        T.TargetFileName_ = "b.seg";
        SegmentationCompressor::ProcessTask(&T);
        EXPECT_TRUE(T.IsDone_);
        return DecodeCompressedSegmentation(ReadSegWords(T.TargetDirectory_ + T.TargetFileName_), 4, 4, 2);
    }
    static size_t Idx(int X, int Y, int Z) { return (size_t(X) * 4 + Y) * 2 + Z; }
};

} // namespace

TEST_F(SegFixture, KnownUidsSurviveTheRoundTripAcrossBlockTypes) {
    // 4x4x2 region = 2x2x1 blocks of 2x2x2: uniform / two values / three values / eight distinct values.
    for (int Z = 0; Z < 2; Z++)
        for (int Y = 0; Y < 2; Y++)
            for (int X = 0; X < 2; X++) {
                Set(X, Y, Z, 5);                                        // block (0,0): uniform
                Set(X + 2, Y, Z, (X + Y + Z) % 2 ? 6 : 5);              // block (1,0): two values
                Set(X, Y + 2, Z, 10 + (X + Y + Z) % 3);                 // block (0,1): three values
                Set(X + 2, Y + 2, Z, 100 + 4 * Z + 2 * Y + X);          // block (1,1): all distinct
            }
    std::vector<uint64_t> Got = RoundTrip(0);
    for (int Z = 0; Z < 2; Z++)
        for (int Y = 0; Y < 4; Y++)
            for (int X = 0; X < 4; X++)
                EXPECT_EQ(Got[Idx(X, Y, Z)], Array->GetVoxel(X, Y, Z).ParentUID) << X << "," << Y << "," << Z;
}

TEST_F(SegFixture, SixtyFourBitUidsAreNotTruncated) {
    Set(1, 1, 0, 0x123456789ABCull);
    Set(2, 3, 1, 0xFFFFFFFFFFFFFFF0ull);
    std::vector<uint64_t> Got = RoundTrip(0);
    EXPECT_EQ(Got[Idx(1, 1, 0)], 0x123456789ABCull);
    EXPECT_EQ(Got[Idx(2, 3, 1)], 0xFFFFFFFFFFFFFFF0ull);
    EXPECT_EQ(Got[Idx(0, 0, 0)], 0u);
}

TEST_F(SegFixture, AllEmptyBlockDecodesToZerosAndStaysTiny) {
    ProcessingTask T;
    ConfigurePlainTask(T, Array.get(), &Params, 0, 4, 0, 4, 0, 4, 4);
    T.OutputPath_ = T.TargetDirectory_ = RootPath() + "/seg/";
    T.TargetFileName_ = "e.seg";
    SegmentationCompressor::ProcessTask(&T);
    std::vector<uint32_t> Words = ReadSegWords(T.TargetDirectory_ + "e.seg");
    ASSERT_FALSE(Words.empty());
    EXPECT_EQ(Words[0], 1u);  // channel 0 starts right after the one-entry channel table
    for (uint64_t V : DecodeCompressedSegmentation(Words, 4, 4, 2)) EXPECT_EQ(V, 0u);
    EXPECT_LT(Words.size(), 24u);  // 8 block headers + one shared one-label table, not one table per voxel
}

TEST_F(SegFixture, EmptyVoxelsAreWrittenAsUidZeroEvenWithAParentUid) {
    Set(1, 1, 0, 42, VoxelState_EMPTY);
    Set(2, 2, 0, 43, VoxelState_BLACK);  // any non-EMPTY state keeps its UID
    std::vector<uint64_t> Got = RoundTrip(0);
    EXPECT_EQ(Got[Idx(1, 1, 0)], 0u);
    EXPECT_EQ(Got[Idx(2, 2, 0)], 43u);
}

TEST_F(SegFixture, ZRangePastTheArrayEndReadsUidZeroForTheMissingSlice) {
    for (int X = 0; X < 4; X++)
        for (int Y = 0; Y < 4; Y++) Set(X, Y, 3, 77);  // last slice of a 4-deep array
    std::vector<uint64_t> Got = RoundTrip(3);          // block covers z = 3 and 4 (out of bounds)
    for (int X = 0; X < 4; X++)
        for (int Y = 0; Y < 4; Y++) {
            EXPECT_EQ(Got[Idx(X, Y, 0)], 77u);
            EXPECT_EQ(Got[Idx(X, Y, 1)], 0u);
        }
}

TEST_F(SegFixture, ProcessTaskCreatesTheOutputDirectoryAndMarksTheTaskDone) {
    ProcessingTask T;
    ConfigurePlainTask(T, Array.get(), &Params, 0, 2, 0, 2, 0, 2, 2);
    T.OutputPath_ = T.TargetDirectory_ = RootPath() + "/deep/er/";
    T.TargetFileName_ = "x.seg";
    T.IsDone_ = false;
    SegmentationCompressor::ProcessTask(&T);
    EXPECT_TRUE(T.IsDone_);
    EXPECT_TRUE(std::filesystem::exists(RootPath() + "/deep/er/x.seg"));
}

TEST_F(SegFixture, ProcessTaskThrowsAndLeavesTheTaskPendingWhenTheDirectoryCannotBeCreated) {
    WriteFileBytes(RootPath() + "/blocker", "x");
    ProcessingTask T;
    ConfigurePlainTask(T, Array.get(), &Params, 0, 2, 0, 2, 0, 2, 2);
    T.OutputPath_ = T.TargetDirectory_ = RootPath() + "/blocker/sub/";
    T.TargetFileName_ = "x.seg";
    T.IsDone_ = false;
    EXPECT_THROW(SegmentationCompressor::ProcessTask(&T), std::runtime_error);
    EXPECT_FALSE(T.IsDone_);
}

TEST_F(SegFixture, ProcessTaskThrowsWhenTheFileCannotBeOpened) {
    ProcessingTask T;
    ConfigurePlainTask(T, Array.get(), &Params, 0, 2, 0, 2, 0, 2, 2);
    T.OutputPath_ = RootPath() + "/seg/";
    T.TargetDirectory_ = RootPath() + "/seg/";
    T.TargetFileName_ = "missing-dir/x.seg";  // directory part does not exist, so open() fails
    EXPECT_THROW(SegmentationCompressor::ProcessTask(&T), std::runtime_error);
    EXPECT_FALSE(T.IsDone_);
}

TEST_F(SegFixture, CompressSegmentationRegionOutputStartsWithTheChannelOffset) {
    Set(0, 0, 0, 9);
    std::vector<uint32_t> Out;
    SegmentationCompressor::CompressSegmentationRegion(*Array, 0, 2, 0, 2, 0, 2, &Out);
    ASSERT_GE(Out.size(), 4u);
    EXPECT_EQ(Out[0], 1u);
    std::vector<uint64_t> Got = DecodeCompressedSegmentation(Out, 2, 2, 2);
    EXPECT_EQ(Got[0], 9u);
    for (size_t i = 1; i < Got.size(); i++) EXPECT_EQ(Got[i], 0u);
}
