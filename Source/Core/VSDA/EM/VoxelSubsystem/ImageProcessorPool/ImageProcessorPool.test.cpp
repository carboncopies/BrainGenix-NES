//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: Unit tests for the EM ImageProcessorPool worker (dev_frame NT-14). One worker thread, a temp output
    root, a stub NullImage and a tiny VoxelArray. rand() is seeded before every pool is constructed (the worker takes
    its mt19937 seed from rand() at thread start). Completion is polled on Task::IsDone_ with a timeout.
*/

#include <sys/stat.h>

#include <cstdlib>
#include <memory>
#include <string>

#include <gtest/gtest.h>

#include <TestUtil/EMTestHelpers.h>
#include <TestUtil/TestUtil.h>
#include <VSDA/EM/VoxelSubsystem/ImageProcessorPool/ImageProcessorPool.h>

using namespace BG::NES::Simulator;
using namespace BG::NES::TestUtil;

namespace {

VoxelType Vox(VoxelState S, uint64_t UID = 0, uint8_t Dist = 0) {
    VoxelType V{};
    V.State_ = S;
    V.ParentUID = UID;
    V.DistanceToEdge_vox_ = Dist;
    return V;
}

struct PoolFixture : TempOutputRoot {
    static constexpr const char* kNullBytes = "NULL-IMAGE-PLACEHOLDER-BYTES";
    MicroscopeParameters Params = MakePlainParams();
    std::unique_ptr<VoxelArray> Array;
    std::string OutDir, NullPath;

    void SetUp() override {
        TempOutputRoot::SetUp();
        OutDir = RootPath() + "/out/";
        NullPath = RootPath() + "/null.png";
        WriteFileBytes(NullPath, kNullBytes);
        MakeArray(8, 8, 2);
    }
    void MakeArray(float X, float Y, float Z) {
        BoundingBox BB{};
        BB.bb_point2[0] = X; BB.bb_point2[1] = Y; BB.bb_point2[2] = Z;
        Array = std::make_unique<VoxelArray>(Logger_.get(), BB, 1.0f);
    }
    void FillSlice(int Z, VoxelState S, uint64_t UID = 0, uint8_t Dist = 0) {
        for (int X = 0; X < (int)Array->GetX(); X++)
            for (int Y = 0; Y < (int)Array->GetY(); Y++) Array->SetVoxel(X, Y, Z, Vox(S, UID, Dist));
    }
    std::unique_ptr<ProcessingTask> Task(const std::string& Name, int X0, int X1, int Y0, int Y1, int Z, int OutW, int OutH) {
        auto T = std::make_unique<ProcessingTask>();
        ConfigurePlainTask(*T, Array.get(), &Params, X0, X1, Y0, Y1, Z, OutW, OutH);
        T->TargetDirectory_ = OutDir;
        T->TargetFileName_ = Name;
        T->NullImagePath_ = NullPath;
        return T;
    }
    // Runs the given tasks through a fresh 1-thread pool; returns true if all finished in time.
    bool RunAll(std::vector<ProcessingTask*> Tasks, unsigned Seed = 7) {
        srand(Seed);
        ImageProcessorPool Pool(Logger_.get(), 1);
        for (auto* T : Tasks) Pool.QueueEncodeOperation(T);
        return WaitFor([&] {
            for (auto* T : Tasks) if (!T->IsDone_) return false;
            return true;
        });
    }
    LoadedImage Run1(ProcessingTask& T, unsigned Seed = 7) {
        EXPECT_TRUE(RunAll({&T}, Seed));
        return LoadImageFile(T.TargetDirectory_ + T.TargetFileName_);
    }
};

} // namespace

// Suites whose name ends in KnownBug carry the ctest label known_bug and never gate (dev_frame D8).
struct PoolFixtureKnownBug : PoolFixture {};

TEST_F(PoolFixture, AllEmptyRegionCopiesTheNullImageVerbatim) {
    auto T = Task("empty.png", 0, 8, 0, 8, 0, 8, 8);
    ASSERT_TRUE(RunAll({T.get()}));
    EXPECT_EQ(ReadFileBytes(OutDir + "empty.png"), kNullBytes);
}

TEST_F(PoolFixture, NullImageCopyOverwritesAnExistingTarget) {
    WriteFileBytes(OutDir + "empty.png", "stale");
    auto T = Task("empty.png", 0, 8, 0, 8, 0, 8, 8);
    ASSERT_TRUE(RunAll({T.get()}));
    EXPECT_EQ(ReadFileBytes(OutDir + "empty.png"), kNullBytes);
}

TEST_F(PoolFixture, OutputDirectoryIsCreatedWhenMissing) {
    auto T = Task("empty.png", 0, 8, 0, 8, 0, 8, 8);
    T->TargetDirectory_ = OutDir + "a/b/c/";
    ASSERT_TRUE(RunAll({T.get()}));
    EXPECT_TRUE(std::filesystem::exists(OutDir + "a/b/c/empty.png"));
}

TEST_F(PoolFixture, BlackAndWhiteVoxelsBecomeZeroAndTwoFiftyFiveWithEffectsOff) {
    for (int Y = 0; Y < 8; Y++)
        for (int X = 0; X < 8; X++) Array->SetVoxel(X, Y, 0, Vox(X < 4 ? VoxelState_BLACK : VoxelState_WHITE));
    auto T = Task("bw.png", 0, 8, 0, 8, 0, 8, 8);
    LoadedImage Img = Run1(*T);
    ASSERT_TRUE(Img.Ok());
    EXPECT_EQ(Img.C, 1);
    ASSERT_EQ(Img.W, 8);
    ASSERT_EQ(Img.H, 8);
    for (int Y = 0; Y < 8; Y++)
        for (int X = 0; X < 8; X++) EXPECT_EQ(Img.At(X, Y), X < 4 ? 0 : 255) << X << "," << Y;
}

TEST_F(PoolFixture, WorkerReadsTheSliceGivenByVoxelZ) {
    FillSlice(0, VoxelState_BLACK);
    FillSlice(1, VoxelState_WHITE);
    auto T0 = Task("z0.png", 0, 4, 0, 4, 0, 4, 4);
    auto T1 = Task("z1.png", 0, 4, 0, 4, 1, 4, 4);
    ASSERT_TRUE(RunAll({T0.get(), T1.get()}));
    EXPECT_EQ(LoadImageFile(OutDir + "z0.png").At(1, 1), 0);
    EXPECT_EQ(LoadImageFile(OutDir + "z1.png").At(1, 1), 255);
}

TEST_F(PoolFixture, SubRegionOfTheArrayIsCropped) {
    Array->SetVoxel(5, 6, 0, Vox(VoxelState_WHITE));
    auto T = Task("crop.png", 4, 8, 4, 8, 0, 4, 4);
    LoadedImage Img = Run1(*T);
    ASSERT_TRUE(Img.Ok());
    EXPECT_EQ(Img.At(1, 2), 255);            // (5-4, 6-4)
    EXPECT_EQ(Img.At(0, 0), 240);            // EMPTY voxels draw as the default light grey
    EXPECT_EQ(Img.At(2, 1), 240);
}

TEST_F(PoolFixture, OutOfBoundsVoxelsAreBlackAndEmptyVoxelsAreLightGrey) {
    Array->SetVoxel(0, 0, 0, Vox(VoxelState_WHITE));  // keeps the image from being "all empty"
    auto T = Task("oob.png", 0, 12, 0, 8, 0, 12, 8);  // X 8..11 is past the array
    LoadedImage Img = Run1(*T);
    ASSERT_TRUE(Img.Ok());
    ASSERT_EQ(Img.W, 12);
    EXPECT_EQ(Img.At(0, 0), 255);
    EXPECT_EQ(Img.At(3, 3), 240);
    EXPECT_EQ(Img.At(8, 0), 0);
    EXPECT_EQ(Img.At(11, 7), 0);
}

TEST_F(PoolFixture, OutputIsResizedToTheRequestedPixelDimensions) {
    FillSlice(0, VoxelState_WHITE);
    auto T = Task("big.png", 0, 8, 0, 8, 0, 32, 16);
    LoadedImage Img = Run1(*T);
    ASSERT_TRUE(Img.Ok());
    EXPECT_EQ(Img.W, 32);
    EXPECT_EQ(Img.H, 16);
    EXPECT_EQ(Img.At(16, 8), 255);

    auto Small = Task("small.png", 0, 8, 0, 8, 0, 2, 3);
    LoadedImage S = Run1(*Small);
    EXPECT_EQ(S.W, 2);
    EXPECT_EQ(S.H, 3);
}

TEST_F(PoolFixture, ResizedHardEdgeKeepsFlatAreasAtTheirOriginalLevels) {
    for (int Y = 0; Y < 8; Y++)
        for (int X = 0; X < 8; X++) Array->SetVoxel(X, Y, 0, Vox(X < 4 ? VoxelState_BLACK : VoxelState_WHITE));
    auto T = Task("edge.png", 0, 8, 0, 8, 0, 32, 32);
    LoadedImage Img = Run1(*T);
    ASSERT_EQ(Img.W, 32);
    EXPECT_EQ(Img.At(2, 16), 0);
    EXPECT_EQ(Img.At(29, 16), 255);
}

TEST_F(PoolFixture, InteriorVoxelsUseTheDefaultIntensityWithoutNoise) {
    FillSlice(0, VoxelState_INTERIOR, 1, 5);
    Params.DefaultIntensity_ = 123;
    auto T = Task("interior.png", 0, 4, 0, 4, 0, 4, 4);
    LoadedImage Img = Run1(*T);
    ASSERT_TRUE(Img.Ok());
    for (int Y = 0; Y < 4; Y++)
        for (int X = 0; X < 4; X++) EXPECT_EQ(Img.At(X, Y), 123);
}

TEST_F(PoolFixture, BorderVoxelsAtDistanceZeroTakeTheBorderEdgeIntensity) {
    FillSlice(0, VoxelState_INTERIOR, 1, 9);
    Array->SetVoxel(1, 1, 0, Vox(VoxelState_INTERIOR, 1, 0));
    Params.DefaultIntensity_ = 200;
    Params.RenderBorders = true;
    Params.BorderEdgeIntensity = 75;
    auto T = Task("border.png", 0, 4, 0, 4, 0, 4, 4);
    LoadedImage Img = Run1(*T);
    EXPECT_EQ(Img.At(1, 1), 75);
    EXPECT_EQ(Img.At(2, 2), 200);
}

TEST_F(PoolFixture, PerlinNoiseDarkensInteriorVoxelsWithinTheConfiguredIntensity) {
    FillSlice(0, VoxelState_INTERIOR, 1, 9);
    noise::module::Perlin Gen;
    Gen.SetSeed(3);
    Params.GeneratePerlinNoise_ = true;
    Params.NoiseIntensity_ = 100;
    Params.DefaultIntensity_ = 250;
    Params.SpatialScale_ = 0.37f;
    auto T = Task("perlin.png", 0, 8, 0, 8, 0, 8, 8);
    T->Generator_ = &Gen;
    LoadedImage Img = Run1(*T);
    ASSERT_TRUE(Img.Ok());
    // Expected pixel = DefaultIntensity - (noise / 2 + 0.5) * NoiseIntensity, truncated to uint8, from the same generator.
    int Lo = 255, Hi = 0;
    for (int Y = 0; Y < 8; Y++)
        for (int X = 0; X < 8; X++) {
            float Px = Array->GetXPositionAtIndex(X), Py = Array->GetYPositionAtIndex(Y), Pz = Array->GetZPositionAtIndex(0);
            double N = Gen.GetValue(Px * 0.37f, Py * 0.37f, Pz * 0.37f);
            double Expected = std::min(255., std::max(0., 250. - (N / 2. + 0.5) * 100.));
            EXPECT_NEAR(Img.At(X, Y), Expected, 1.01) << X << "," << Y;
            Lo = std::min<int>(Lo, Img.At(X, Y));
            Hi = std::max<int>(Hi, Img.At(X, Y));
        }
    EXPECT_GT(Hi, Lo + 10);  // not flat
}

TEST_F(PoolFixture, ContrastAdjustmentMapsLevelsAroundMidGrey) {
    for (int Y = 0; Y < 4; Y++)
        for (int X = 0; X < 4; X++) Array->SetVoxel(X, Y, 0, Vox(X < 2 ? VoxelState_BLACK : VoxelState_WHITE));
    auto T = Task("contrast.png", 0, 4, 0, 4, 0, 4, 4);
    T->AdjustContrast = true;
    T->Contrast = 0.5f;
    T->Brightness = 10.0f;
    T->ContrastRandomAmount = 0.0f;
    T->BrightnessRandomAmount = 0.0f;
    LoadedImage Img = Run1(*T);
    EXPECT_EQ(Img.At(0, 0), 74);   // 0.5 * (0 - 128) + 128 + 10
    EXPECT_EQ(Img.At(3, 3), 201);  // 0.5 * (255 - 128) + 128 + 10 = 201.5, truncated
}

TEST_F(PoolFixture, ImageNoiseStaysWithinItsAmplitudeAndIsReproducibleForASeed) {
    MakeArray(16, 16, 1);
    FillSlice(0, VoxelState_WHITE);
    auto Make = [&](const char* Name) {
        auto T = Task(Name, 0, 16, 0, 16, 0, 16, 16);
        T->EnableImageNoise = true;
        T->ImageNoiseAmount = 10;
        T->PreBlurNoisePasses = 1;
        return T;
    };
    auto A = Make("na.png"), B = Make("nb.png"), C = Make("nc.png");
    LoadedImage Ia = Run1(*A, 11), Ib = Run1(*B, 11), Ic = Run1(*C, 12);
    ASSERT_TRUE(Ia.Ok());
    bool AnyChanged = false;
    for (auto P : Ia.Px) {
        EXPECT_GE(P, 250);  // 255 + (r % 10 - 5), r % 10 in 0..9, clamped to 255
        if (P != 255) AnyChanged = true;
    }
    EXPECT_TRUE(AnyChanged);
    EXPECT_EQ(Ia.Px, Ib.Px);  // same srand seed -> same worker seed -> same pixels
    EXPECT_NE(Ia.Px, Ic.Px);  // different seed -> different noise
}

TEST_F(PoolFixture, GaussianBlurSoftensAHardEdge) {
    MakeArray(16, 16, 1);
    for (int Y = 0; Y < 16; Y++)
        for (int X = 0; X < 16; X++) Array->SetVoxel(X, Y, 0, Vox(X < 8 ? VoxelState_BLACK : VoxelState_WHITE));
    auto T = Task("blur.png", 0, 16, 0, 16, 0, 16, 16);
    T->EnableGaussianBlur = true;
    T->GaussianBlurSigma = 1.5f;
    LoadedImage Img = Run1(*T);
    ASSERT_TRUE(Img.Ok());
    EXPECT_GT(Img.At(7, 8), 0);
    EXPECT_LT(Img.At(8, 8), 255);
    EXPECT_LT(Img.At(7, 8), Img.At(8, 8));
    EXPECT_LE(Img.At(0, 8), 5);     // far from the edge the levels are (nearly) unchanged
    EXPECT_GE(Img.At(15, 8), 250);
}

TEST_F(PoolFixture, InterferencePatternAddsBiasAndStaysInRange) {
    FillSlice(0, VoxelState_INTERIOR, 1, 9);
    Params.DefaultIntensity_ = 100;
    auto T = Task("interf.png", 0, 8, 0, 8, 0, 8, 8);
    T->EnableInterferencePattern = true;
    T->InterferencePatternAmplitude = 0.0f;  // no sinusoid, only the bias
    T->InterferencePatternBias = -15.0f;
    T->InterferencePatternStrengthVariation = 0.0f;
    LoadedImage Img = Run1(*T);
    for (auto P : Img.Px) EXPECT_EQ(P, 85);
}

TEST_F(PoolFixture, TasksAreProcessedAndTheQueueDrains) {
    FillSlice(0, VoxelState_WHITE);
    std::vector<std::unique_ptr<ProcessingTask>> Tasks;
    std::vector<ProcessingTask*> Raw;
    for (int i = 0; i < 12; i++) {
        Tasks.push_back(Task("t" + std::to_string(i) + ".png", 0, 8, 0, 8, 0, 4, 4));
        Raw.push_back(Tasks.back().get());
    }
    srand(1);
    ImageProcessorPool Pool(Logger_.get(), 1);
    for (auto* T : Raw) Pool.QueueEncodeOperation(T);
    ASSERT_TRUE(WaitFor([&] { for (auto* T : Raw) if (!T->IsDone_) return false; return true; }));
    EXPECT_EQ(Pool.GetQueueSize(), 0);
    for (int i = 0; i < 12; i++) EXPECT_TRUE(std::filesystem::exists(OutDir + "t" + std::to_string(i) + ".png"));
}

TEST_F(PoolFixture, ZeroThreadPoolOnlyQueues) {
    ImageProcessorPool Pool(Logger_.get(), 0);
    auto T = Task("never.png", 0, 8, 0, 8, 0, 8, 8);
    Pool.QueueEncodeOperation(T.get());
    EXPECT_EQ(Pool.GetQueueSize(), 1);
    EXPECT_FALSE(T->IsDone_);
}

TEST_F(PoolFixture, SegmentationTaskWritesTheSegFileAndColoursByParentUid) {
    Params.GenerateSegmentationPNGs = true;
    Array->SetVoxel(1, 2, 0, Vox(VoxelState_INTERIOR, 5));
    Array->SetVoxel(2, 2, 0, Vox(VoxelState_BLACK, 0));        // UID 0 -> black
    Array->SetVoxel(3, 3, 0, Vox(VoxelState_EMPTY, 9));        // EMPTY -> black even with a UID
    auto T = Task("block.seg", 0, 4, 0, 4, 0, 4, 4);
    T->IsSegmentation_ = true;
    T->OutputPath_ = OutDir;
    ASSERT_TRUE(RunAll({T.get()}));
    EXPECT_TRUE(std::filesystem::exists(OutDir + "block.seg"));
    LoadedImage Img = LoadImageFile(OutDir + "block.seg.seg.png");
    ASSERT_TRUE(Img.Ok());
    EXPECT_EQ(Img.C, 3);
    EXPECT_EQ(Img.W, 4);
    EXPECT_EQ(Img.H, 4);
    EXPECT_EQ(Img.At(1, 2, 0), (5 * 9301 + 49297) % 256);
    EXPECT_EQ(Img.At(1, 2, 1), (5 * 8303 + 49299) % 256);
    EXPECT_EQ(Img.At(1, 2, 2), (5 * 7307 + 49303) % 256);
    for (int Ch = 0; Ch < 3; Ch++) {
        EXPECT_EQ(Img.At(2, 2, Ch), 0);
        EXPECT_EQ(Img.At(3, 3, Ch), 0);
        EXPECT_EQ(Img.At(0, 0, Ch), 0);
    }
}

TEST_F(PoolFixture, SegmentationPngIsNotWrittenWhenDisabled) {
    Params.GenerateSegmentationPNGs = false;
    auto T = Task("block.seg", 0, 4, 0, 4, 0, 4, 4);
    T->IsSegmentation_ = true;
    T->OutputPath_ = OutDir;
    ASSERT_TRUE(RunAll({T.get()}));
    EXPECT_TRUE(std::filesystem::exists(OutDir + "block.seg"));
    EXPECT_FALSE(std::filesystem::exists(OutDir + "block.seg.seg.png"));
}

// ---- Known bugs: each runs the scenario in a death-test child, so a crash fails only this test. ----

namespace {
// Runs Scenario in the child; exit code 0 means the pool survived and finished the task.
template <typename F>
void ExpectSurvives(const std::string& Root, F Scenario) {
    GTEST_FLAG_SET(death_test_style, "threadsafe");
    EXPECT_EXIT({ ArmChildCleanup(Root); Scenario(); }, ::testing::ExitedWithCode(0), "");
}
} // namespace

// Bug: dev_frame #138
TEST_F(PoolFixtureKnownBug, MissingNullImageDoesNotKillTheProcess) {
    ExpectSurvives(RootPath(), [&] {
        auto T = Task("empty.png", 0, 8, 0, 8, 0, 8, 8);
        T->NullImagePath_ = RootPath() + "/does-not-exist.png";
        srand(1);
        ImageProcessorPool Pool(Logger_.get(), 1);
        Pool.QueueEncodeOperation(T.get());
        bool Done = WaitFor([&] { return T->IsDone_.load(); }, 3000);
        ChildExit(Done ? 0 : 1);
    });
}

// Bug: dev_frame #138
TEST_F(PoolFixtureKnownBug, UnwritableSegmentationOutputDoesNotKillTheProcess) {
    ExpectSurvives(RootPath(), [&] {
        WriteFileBytes(RootPath() + "/blocker", "x");  // a file where the output directory should go
        auto T = Task("block.seg", 0, 4, 0, 4, 0, 4, 4);
        T->IsSegmentation_ = true;
        T->OutputPath_ = RootPath() + "/blocker/sub/";
        T->TargetDirectory_ = T->OutputPath_;
        srand(1);
        ImageProcessorPool Pool(Logger_.get(), 1);
        Pool.QueueEncodeOperation(T.get());
        // Either outcome is acceptable as long as the process survives and the task is marked finished.
        bool Done = WaitFor([&] { return T->IsDone_.load(); }, 3000);
        ChildExit(Done ? 0 : 1);
    });
}

// Bug: dev_frame #139
TEST_F(PoolFixtureKnownBug, ZeroImageNoiseIntensityDoesNotCrash) {
    ExpectSurvives(RootPath(), [&] {
        FillSlice(0, VoxelState_WHITE);
        auto T = Task("n0.png", 0, 8, 0, 8, 0, 8, 8);
        T->EnableImageNoise = true;
        T->ImageNoiseAmount = 0;
        T->PreBlurNoisePasses = 1;
        srand(1);
        ImageProcessorPool Pool(Logger_.get(), 1);
        Pool.QueueEncodeOperation(T.get());
        bool Done = WaitFor([&] { return T->IsDone_.load(); }, 3000);
        ChildExit(Done ? 0 : 1);
    });
}

// Bug: dev_frame #140
TEST_F(PoolFixtureKnownBug, InterferenceAmplitudeVariesByPlusMinusStrengthVariation) {
    // sin(pi/2 * x) is 1 at x = 1 um and 0 at x = 0, so pixel(1) - pixel(0) is this image's amplitude.
    MakeArray(4, 1, 1);
    FillSlice(0, VoxelState_INTERIOR, 1, 9);
    Params.DefaultIntensity_ = 100;
    const float Amp = 20.0f, Var = 0.1f;
    int MaxDelta = 0;
    for (unsigned Seed = 1; Seed <= 25; Seed++) {
        auto T = Task("ia" + std::to_string(Seed) + ".png", 0, 4, 0, 1, 0, 4, 1);
        T->EnableInterferencePattern = true;
        T->InterferencePatternAmplitude = Amp;
        T->InterferencePatternBias = 0.0f;
        T->InterferencePatternXScale_um = float(M_PI / 2.0);
        T->InterferencePatternYAxisWobbleIntensity = 0.0f;
        T->InterferencePatternStrengthVariation = Var;
        T->InterferencePatternZOffsetShift = false;
        LoadedImage Img = Run1(*T, Seed);
        ASSERT_TRUE(Img.Ok());
        MaxDelta = std::max<int>(MaxDelta, int(Img.At(1, 0)) - int(Img.At(0, 0)));
    }
    EXPECT_LE(MaxDelta, int(Amp * (1.0f + Var)) + 1);  // intended range is Amp * (1 +- Var) = 18..22
}

// Bug: dev_frame #141
TEST_F(PoolFixtureKnownBug, BorderThicknessInMicronsIsComparedInMicrons) {
    // 0.5 um voxels: a voxel 1 voxel (0.5 um) from the edge lies inside a 0.75 um border.
    FillSlice(0, VoxelState_INTERIOR, 1, 1);
    Params.DefaultIntensity_ = 200;
    Params.RenderBorders = true;
    Params.BorderEdgeIntensity = 75;
    Params.BorderThickness_um = 0.75f;
    Params.VoxelResolution_um = 0.5f;
    auto T = Task("bu.png", 0, 4, 0, 4, 0, 4, 4);
    T->VoxelScale_um = 0.5f;
    LoadedImage Img = Run1(*T);
    EXPECT_LT(Img.At(1, 1), 200);
}
