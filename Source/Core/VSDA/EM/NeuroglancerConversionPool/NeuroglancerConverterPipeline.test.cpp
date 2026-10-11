//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: Tests for ExecuteConversionOperation with a real 1-thread ConversionPool and tiny rendered images
    (dev_frame NT-14): state transitions, written datasets, failure paths. GenerateMeshes is off except for the
    meshing failure path (the venv location is fixed in the production code and does not exist in a test run).
    The info-file contents are covered by NeuroglancerConverter.test.cpp.
*/

#include <fstream>
#include <memory>
#include <sstream>

#include <gtest/gtest.h>

#include <TestUtil/EMTestHelpers.h>
#include <TestUtil/TestUtil.h>
#include <Simulator/Structs/RecordingElectrode.h>
#include <Util/StoragePaths.h>
#include <VSDA/EM/NeuroglancerConversionPool/NeuroglancerConverter.h>

using namespace BG::NES;
using namespace BG::NES::Simulator;
using namespace BG::NES::Simulator::VSDA;
using namespace BG::NES::TestUtil;

namespace {

struct PipelineFixture : TempOutputRoot {
    std::unique_ptr<Simulation> Sim;
    std::unique_ptr<BG::NES::ConversionPool::ConversionPool> Pool;
    ScanRegion* Region = nullptr;

    void SetUp() override {
        TempOutputRoot::SetUp();
        Sim = std::make_unique<Simulation>(Logger_.get());
        Sim->OwnerUsername = "tester";
        auto& D = *Sim->VSDAData_;
        D.State_ = VSDA_CONVERSION_REQUESTED;
        D.Params_ = MakePlainParams();
        D.Params_.ImageWidth_px = 4;
        D.Params_.ImageHeight_px = 4;
        D.Regions_.push_back(ScanRegion{});
        D.ActiveRegionID_ = 0;
        Region = &D.Regions_[0];
        Region->RegionIndexInfo_ = VoxelIndexInfo{0, 8, 0, 4, 0, 2};
        Pool = std::make_unique<BG::NES::ConversionPool::ConversionPool>(Logger_.get(), 1);
    }
    void TearDown() override {
        Pool.reset();  // join workers before the tasks owned by Sim go away
        Sim.reset();
        TempOutputRoot::TearDown();
    }

    // Registers an image at x1-x2 / y 0-4 / slice 0 and writes its source PNG under the user root.
    void AddImage(int X1, int X2, uint8_t Grey) {
        const std::string Handle = "Renders/pfx/Slice0/" + std::to_string(X1) + "-" + std::to_string(X2) + "_0-4_0-1.png";
        WriteGreyPng(Sim->ResolvePath(Handle), 4, 4, Grey);
        Region->ImageFilenames_.push_back(Handle);
        Region->ImageVoxelIndexes_.push_back(VoxelIndexInfo{X1, X2, 0, 4, 0, 1});
    }
    void AddSegmentation(int X1, int X2, const std::string& Bytes) {
        const std::string Handle = "Renders/pfx/Slice0/" + std::to_string(X1) + "-" + std::to_string(X2) + "_0-4_0-2.seg";
        WriteFileBytes(Sim->ResolvePath(Handle), Bytes);
        Region->SegmentationFilenames_.push_back(Handle);
        Region->SegmentationVoxelIndexes_.push_back(VoxelIndexInfo{X1, X2, 0, 4, 0, 2});
    }
    std::string Dataset() const { return Sim->ResolvePath("NeuroglancerDatasets/" + Region->NeuroglancerDatasetHandle_); }
    bool Convert(int Levels = 1) { return ExecuteConversionOperation(Logger_.get(), Sim.get(), Pool.get(), Levels); }
};

} // namespace

TEST_F(PipelineFixture, ConvertsRenderedImagesAndSegmentationBlocksIntoTheDataset) {
    AddImage(0, 4, 180);
    AddImage(4, 8, 60);
    AddSegmentation(0, 4, "seg-a");
    AddSegmentation(4, 8, "seg-b");

    ASSERT_TRUE(Convert(1));
    EXPECT_EQ(Sim->VSDAData_->State_, VSDA_RENDER_DONE);
    ASSERT_EQ(Region->NeuroglancerDatasetHandle_.size(), 36u);  // a UUID
    const std::string DS = Dataset();

    LoadedImage Full = LoadImageFile(DS + "/Data/ReductionLevel-0/4-8_0-4_0-1");
    ASSERT_TRUE(Full.Ok());
    EXPECT_EQ(Full.W, 4);
    EXPECT_NEAR(Full.At(2, 2), 60, 3);
    LoadedImage Half = LoadImageFile(DS + "/Data/ReductionLevel-1/2-4_0-2_0-1");
    ASSERT_TRUE(Half.Ok());
    EXPECT_EQ(Half.W, 2);
    EXPECT_EQ(Half.H, 2);
    EXPECT_NEAR(Half.At(1, 1), 60, 3);

    EXPECT_EQ(ReadFileBytes(DS + "/Segmentation/Segmentation/0-4_0-4_0-2"), "seg-a");
    EXPECT_EQ(ReadFileBytes(DS + "/Segmentation/Segmentation/4-8_0-4_0-2"), "seg-b");
    EXPECT_TRUE(std::filesystem::exists(DS + "/Data/info"));
    EXPECT_TRUE(std::filesystem::exists(DS + "/Data/provenance"));
    EXPECT_EQ(Sim->VSDAData_->ConversionTasks_.size(), 4u);
    for (auto& T : Sim->VSDAData_->ConversionTasks_) EXPECT_TRUE(T->IsDone_);
}

TEST_F(PipelineFixture, StatusFieldsAreResetAndTheOperationNameIsSet) {
    auto& D = *Sim->VSDAData_;
    D.TotalSlices_ = 9; D.CurrentSlice_ = 8; D.VoxelQueueLength_ = 7; D.TotalSliceImages_ = 6;
    AddImage(0, 4, 100);
    ASSERT_TRUE(Convert(0));
    EXPECT_EQ(D.CurrentOperation_, "Converting Image Data To Neuroglancer Format");
    EXPECT_EQ(D.TotalSlices_, 0);
    EXPECT_EQ(D.CurrentSlice_, 0);
    EXPECT_EQ(D.VoxelQueueLength_, 0);
    EXPECT_EQ(D.TotalSliceImages_, 0);
    EXPECT_EQ(D.OutputUsername_, "tester");
}

TEST_F(PipelineFixture, NoImagesStillProducesAValidEmptyDataset) {
    ASSERT_TRUE(Convert(1));
    EXPECT_EQ(Sim->VSDAData_->State_, VSDA_RENDER_DONE);
    EXPECT_TRUE(Sim->VSDAData_->ConversionTasks_.empty());
    EXPECT_TRUE(std::filesystem::exists(Dataset() + "/Data/info"));
}

TEST_F(PipelineFixture, MeshingIsSkippedWhenGenerateMeshesIsOff) {
    Sim->VSDAData_->Params_.GenerateSegmentation = true;
    Sim->VSDAData_->Params_.GenerateMeshes = false;
    ASSERT_TRUE(Convert(1));
    EXPECT_FALSE(std::filesystem::exists(Sim->ResolvePath("Meshes")));
}

TEST_F(PipelineFixture, MeshingIsSkippedWithoutSegmentationEvenWhenRequested) {
    Sim->VSDAData_->Params_.GenerateSegmentation = false;
    Sim->VSDAData_->Params_.GenerateMeshes = true;
    ASSERT_TRUE(Convert(1));
    EXPECT_EQ(Sim->VSDAData_->State_, VSDA_RENDER_DONE);
}

TEST_F(PipelineFixture, WrongStateReturnsFalseAndTouchesNothing) {
    for (VSDAState S : {VSDA_NOT_INITIALIZED, VSDA_INIT_DONE, VSDA_RENDER_REQUESTED, VSDA_RENDER_IN_PROGRESS, VSDA_RENDER_DONE,
                        VSDA_CONVERSION_IN_PROGRESS, VSDA_CONVERSION_FAILED}) {
        Sim->VSDAData_->State_ = S;
        EXPECT_FALSE(Convert(1)) << S;
        EXPECT_EQ(Sim->VSDAData_->State_, S);
    }
    EXPECT_TRUE(Region->NeuroglancerDatasetHandle_.empty());
    EXPECT_FALSE(std::filesystem::exists(Sim->ResolvePath("NeuroglancerDatasets")));
}

// On every failure path the function leaves CONVERSION_IN_PROGRESS for its caller (RenderPool) to turn into
// CONVERSION_FAILED, and never publishes a dataset handle.
TEST_F(PipelineFixture, ImageListSizeMismatchFailsBeforeQueueingAnyWork) {
    AddImage(0, 4, 100);
    Region->ImageVoxelIndexes_.push_back(VoxelIndexInfo{4, 8, 0, 4, 0, 1});  // one index too many
    EXPECT_FALSE(Convert(1));
    EXPECT_EQ(Sim->VSDAData_->State_, VSDA_CONVERSION_IN_PROGRESS);
    EXPECT_TRUE(Region->NeuroglancerDatasetHandle_.empty());
    EXPECT_EQ(Pool->GetQueueSize(), 0);
    EXPECT_TRUE(Sim->VSDAData_->ConversionTasks_.empty());
}

TEST_F(PipelineFixture, SegmentationListSizeMismatchFails) {
    Region->SegmentationFilenames_.push_back("Renders/pfx/Slice0/x.seg");  // no matching index
    EXPECT_FALSE(Convert(1));
    EXPECT_EQ(Sim->VSDAData_->State_, VSDA_CONVERSION_IN_PROGRESS);
    EXPECT_TRUE(Region->NeuroglancerDatasetHandle_.empty());
}

TEST_F(PipelineFixture, UnwritableUserRootFailsWithoutAHandle) {
    Sim->OwnerUsername = "blocked";
    WriteFileBytes(RootPath() + "/blocked", "a file where the user directory should be");
    EXPECT_FALSE(Convert(1));
    EXPECT_EQ(Sim->VSDAData_->State_, VSDA_CONVERSION_IN_PROGRESS);
    EXPECT_TRUE(Region->NeuroglancerDatasetHandle_.empty());
}

TEST_F(PipelineFixture, MeshingFailureReturnsFalseAndNoHandleIsPublished) {
    // The venv path is fixed to ../venv relative to the working directory; with no python there the run must fail.
    Sim->VSDAData_->Params_.GenerateSegmentation = true;
    Sim->VSDAData_->Params_.GenerateMeshes = true;
    std::filesystem::path Old = std::filesystem::current_path();
    std::filesystem::create_directories(RootPath() + "/cwd/inner");
    std::filesystem::current_path(RootPath() + "/cwd/inner");  // ../venv does not exist here
    bool Ok = Convert(1);
    std::filesystem::current_path(Old);
    EXPECT_FALSE(Ok);
    EXPECT_EQ(Sim->VSDAData_->State_, VSDA_CONVERSION_IN_PROGRESS);
    EXPECT_TRUE(Region->NeuroglancerDatasetHandle_.empty());
    EXPECT_TRUE(std::filesystem::is_directory(Sim->ResolvePath("Meshes")));  // the mesh root was created before the attempt
}

TEST_F(PipelineFixture, ASecondConversionGetsItsOwnDatasetDirectory) {
    AddImage(0, 4, 100);
    ASSERT_TRUE(Convert(0));
    std::string First = Region->NeuroglancerDatasetHandle_;
    Sim->VSDAData_->State_ = VSDA_CONVERSION_REQUESTED;
    Sim->VSDAData_->ConversionTasks_.clear();
    ASSERT_TRUE(Convert(0));
    EXPECT_NE(First, Region->NeuroglancerDatasetHandle_);
    EXPECT_TRUE(std::filesystem::exists(Sim->ResolvePath("NeuroglancerDatasets/" + First + "/Data/info")));
}
