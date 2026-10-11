//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides unit tests for the Cylinder struct.
    Additional Notes: None
    Date Created: 2023-10-08
*/

#include <cmath>
#include <memory>

#include <gtest/gtest.h>

#include <Simulator/Geometries/Cylinder.h>

/**
 * @brief Test class for unit tests for struct Cylinder.
 *
 */

struct CylinderTest : testing::Test {
    std::unique_ptr<BG::NES::Simulator::Geometries::Cylinder> testCylinder =
        nullptr;
    float tol = 1e-3;

    void SetUp() {
        float End0Radius_um = 0.5;
        BG::NES::Simulator::Geometries::Vec3D End0Pos_um{0.1, 0.2, 0.3};
        float End1Radius_um = 1.2;
        BG::NES::Simulator::Geometries::Vec3D End1Pos_um{0.1, 10.2, 0.3};

        testCylinder =
            std::make_unique<BG::NES::Simulator::Geometries::Cylinder>(
                End0Radius_um, End0Pos_um, End1Radius_um, End1Pos_um);
    }

    void TearDown() { return; }
};

TEST_F(CylinderTest, test_Volume_um3_default) {

    float volume_um3 = testCylinder->Volume_um3();
    float height_um = 0.0;
    float expected_volume_um3 = 0.0;

    // Height is the Euclidian distance between the two end points.
    height_um = testCylinder->End0Pos_um.Distance(testCylinder->End1Pos_um);

    // Volume (conical frustum) = pi/3 * (R0^2 + R1^2 + R0*R1)
    // (Ref: https://mathworld.wolfram.com/ConicalFrustum.html)

    expected_volume_um3 =
        M_PI *
        (pow(testCylinder->End0Radius_um, 2.0) +
         pow(testCylinder->End1Radius_um, 2.0) +
         testCylinder->End0Radius_um * testCylinder->End1Radius_um) *
        height_um / 3.0;

    ASSERT_NEAR(volume_um3, expected_volume_um3, tol)
        << "Volume = " << volume_um3;
}

TEST_F(CylinderTest, test_RAtPosition_um_default) {
    float gotR_um = 0.0;

    gotR_um = testCylinder->RAtPosition_um(0.5);
    ASSERT_NEAR(gotR_um, 0.5 * (1.2 + 0.5), tol) << "R = " << gotR_um;

    gotR_um = testCylinder->RAtPosition_um(1.0);
    ASSERT_NEAR(gotR_um, 1.2, tol) << "R = " << gotR_um;

    gotR_um = testCylinder->RAtPosition_um(0.0);
    ASSERT_NEAR(gotR_um, 0.5, tol) << "R = " << gotR_um;
}

TEST_F(CylinderTest, test_GetRotation_rad_default) {
    // GetRotation_rad invariant: ẑ.rotate_around_xyz(GetRotation_rad()) ≈ axis direction
    BG::NES::Simulator::Geometries::Vec3D zAxis{0.0f, 0.0f, 1.0f};
    BG::NES::Simulator::Geometries::Vec3D axisVec = testCylinder->End1Pos_um - testCylinder->End0Pos_um;

    BG::NES::Simulator::Geometries::Vec3D rotation = testCylinder->GetRotation_rad();
    BG::NES::Simulator::Geometries::Vec3D rotatedZ = zAxis.rotate_around_xyz(rotation.x, rotation.y, rotation.z);

    // Normalize both for comparison
    BG::NES::Simulator::Geometries::Vec3D normalizedRotatedZ = rotatedZ.Normalize();
    BG::NES::Simulator::Geometries::Vec3D normalizedAxis = axisVec.Normalize();

    ASSERT_NEAR(normalizedRotatedZ.x, normalizedAxis.x, tol);
    ASSERT_NEAR(normalizedRotatedZ.y, normalizedAxis.y, tol);
    ASSERT_NEAR(normalizedRotatedZ.z, normalizedAxis.z, tol);
}

// New comprehensive tests for NT-3

TEST_F(CylinderTest, test_Volume_um3_case3) {
    // Frustum R0=0.5, R1=1.2, h=10 gives π·22.9/3 = 23.981
    float R0 = 0.5f;
    float R1 = 1.2f;
    BG::NES::Simulator::Geometries::Vec3D end0{0.0f, 0.0f, 0.0f};
    BG::NES::Simulator::Geometries::Vec3D end1{0.0f, 0.0f, 10.0f};
    BG::NES::Simulator::Geometries::Cylinder cyl(R0, end0, R1, end1);

    float volume = cyl.Volume_um3();
    float expected = M_PI * 22.9f / 3.0f;  // π·(R0²+R1²+R0R1)·h/3 = π·(0.25+1.44+0.6)·10/3 ≈ 23.981

    ASSERT_NEAR(volume, expected, 1e-3f);
}

TEST_F(CylinderTest, test_Volume_um3_cylinder_equal_radii) {
    // When R0==R1==r, frustum formula equals πr²h
    float r = 1.5f;
    BG::NES::Simulator::Geometries::Vec3D end0{0.0f, 0.0f, 0.0f};
    BG::NES::Simulator::Geometries::Vec3D end1{0.0f, 0.0f, 5.0f};
    BG::NES::Simulator::Geometries::Cylinder cyl(r, end0, r, end1);

    float volume = cyl.Volume_um3();
    float expected = M_PI * r * r * 5.0f;

    ASSERT_NEAR(volume, expected, tol);
}

TEST_F(CylinderTest, test_RAtPosition_um_case4) {
    // pos ≤ 0 returns R0, pos ≥ 1 returns R1, pos 0.5 returns (R0+R1)/2
    ASSERT_NEAR(testCylinder->RAtPosition_um(-0.5f), testCylinder->End0Radius_um, tol);
    ASSERT_NEAR(testCylinder->RAtPosition_um(0.0f), testCylinder->End0Radius_um, tol);
    ASSERT_NEAR(testCylinder->RAtPosition_um(0.5f),
                (testCylinder->End0Radius_um + testCylinder->End1Radius_um) / 2.0f, tol);
    ASSERT_NEAR(testCylinder->RAtPosition_um(1.0f), testCylinder->End1Radius_um, tol);
    ASSERT_NEAR(testCylinder->RAtPosition_um(1.5f), testCylinder->End1Radius_um, tol);
}

TEST_F(CylinderTest, test_GetRotation_rad_axis_y) {
    // Test with axis pointing along +y
    BG::NES::Simulator::Geometries::Vec3D end0{0.0f, 0.0f, 0.0f};
    BG::NES::Simulator::Geometries::Vec3D end1{0.0f, 1.0f, 0.0f};
    BG::NES::Simulator::Geometries::Cylinder cyl(1.0f, end0, 1.0f, end1);

    BG::NES::Simulator::Geometries::Vec3D zAxis{0.0f, 0.0f, 1.0f};
    BG::NES::Simulator::Geometries::Vec3D yAxis{0.0f, 1.0f, 0.0f};

    BG::NES::Simulator::Geometries::Vec3D rotation = cyl.GetRotation_rad();
    BG::NES::Simulator::Geometries::Vec3D rotatedZ = zAxis.rotate_around_xyz(rotation.x, rotation.y, rotation.z);

    BG::NES::Simulator::Geometries::Vec3D normalizedRotatedZ = rotatedZ.Normalize();
    BG::NES::Simulator::Geometries::Vec3D normalizedAxis = yAxis.Normalize();

    ASSERT_NEAR(normalizedRotatedZ.x, normalizedAxis.x, tol);
    ASSERT_NEAR(normalizedRotatedZ.y, normalizedAxis.y, tol);
    ASSERT_NEAR(normalizedRotatedZ.z, normalizedAxis.z, tol);
}

TEST_F(CylinderTest, test_GetRotation_rad_axis_x) {
    // Test with axis pointing along +x
    BG::NES::Simulator::Geometries::Vec3D end0{0.0f, 0.0f, 0.0f};
    BG::NES::Simulator::Geometries::Vec3D end1{1.0f, 0.0f, 0.0f};
    BG::NES::Simulator::Geometries::Cylinder cyl(1.0f, end0, 1.0f, end1);

    BG::NES::Simulator::Geometries::Vec3D zAxis{0.0f, 0.0f, 1.0f};
    BG::NES::Simulator::Geometries::Vec3D xAxis{1.0f, 0.0f, 0.0f};

    BG::NES::Simulator::Geometries::Vec3D rotation = cyl.GetRotation_rad();
    BG::NES::Simulator::Geometries::Vec3D rotatedZ = zAxis.rotate_around_xyz(rotation.x, rotation.y, rotation.z);

    BG::NES::Simulator::Geometries::Vec3D normalizedRotatedZ = rotatedZ.Normalize();
    BG::NES::Simulator::Geometries::Vec3D normalizedAxis = xAxis.Normalize();

    ASSERT_NEAR(normalizedRotatedZ.x, normalizedAxis.x, tol);
    ASSERT_NEAR(normalizedRotatedZ.y, normalizedAxis.y, tol);
    ASSERT_NEAR(normalizedRotatedZ.z, normalizedAxis.z, tol);
}

TEST_F(CylinderTest, test_GetRotation_rad_diagonal_axis) {
    // Test with diagonal axis (1,2,3)
    BG::NES::Simulator::Geometries::Vec3D end0{0.0f, 0.0f, 0.0f};
    BG::NES::Simulator::Geometries::Vec3D end1{1.0f, 2.0f, 3.0f};
    BG::NES::Simulator::Geometries::Cylinder cyl(1.0f, end0, 1.0f, end1);

    BG::NES::Simulator::Geometries::Vec3D zAxis{0.0f, 0.0f, 1.0f};
    BG::NES::Simulator::Geometries::Vec3D axis{1.0f, 2.0f, 3.0f};

    BG::NES::Simulator::Geometries::Vec3D rotation = cyl.GetRotation_rad();
    BG::NES::Simulator::Geometries::Vec3D rotatedZ = zAxis.rotate_around_xyz(rotation.x, rotation.y, rotation.z);

    BG::NES::Simulator::Geometries::Vec3D normalizedRotatedZ = rotatedZ.Normalize();
    BG::NES::Simulator::Geometries::Vec3D normalizedAxis = axis.Normalize();

    ASSERT_NEAR(normalizedRotatedZ.x, normalizedAxis.x, tol);
    ASSERT_NEAR(normalizedRotatedZ.y, normalizedAxis.y, tol);
    ASSERT_NEAR(normalizedRotatedZ.z, normalizedAxis.z, tol);
}

TEST_F(CylinderTest, test_GetRotation_rad_axis_negative_mixed) {
    // Axis (-1, 0.5, 0.2): rotated z-hat must point along the axis.
    BG::NES::Simulator::Geometries::Vec3D end0{0.0f, 0.0f, 0.0f};
    BG::NES::Simulator::Geometries::Vec3D end1{-1.0f, 0.5f, 0.2f};
    BG::NES::Simulator::Geometries::Cylinder cyl(1.0f, end0, 1.0f, end1);

    BG::NES::Simulator::Geometries::Vec3D zAxis{0.0f, 0.0f, 1.0f};
    BG::NES::Simulator::Geometries::Vec3D rotation = cyl.GetRotation_rad();
    BG::NES::Simulator::Geometries::Vec3D rotatedZ =
        zAxis.rotate_around_xyz(rotation.x, rotation.y, rotation.z);
    BG::NES::Simulator::Geometries::Vec3D expected = end1.Normalize();

    ASSERT_NEAR(rotatedZ.x, expected.x, 1e-4f);
    ASSERT_NEAR(rotatedZ.y, expected.y, 1e-4f);
    ASSERT_NEAR(rotatedZ.z, expected.z, 1e-4f);
}

TEST_F(CylinderTest, test_IsPointInShape_point_far_outside) {
    // A point far from the cylinder must not be inside (holds today only
    // because the function always returns false; see CylinderKnownBug).
    BG::NES::VSDA::WorldInfo worldInfo;
    BG::NES::Simulator::Geometries::Vec3D far{100.0f, 100.0f, 100.0f};
    ASSERT_FALSE(testCylinder->IsPointInShape(far, worldInfo));
}

TEST_F(CylinderTest, test_IsInsideRegion_overlapping_and_disjoint) {
    // Cylinder (0,0,0)->(0,0,10), both radii 1: bounding extent is
    // x,y in [-1,1], z in [-1,11] (the code pads z by the radius).
    BG::NES::VSDA::WorldInfo worldInfo;
    BG::NES::Simulator::Geometries::Vec3D end0{0.0f, 0.0f, 0.0f};
    BG::NES::Simulator::Geometries::Vec3D end1{0.0f, 0.0f, 10.0f};
    BG::NES::Simulator::Geometries::Cylinder cyl(1.0f, end0, 1.0f, end1);

    BG::NES::Simulator::BoundingBox overlapping;
    overlapping.bb_point1[0] = -0.5f; overlapping.bb_point1[1] = -0.5f; overlapping.bb_point1[2] = 4.0f;
    overlapping.bb_point2[0] = 0.5f;  overlapping.bb_point2[1] = 0.5f;  overlapping.bb_point2[2] = 6.0f;
    ASSERT_TRUE(cyl.IsInsideRegion(overlapping, worldInfo));

    BG::NES::Simulator::BoundingBox disjoint;
    disjoint.bb_point1[0] = 5.0f; disjoint.bb_point1[1] = 5.0f; disjoint.bb_point1[2] = 4.0f;
    disjoint.bb_point2[0] = 6.0f; disjoint.bb_point2[1] = 6.0f; disjoint.bb_point2[2] = 6.0f;
    ASSERT_FALSE(cyl.IsInsideRegion(disjoint, worldInfo));
}

// ---------------------------------------------------------------------
// Known bugs. Each asserts the CORRECT behaviour, so it FAILS today. The
// suite name ends in KnownBug so ctest labels it known_bug and the gate
// (--gtest_filter=-*KnownBug*) excludes it. Fix the bug, then rename the
// suite to join the gate.
// ---------------------------------------------------------------------

// Bug: dev_frame #97
// Cylinder.cpp:326 CylinderBase::IsPointInShape is a stub returning false.
TEST(CylinderKnownBug, test_IsPointInShape_axis_midpoint_is_inside) {
    BG::NES::VSDA::WorldInfo worldInfo;
    BG::NES::Simulator::Geometries::Vec3D end0{0.0f, 0.0f, 0.0f};
    BG::NES::Simulator::Geometries::Vec3D end1{0.0f, 0.0f, 10.0f};
    BG::NES::Simulator::Geometries::Cylinder cyl(1.0f, end0, 1.0f, end1);

    BG::NES::Simulator::Geometries::Vec3D mid{0.0f, 0.0f, 5.0f};
    EXPECT_TRUE(cyl.IsPointInShape(mid, worldInfo))
        << "a point on the axis halfway between the ends is inside";
}

// Bug: dev_frame #99
// Cylinder.cpp:343 maxZ uses End1Radius_um for End0's z extent.
// End0 (z=10) has radius 5, End1 (z=0) has radius 1, so the correct maximum
// z of the extent is 15 but the code gives 11. A region at z in [12,14]
// overlapping in x,y must therefore intersect.
TEST(CylinderKnownBug, test_IsInsideRegion_maxZ_uses_End0Radius) {
    BG::NES::VSDA::WorldInfo worldInfo;
    BG::NES::Simulator::Geometries::Vec3D end0{0.0f, 0.0f, 10.0f};
    BG::NES::Simulator::Geometries::Vec3D end1{0.0f, 0.0f, 0.0f};
    BG::NES::Simulator::Geometries::Cylinder cyl(5.0f, end0, 1.0f, end1);

    BG::NES::Simulator::BoundingBox region;
    region.bb_point1[0] = -1.0f; region.bb_point1[1] = -1.0f; region.bb_point1[2] = 12.0f;
    region.bb_point2[0] = 1.0f;  region.bb_point2[1] = 1.0f;  region.bb_point2[2] = 14.0f;
    EXPECT_TRUE(cyl.IsInsideRegion(region, worldInfo));
}

// Bug: dev_frame #99
// Mirror case: End0 (z=10) radius 1, End1 (z=0) radius 5. Correct maxZ is 11
// (min z is -5), but the code gives 15, so a region at z in [12,14] is wrongly
// reported as intersecting.
TEST(CylinderKnownBug, test_IsInsideRegion_maxZ_not_overestimated) {
    BG::NES::VSDA::WorldInfo worldInfo;
    BG::NES::Simulator::Geometries::Vec3D end0{0.0f, 0.0f, 10.0f};
    BG::NES::Simulator::Geometries::Vec3D end1{0.0f, 0.0f, 0.0f};
    BG::NES::Simulator::Geometries::Cylinder cyl(1.0f, end0, 5.0f, end1);

    BG::NES::Simulator::BoundingBox region;
    region.bb_point1[0] = -1.0f; region.bb_point1[1] = -1.0f; region.bb_point1[2] = 12.0f;
    region.bb_point2[0] = 1.0f;  region.bb_point2[1] = 1.0f;  region.bb_point2[2] = 14.0f;
    EXPECT_FALSE(cyl.IsInsideRegion(region, worldInfo));
}

// Bug: dev_frame #100
// Cylinder.cpp:81,84-85 acos is unclamped and the zero cross product of an
// axis parallel to +z is normalised, so GetRotation_rad returns NaN. The
// correct rotation is zero (the axis already is +z).
TEST(CylinderKnownBug, test_GetRotation_rad_axis_parallel_z_is_finite_zero) {
    BG::NES::Simulator::Geometries::Vec3D end0{0.0f, 0.0f, 0.0f};
    BG::NES::Simulator::Geometries::Vec3D end1{0.0f, 0.0f, 3.0f};
    BG::NES::Simulator::Geometries::Cylinder cyl(1.0f, end0, 1.0f, end1);

    BG::NES::Simulator::Geometries::Vec3D rotation = cyl.GetRotation_rad();
    EXPECT_TRUE(std::isfinite(rotation.x));
    EXPECT_TRUE(std::isfinite(rotation.y));
    EXPECT_TRUE(std::isfinite(rotation.z));
    EXPECT_NEAR(rotation.x, 0.0f, 1e-4f);
    EXPECT_NEAR(rotation.y, 0.0f, 1e-4f);
    EXPECT_NEAR(rotation.z, 0.0f, 1e-4f);
}
