//=================================//
// This file is part of BrainGenix //
//=================================//

#include <gtest/gtest.h>

#include <VSDA/EM/VoxelSubsystem/ImageProcessorPool/AppleGaussianBlur.h>
#include <VSDA/EM/VoxelSubsystem/ShapeToVoxel/MetalVoxelFill.h>
#include <VSDA/EM/VoxelSubsystem/Structs/VoxelArray.h>

#include <BG/Common/Logger/Logger.h>

#include <cstdint>
#include <vector>

namespace {

TEST(AppleGaussianBlur, AcceleratePreservesConstantImage) {
    constexpr int Width = 16;
    constexpr int Height = 12;
    std::vector<uint8_t> Pixels(Width * Height, 127);

    ASSERT_TRUE(Accelerate_GaussianBlur(Pixels.data(), Width, Height, 3.0f));
    for (uint8_t Pixel : Pixels) {
        EXPECT_EQ(Pixel, 127);
    }
}

TEST(AppleGaussianBlur, MPSPreservesConstantImageWhenAvailable) {
    constexpr int Width = 16;
    constexpr int Height = 12;
    std::vector<uint8_t> Pixels(Width * Height, 127);

    if (!MPS_GaussianBlur(Pixels.data(), Width, Height, 6.0f)) {
        GTEST_SKIP() << "No usable Metal device is available.";
    }

    for (uint8_t Pixel : Pixels) {
        EXPECT_EQ(Pixel, 127);
    }
}

TEST(MetalVoxelFill, FillsTheSphereCenterWhenMetalIsAvailable) {
    BG::Common::Logger::LoggingSystem Logger(false);
    BG::NES::Simulator::BoundingBox Bounds{{0.0f, 0.0f, 0.0f},
                                            {4.0f, 4.0f, 4.0f}};
    BG::NES::Simulator::VoxelArray Array(&Logger, Bounds, 1.0f);

    if (!Metal_FillSphere(&Array, 2.0f, 2.0f, 2.0f, 1.5f, 42)) {
        GTEST_SKIP() << "No usable Metal device is available.";
    }

    const auto Center = Array.GetVoxel(2, 2, 2);
    EXPECT_EQ(Center.State_, BG::NES::Simulator::VoxelState_INTERIOR);
    EXPECT_EQ(Center.ParentUID, 42u);
    EXPECT_EQ(Array.GetVoxel(0, 0, 0).State_, BG::NES::Simulator::VoxelState_EMPTY);
}

} // namespace
