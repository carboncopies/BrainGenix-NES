//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides unit tests for the Box struct.
    Additional Notes: None
    Date Created: 2023-10-06
*/

#include <cmath>
#include <vector>
#include <memory>

#include <gtest/gtest.h>

#include <Simulator/Geometries/Box.h>


/**
 * @brief Test class for unit tests for struct Box.
 *
 */

struct BoxTest: testing::Test {
    std::unique_ptr<BG::NES::Simulator::Geometries::Box> testBox = nullptr;
    float tol = 1e-3;
    float minY = 0.0, maxY = 0.0;

    void SetUp() {
        BG::NES::Simulator::Geometries::Vec3D Center_um(10.0, 10.0, 10.0);
        BG::NES::Simulator::Geometries::Vec3D Dims_um(5.5, 10.2, 6.7);
        BG::NES::Simulator::Geometries::Vec3D Rotations_rad(M_PI / 4.0, M_PI / 4.0, M_PI / 2.0);

        testBox = std::make_unique<BG::NES::Simulator::Geometries::Box>(
                Center_um, Dims_um, Rotations_rad);

        minY = testBox->Center_um.y - testBox->Dims_um.y / 2.0;
        maxY = testBox->Center_um.y + testBox->Dims_um.y / 2.0;
    }

    void TearDown() {
        return;
    }
};

TEST_F( BoxTest, test_Volume_um3_default ) {

    float volume_um3 = testBox->Volume_um3();

    ASSERT_NEAR(volume_um3, 5.5 * 10.2 * 6.7, tol) << "Volume = " << volume_um3;
}

TEST_F( BoxTest, test_EqualSliceBounds_default ) {
    int nSlices = 10;
    std::vector<std::vector<float>> gotBounds = testBox->EqualSliceBounds(nSlices, 1);
    std::vector<float> topLeft = gotBounds[0];
    std::vector<float> bottomRight = gotBounds[1];

    ASSERT_TRUE((topLeft[1] >= minY) && (topLeft[1] <= maxY)) << "Top left y coordinate = " << topLeft[1];
    ASSERT_TRUE((bottomRight[1] >= minY) && (bottomRight[1] <= maxY)) << "Bottom right y coordinate = " << bottomRight[1];
}

TEST_F( BoxTest, test_Sides_default ) {
    std::vector<float> gotSides = testBox->Sides();

    ASSERT_NEAR(gotSides[0], testBox->Dims_um.x, tol) << "Length = " << gotSides[0];
    ASSERT_NEAR(gotSides[1], testBox->Dims_um.y, tol) <<  "Breadth = " << gotSides[1];
    ASSERT_NEAR(gotSides[2], testBox->Dims_um.z, tol) <<  "Height = " << gotSides[2];
}

// New comprehensive tests for NT-3

TEST_F( BoxTest, test_Volume_um3_case2 ) {
    // Box 5.5×10.2×6.7 gives 375.87
    BG::NES::Simulator::Geometries::Vec3D center{0.0f, 0.0f, 0.0f};
    BG::NES::Simulator::Geometries::Vec3D dims{5.5f, 10.2f, 6.7f};
    BG::NES::Simulator::Geometries::Box box(center, dims);

    float volume = box.Volume_um3();
    float expected = 5.5f * 10.2f * 6.7f;  // 375.87

    ASSERT_NEAR(volume, expected, 1e-2f);
}

TEST_F( BoxTest, test_EqualSliceBounds_values ) {
    // Box center (10,10,10), dims (5.5,10.2,6.7): y from 5.92 to 6.94, x from 7.25 to 12.75, z from 6.65 to 13.35
    BG::NES::Simulator::Geometries::Vec3D center{10.0f, 10.0f, 10.0f};
    BG::NES::Simulator::Geometries::Vec3D dims{5.5f, 10.2f, 6.7f};
    BG::NES::Simulator::Geometries::Box box(center, dims);

    std::vector<std::vector<float>> bounds = box.EqualSliceBounds(10, 1);
    std::vector<float> topLeft = bounds[0];
    std::vector<float> bottomRight = bounds[1];

    float pWidth = dims.y / 10.0f;
    float y0 = center.y - dims.y / 2.0f;

    // topLeft: (c.x - dx/2, y0 + 1*pWidth, c.z - dz/2)
    ASSERT_NEAR(topLeft[0], center.x - dims.x / 2.0f, tol);
    ASSERT_NEAR(topLeft[1], y0 + 1.0f * pWidth, tol);
    ASSERT_NEAR(topLeft[2], center.z - dims.z / 2.0f, tol);

    // bottomRight: (c.x + dx/2, y0 + 2*pWidth, c.z + dz/2)
    ASSERT_NEAR(bottomRight[0], center.x + dims.x / 2.0f, tol);
    ASSERT_NEAR(bottomRight[1], y0 + 2.0f * pWidth, tol);
    ASSERT_NEAR(bottomRight[2], center.z + dims.z / 2.0f, tol);
}

TEST_F( BoxTest, test_EqualSliceBounds_adjacent_tiling ) {
    // Adjacent slices should tile: slice k max-y equals slice k+1 min-y
    BG::NES::Simulator::Geometries::Vec3D center{10.0f, 10.0f, 10.0f};
    BG::NES::Simulator::Geometries::Vec3D dims{5.5f, 10.2f, 6.7f};
    BG::NES::Simulator::Geometries::Box box(center, dims);

    std::vector<std::vector<float>> bounds0 = box.EqualSliceBounds(10, 0);
    std::vector<std::vector<float>> bounds1 = box.EqualSliceBounds(10, 1);

    float max_y_slice0 = bounds0[1][1];
    float min_y_slice1 = bounds1[0][1];

    ASSERT_NEAR(max_y_slice0, min_y_slice1, tol);
}

TEST_F( BoxTest, test_IsPointInShape_center ) {
    // Center should be inside
    BG::NES::VSDA::WorldInfo worldInfo;
    worldInfo.WorldRotationOffsetX_rad = 0.0f;
    worldInfo.WorldRotationOffsetY_rad = 0.0f;
    worldInfo.WorldRotationOffsetZ_rad = 0.0f;

    BG::NES::Simulator::Geometries::Vec3D center{10.0f, 10.0f, 10.0f};

    bool isInside = testBox->IsPointInShape(center, worldInfo);
    ASSERT_TRUE(isInside);
}

TEST_F( BoxTest, test_IsPointInShape_corner ) {
    // Corner C ± D/2 should be inside (inclusive)
    BG::NES::VSDA::WorldInfo worldInfo;
    worldInfo.WorldRotationOffsetX_rad = 0.0f;
    worldInfo.WorldRotationOffsetY_rad = 0.0f;
    worldInfo.WorldRotationOffsetZ_rad = 0.0f;

    BG::NES::Simulator::Geometries::Vec3D corner = testBox->Center_um + testBox->Dims_um / 2.0f;

    bool isInside = testBox->IsPointInShape(corner, worldInfo);
    ASSERT_TRUE(isInside) << "Corner should be inside (inclusive)";
}

TEST_F( BoxTest, test_IsPointInShape_outside ) {
    // Point 1e-3 outside should be outside
    BG::NES::VSDA::WorldInfo worldInfo;
    worldInfo.WorldRotationOffsetX_rad = 0.0f;
    worldInfo.WorldRotationOffsetY_rad = 0.0f;
    worldInfo.WorldRotationOffsetZ_rad = 0.0f;

    BG::NES::Simulator::Geometries::Vec3D corner = testBox->Center_um + testBox->Dims_um / 2.0f + BG::NES::Simulator::Geometries::Vec3D(0.001f, 0.0f, 0.0f);

    bool isInside = testBox->IsPointInShape(corner, worldInfo);
    ASSERT_FALSE(isInside);
}

TEST_F( BoxTest, test_IsPointInShape_unrotated_box_is_axis_aligned ) {
    // With zero rotation the box is axis-aligned: (c +/- d/2) bounds apply.
    BG::NES::VSDA::WorldInfo worldInfo;
    BG::NES::Simulator::Geometries::Vec3D center{10.0f, 10.0f, 10.0f};
    BG::NES::Simulator::Geometries::Vec3D dims{10.0f, 1.0f, 1.0f};
    BG::NES::Simulator::Geometries::Box box(center, dims);

    EXPECT_TRUE(box.IsPointInShape({14.0f, 10.0f, 10.0f}, worldInfo));
    EXPECT_FALSE(box.IsPointInShape({10.0f, 14.0f, 10.0f}, worldInfo));
}

// ---------------------------------------------------------------------
// Known bug. Asserts the CORRECT behaviour, so it FAILS today. The suite
// name ends in KnownBug so ctest labels it known_bug and the gate
// (--gtest_filter=-*KnownBug*) excludes it.
// ---------------------------------------------------------------------

// Bug: dev_frame #98
// Box.cpp:89-98 IsPointInShape ignores Rotations_rad. A thin box 10x1x1
// rotated by pi/2 about z lies along the y axis (the sign convention does not
// matter at pi/2). (10,14,10) is inside it but outside the axis-aligned
// footprint; (14,10,10) is the reverse.
TEST(BoxKnownBug, test_IsPointInShape_honours_rotation) {
    BG::NES::VSDA::WorldInfo worldInfo;
    BG::NES::Simulator::Geometries::Vec3D center{10.0f, 10.0f, 10.0f};
    BG::NES::Simulator::Geometries::Vec3D dims{10.0f, 1.0f, 1.0f};
    BG::NES::Simulator::Geometries::Vec3D rotation{0.0f, 0.0f, static_cast<float>(M_PI / 2.0)};
    BG::NES::Simulator::Geometries::Box box(center, dims, rotation);

    EXPECT_TRUE(box.IsPointInShape({10.0f, 14.0f, 10.0f}, worldInfo))
        << "inside the rotated box, outside its axis-aligned footprint";
    EXPECT_FALSE(box.IsPointInShape({14.0f, 10.0f, 10.0f}, worldInfo))
        << "inside the axis-aligned footprint, outside the rotated box";
}
