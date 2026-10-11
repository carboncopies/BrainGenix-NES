//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides unit tests for the Sphere struct.
    Additional Notes: None
    Date Created: 2023-10-08
*/

#include <cmath>
#include <memory>

#include <gtest/gtest.h>

#include <Simulator/Geometries/Sphere.h>


/**
 * @brief Test class for unit tests for struct Sphere.
 *
 */

struct SphereTest: testing::Test {
    std::unique_ptr<BG::NES::Simulator::Geometries::Sphere> testSphere = nullptr;
    float tol = 1e-3;

    void SetUp() {
        BG::NES::Simulator::Geometries::Vec3D center_um{10.0, 10.0, 10.0};
        float radius_um = 2.32;
        testSphere = std::make_unique<BG::NES::Simulator::Geometries::Sphere>(
                center_um, radius_um);
    }   

    void TearDown() {
        return;
    }
};

TEST_F( SphereTest, test_Volume_um3_default ) {

    float volume_um3 = testSphere->Volume_um3();
    float expected_volume_um3 = 0.0;

    expected_volume_um3 = 4.0 / 3.0 * M_PI * pow(testSphere->Radius_um, 3.0);

    ASSERT_NEAR(volume_um3, expected_volume_um3, tol) << "Volume = " << volume_um3;
}

// New comprehensive tests for NT-3

TEST_F( SphereTest, test_Volume_um3_case1 ) {
    // Sphere r=1 gives V = 4.18879
    BG::NES::Simulator::Geometries::Vec3D center{0.0f, 0.0f, 0.0f};
    BG::NES::Simulator::Geometries::Sphere sphere(center, 1.0f);

    float volume = sphere.Volume_um3();
    float expected = 4.0f / 3.0f * M_PI;  // ≈ 4.18879

    ASSERT_NEAR(volume, expected, 1e-4f);
}

TEST_F( SphereTest, test_IsPointInShape_center ) {
    // Center should be inside
    BG::NES::VSDA::WorldInfo worldInfo;
    worldInfo.WorldRotationOffsetX_rad = 0.0f;
    worldInfo.WorldRotationOffsetY_rad = 0.0f;
    worldInfo.WorldRotationOffsetZ_rad = 0.0f;

    BG::NES::Simulator::Geometries::Vec3D center{10.0f, 10.0f, 10.0f};
    BG::NES::Simulator::Geometries::Vec3D point = center;

    bool isInside = testSphere->IsPointInShape(point, worldInfo);
    ASSERT_TRUE(isInside);
}

TEST_F( SphereTest, test_IsPointInShape_surface ) {
    // Point on surface at distance exactly r should be inside (inclusive)
    BG::NES::VSDA::WorldInfo worldInfo;
    worldInfo.WorldRotationOffsetX_rad = 0.0f;
    worldInfo.WorldRotationOffsetY_rad = 0.0f;
    worldInfo.WorldRotationOffsetZ_rad = 0.0f;

    BG::NES::Simulator::Geometries::Vec3D center{10.0f, 10.0f, 10.0f};
    float radius = testSphere->Radius_um;
    BG::NES::Simulator::Geometries::Vec3D point{10.0f + radius, 10.0f, 10.0f};

    bool isInside = testSphere->IsPointInShape(point, worldInfo);
    ASSERT_TRUE(isInside) << "Surface point should be inside (inclusive)";
}

TEST_F( SphereTest, test_IsPointInShape_outside ) {
    // Point just outside should be outside
    BG::NES::VSDA::WorldInfo worldInfo;
    worldInfo.WorldRotationOffsetX_rad = 0.0f;
    worldInfo.WorldRotationOffsetY_rad = 0.0f;
    worldInfo.WorldRotationOffsetZ_rad = 0.0f;

    BG::NES::Simulator::Geometries::Vec3D center{10.0f, 10.0f, 10.0f};
    float radius = testSphere->Radius_um;
    BG::NES::Simulator::Geometries::Vec3D point{10.0f + radius + 0.001f, 10.0f, 10.0f};

    bool isInside = testSphere->IsPointInShape(point, worldInfo);
    ASSERT_FALSE(isInside);
}

TEST_F( SphereTest, test_GetBoundingBox ) {
    // GetBoundingBox should equal centre ± r
    BG::NES::VSDA::WorldInfo worldInfo;
    worldInfo.WorldRotationOffsetX_rad = 0.0f;
    worldInfo.WorldRotationOffsetY_rad = 0.0f;
    worldInfo.WorldRotationOffsetZ_rad = 0.0f;

    BG::NES::Simulator::Geometries::Vec3D center = testSphere->Center_um;
    float radius = testSphere->Radius_um;

    BG::NES::Simulator::BoundingBox bb = testSphere->GetBoundingBox(worldInfo);

    ASSERT_NEAR(bb.bb_point1[0], center.x - radius, tol);
    ASSERT_NEAR(bb.bb_point1[1], center.y - radius, tol);
    ASSERT_NEAR(bb.bb_point1[2], center.z - radius, tol);

    ASSERT_NEAR(bb.bb_point2[0], center.x + radius, tol);
    ASSERT_NEAR(bb.bb_point2[1], center.y + radius, tol);
    ASSERT_NEAR(bb.bb_point2[2], center.z + radius, tol);
}

TEST_F( SphereTest, test_Volume_um3_r2p32 ) {
    // Fixture sphere r=2.32: V = 4/3 pi r^3 = 52.307
    ASSERT_NEAR(testSphere->Volume_um3(), 52.307f, 1e-2f);
}

TEST_F( SphereTest, test_IsPointInShape_r2_boundary ) {
    // Centre (10,10,10), r=2: (12,10,10) in, (12.001,10,10) out.
    BG::NES::VSDA::WorldInfo worldInfo;
    BG::NES::Simulator::Geometries::Sphere sphere({10.0f, 10.0f, 10.0f}, 2.0f);
    EXPECT_TRUE(sphere.IsPointInShape({12.0f, 10.0f, 10.0f}, worldInfo));
    EXPECT_FALSE(sphere.IsPointInShape({12.001f, 10.0f, 10.0f}, worldInfo));
}

TEST_F( SphereTest, test_IsInsideRegion_touching_inclusive_and_disjoint ) {
    // Sphere (10,10,10) r=2 spans [8,12]^3. A region starting at x=12
    // touches it (inclusive); one starting at x=12.5 does not.
    BG::NES::VSDA::WorldInfo worldInfo;
    BG::NES::Simulator::Geometries::Sphere sphere({10.0f, 10.0f, 10.0f}, 2.0f);

    BG::NES::Simulator::BoundingBox touching;
    touching.bb_point1[0] = 12.0f; touching.bb_point1[1] = 8.0f; touching.bb_point1[2] = 8.0f;
    touching.bb_point2[0] = 14.0f; touching.bb_point2[1] = 12.0f; touching.bb_point2[2] = 12.0f;
    EXPECT_TRUE(sphere.IsInsideRegion(touching, worldInfo));

    BG::NES::Simulator::BoundingBox apart = touching;
    apart.bb_point1[0] = 12.5f;
    EXPECT_FALSE(sphere.IsInsideRegion(apart, worldInfo));
}
