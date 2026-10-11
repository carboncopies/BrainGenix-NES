//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: Unit tests for the Neuroglancer ConversionPool worker (dev_frame NT-14): one worker thread, source PNG
    and .seg files written by the test into a temp root. The pool never creates output directories itself.
*/

#include <memory>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include <TestUtil/EMTestHelpers.h>
#include <TestUtil/TestUtil.h>
#include <VSDA/EM/NeuroglancerConversionPool/ConversionPool/ConversionPool.h>

using namespace BG::NES;
using namespace BG::NES::TestUtil;
using ConversionPool::ProcessingTask;

namespace {

struct ConvFixture : TempOutputRoot {
    std::string Data, Seg;

    void SetUp() override {
        TempOutputRoot::SetUp();
        Data = RootPath() + "/ds/Data";
        Seg = RootPath() + "/ds/Segmentation/";
        for (int L = 0; L <= 3; L++) std::filesystem::create_directories(Data + "/ReductionLevel-" + std::to_string(L));
        std::filesystem::create_directories(Seg + "Segmentation");
    }
    std::unique_ptr<ProcessingTask> ImageTask(const std::string& Src, int Levels, int X1 = 0, int X2 = 16, int Y1 = 0, int Y2 = 8) {
        auto T = std::make_unique<ProcessingTask>();
        T->IsSegmentation_ = false;
        T->SourceFilePath_ = Src;
        T->OutputDirectoryBasePath_ = Data;
        T->ReductionLevels_ = Levels;
        T->IndexInfo_.StartX = X1; T->IndexInfo_.EndX = X2;
        T->IndexInfo_.StartY = Y1; T->IndexInfo_.EndY = Y2;
        T->IndexInfo_.StartZ = 2;  T->IndexInfo_.EndZ = 3;
        return T;
    }
    std::unique_ptr<ProcessingTask> SegTask(const std::string& Src) {
        auto T = std::make_unique<ProcessingTask>();
        T->IsSegmentation_ = true;
        T->SourceFilePath_ = Src;
        T->OutputDirectoryBasePath_ = Seg;
        T->ReductionLevels_ = 3;
        T->IndexInfo_.StartX = 0; T->IndexInfo_.EndX = 16;
        T->IndexInfo_.StartY = 8; T->IndexInfo_.EndY = 24;
        T->IndexInfo_.StartZ = 4; T->IndexInfo_.EndZ = 6;
        return T;
    }
};

} // namespace

// Suites whose name ends in KnownBug carry the ctest label known_bug and never gate (dev_frame D8).
struct ConvFixtureKnownBug : ConvFixture {};

TEST_F(ConvFixture, ImageTaskWritesOneJpegPerReductionLevelWithHalvedSizes) {
    WriteGreyPng(RootPath() + "/src.png", 16, 8, 200);
    auto T = ImageTask(RootPath() + "/src.png", 2);
    {
        ConversionPool::ConversionPool Pool(Logger_.get(), 1);
        Pool.QueueEncodeOperation(T.get());
        ASSERT_TRUE(WaitFor([&] { return T->IsDone_.load(); }));
    }
    struct Expect { const char* Name; int W, H; };
    for (Expect E : {Expect{"/ReductionLevel-0/0-16_0-8_2-3", 16, 8}, Expect{"/ReductionLevel-1/0-8_0-4_2-3", 8, 4},
                     Expect{"/ReductionLevel-2/0-4_0-2_2-3", 4, 2}}) {
        LoadedImage Img = LoadImageFile(Data + E.Name);
        ASSERT_TRUE(Img.Ok()) << E.Name;
        EXPECT_EQ(Img.W, E.W) << E.Name;
        EXPECT_EQ(Img.H, E.H) << E.Name;
        EXPECT_NEAR(Img.At(E.W / 2, E.H / 2), 200, 3) << E.Name;
    }
    EXPECT_FALSE(std::filesystem::exists(Data + "/ReductionLevel-3/0-2_0-1_2-3"));  // only ReductionLevels + 1 files
}

TEST_F(ConvFixture, ZeroReductionLevelsWritesOnlyTheFullResolutionFile) {
    WriteGreyPng(RootPath() + "/src.png", 16, 8, 100);
    auto T = ImageTask(RootPath() + "/src.png", 0);
    ConversionPool::ConversionPool Pool(Logger_.get(), 1);
    Pool.QueueEncodeOperation(T.get());
    ASSERT_TRUE(WaitFor([&] { return T->IsDone_.load(); }));
    EXPECT_TRUE(std::filesystem::exists(Data + "/ReductionLevel-0/0-16_0-8_2-3"));
    EXPECT_FALSE(std::filesystem::exists(Data + "/ReductionLevel-1/0-8_0-4_2-3"));
}

TEST_F(ConvFixture, IndexRangesAreHalvedPerLevelButZIsNot) {
    WriteGreyPng(RootPath() + "/src.png", 16, 16, 100);
    auto T = ImageTask(RootPath() + "/src.png", 1, 32, 48, 64, 80);
    ConversionPool::ConversionPool Pool(Logger_.get(), 1);
    Pool.QueueEncodeOperation(T.get());
    ASSERT_TRUE(WaitFor([&] { return T->IsDone_.load(); }));
    EXPECT_TRUE(std::filesystem::exists(Data + "/ReductionLevel-0/32-48_64-80_2-3"));
    EXPECT_TRUE(std::filesystem::exists(Data + "/ReductionLevel-1/16-24_32-40_2-3"));
}

TEST_F(ConvFixture, ThreeChannelImagesKeepTheirChannelCount) {
    WriteGreyPng(RootPath() + "/rgb.png", 8, 8, 90, 3);
    auto T = ImageTask(RootPath() + "/rgb.png", 1, 0, 8, 0, 8);
    ConversionPool::ConversionPool Pool(Logger_.get(), 1);
    Pool.QueueEncodeOperation(T.get());
    ASSERT_TRUE(WaitFor([&] { return T->IsDone_.load(); }));
    LoadedImage Img = LoadImageFile(Data + "/ReductionLevel-1/0-4_0-4_2-3");
    ASSERT_TRUE(Img.Ok());
    EXPECT_EQ(Img.C, 3);
    EXPECT_EQ(Img.W, 4);
}

TEST_F(ConvFixture, SegmentationTaskCopiesTheFileToTheSegmentationFolder) {
    WriteFileBytes(RootPath() + "/in.seg", std::string("\x01\x00\x00\x00segdata", 11));
    auto T = SegTask(RootPath() + "/in.seg");
    ConversionPool::ConversionPool Pool(Logger_.get(), 1);
    Pool.QueueEncodeOperation(T.get());
    ASSERT_TRUE(WaitFor([&] { return T->IsDone_.load(); }));
    EXPECT_EQ(ReadFileBytes(Seg + "Segmentation/0-16_8-24_4-6"), std::string("\x01\x00\x00\x00segdata", 11));
}

TEST_F(ConvFixture, SegmentationTaskWithMissingSourceStillFinishesWithoutOutput) {
    auto T = SegTask(RootPath() + "/nope.seg");
    ConversionPool::ConversionPool Pool(Logger_.get(), 1);
    Pool.QueueEncodeOperation(T.get());
    ASSERT_TRUE(WaitFor([&] { return T->IsDone_.load(); }));
    EXPECT_FALSE(std::filesystem::exists(Seg + "Segmentation/0-16_8-24_4-6"));
}

TEST_F(ConvFixture, ManyTasksAllReachDoneAndTheQueueDrains) {
    WriteFileBytes(RootPath() + "/in.seg", "x");
    std::vector<std::unique_ptr<ProcessingTask>> Tasks;
    ConversionPool::ConversionPool Pool(Logger_.get(), 1);
    for (int i = 0; i < 40; i++) {
        Tasks.push_back(SegTask(RootPath() + "/in.seg"));
        Tasks.back()->IndexInfo_.StartZ = i;  // distinct target names
        Pool.QueueEncodeOperation(Tasks.back().get());
    }
    ASSERT_TRUE(WaitFor([&] { for (auto& T : Tasks) if (!T->IsDone_) return false; return true; }));
    EXPECT_EQ(Pool.GetQueueSize(), 0);
}

TEST_F(ConvFixture, ZeroThreadPoolOnlyQueuesAndDestructsCleanly) {
    auto T = SegTask(RootPath() + "/in.seg");
    {
        ConversionPool::ConversionPool Pool(Logger_.get(), 0);
        Pool.QueueEncodeOperation(T.get());
        EXPECT_EQ(Pool.GetQueueSize(), 1);
    }
    EXPECT_FALSE(T->IsDone_);
}

TEST_F(ConvFixture, PoolWithWorkersDestructsPromptlyWhenIdle) {
    auto Start = std::chrono::steady_clock::now();
    { ConversionPool::ConversionPool Pool(Logger_.get(), 3); }
    EXPECT_LT(std::chrono::steady_clock::now() - Start, std::chrono::seconds(2));
}

// Bug: dev_frame #144
TEST_F(ConvFixtureKnownBug, MissingSourceImageDoesNotCrashAndTheTaskFinishes) {
    GTEST_FLAG_SET(death_test_style, "threadsafe");
    EXPECT_EXIT(
        {
            auto T = ImageTask(RootPath() + "/missing.png", 1);
            ConversionPool::ConversionPool Pool(Logger_.get(), 1);
            Pool.QueueEncodeOperation(T.get());
            bool Done = WaitFor([&] { return T->IsDone_.load(); }, 3000);
            _exit(Done ? 0 : 1);
        },
        ::testing::ExitedWithCode(0), "");
}
