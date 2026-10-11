//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides unit tests for the Vec3D struct.
    Additional Notes: None
    Date Created: 2023-10-08
*/

#include <cmath>
#include <memory>

#include <cmath>
#include <random>

#include <gtest/gtest.h>

#include <Simulator/Geometries/VecTools.h>

/**
 * @brief Test class for unit tests for struct Vec3D.
 *
 */

struct Vec3DTest : testing::Test {
    std::unique_ptr<BG::NES::Simulator::Geometries::Vec3D> testVec3D = nullptr;
    std::unique_ptr<BG::NES::Simulator::Geometries::Vec3D> testVec3D_2 =
        nullptr;
    float tol = 1e-3f;

    void SetUp() {
        testVec3D = std::make_unique<BG::NES::Simulator::Geometries::Vec3D>();
        testVec3D_2 = std::make_unique<BG::NES::Simulator::Geometries::Vec3D>(
            1.2f, 3.4f, 5.6f);
    }

    void TearDown() { return; }
};

TEST_F(Vec3DTest, test_Initialization_default) {

    ASSERT_EQ(testVec3D_2->x, 1.2f);
    ASSERT_EQ(testVec3D_2->y, 3.4f);
    ASSERT_EQ(testVec3D_2->z, 5.6f);
}

TEST_F(Vec3DTest, test_Addition_default) {

    BG::NES::Simulator::Geometries::Vec3D sum = *testVec3D + *testVec3D_2;
    ASSERT_EQ(sum.x, 1.2f);
    ASSERT_EQ(sum.y, 3.4f);
    ASSERT_EQ(sum.z, 5.6f);
}

TEST_F(Vec3DTest, test_Subtraction_default) {

    BG::NES::Simulator::Geometries::Vec3D diff = *testVec3D - *testVec3D_2;
    ASSERT_EQ(diff.x, -1.2f);
    ASSERT_EQ(diff.y, -3.4f);
    ASSERT_EQ(diff.z, -5.6f);
}

TEST_F(Vec3DTest, test_VecMultiply_default) {

    BG::NES::Simulator::Geometries::Vec3D res =
        BG::NES::Simulator::Geometries::Vec3D(2.0f, 4.0f, 6.0f) *
        (*testVec3D_2);
    ASSERT_NEAR(res.x, 2.4, tol);
    ASSERT_NEAR(res.y, 13.6, tol);
    ASSERT_NEAR(res.z, 33.6, tol);
}

TEST_F(Vec3DTest, test_ScalarMultiply_default) {

    BG::NES::Simulator::Geometries::Vec3D res = *testVec3D_2 * 10.0f;
    ASSERT_NEAR(res.x, 12.0, tol);
    ASSERT_NEAR(res.y, 34.0, tol);
    ASSERT_NEAR(res.z, 56.0, tol);
}

TEST_F(Vec3DTest, test_ScalarDivide_default) {

    BG::NES::Simulator::Geometries::Vec3D res = *testVec3D_2 / 2.0f;
    ASSERT_NEAR(res.x, 0.6, tol);
    ASSERT_NEAR(res.y, 1.7, tol);
    ASSERT_NEAR(res.z, 2.8, tol);
}

TEST_F(Vec3DTest, test_Equality_default) {

    BG::NES::Simulator::Geometries::Vec3D equalVec{1.2f, 3.4f, 5.6f};

    ASSERT_EQ(equalVec == *testVec3D_2, true);
    ASSERT_EQ(equalVec == *testVec3D, false);
}

TEST_F(Vec3DTest, test_Inequality_default) {

    ASSERT_FALSE(*testVec3D_2 != *testVec3D_2);
    ASSERT_TRUE(*testVec3D != *testVec3D_2);
}

TEST_F(Vec3DTest, test_Distance_default) {

    float dist = testVec3D->Distance(*testVec3D_2);
    float expectedDist = 0.0;
    expectedDist += pow(testVec3D->x - testVec3D_2->x, 2.0) +
                    pow(testVec3D->y - testVec3D_2->y, 2.0) +
                    pow(testVec3D->z - testVec3D_2->z, 2.0);
    expectedDist = sqrt(expectedDist);

    ASSERT_NEAR(dist, expectedDist, tol);
}

TEST_F(Vec3DTest, test_Dot_default) {

    float dot = testVec3D_2->Dot(*testVec3D_2);
    float expectedDot = 0.0;
    expectedDot += pow(testVec3D_2->x, 2.0) + pow(testVec3D_2->y, 2.0) +
                   pow(testVec3D_2->z, 2.0);

    ASSERT_NEAR(dot, expectedDot, tol);
}

TEST_F(Vec3DTest, test_Cross_default) {
    BG::NES::Simulator::Geometries::Vec3D vec1{1.0f, 0.0f, 0.0f};
    BG::NES::Simulator::Geometries::Vec3D vec2{0.0f, 0.0f, 1.0f};
    BG::NES::Simulator::Geometries::Vec3D expectedCross{0.0f, -1.0f, 0.0f};
    BG::NES::Simulator::Geometries::Vec3D gotCross = vec1.Cross(vec2);

    ASSERT_EQ(expectedCross, gotCross);
}

TEST_F(Vec3DTest, test_Minimum_default) {

    float min = testVec3D_2->Min();
    ASSERT_EQ(min, 1.2f);
}

TEST_F(Vec3DTest, test_Maximum_default) {

    float max = testVec3D_2->Max();
    ASSERT_EQ(max, 5.6f);
}

// New comprehensive tests for NT-3

TEST_F(Vec3DTest, test_RotateAroundX_Pi2) {
    // (0,1,0).rotate_around_x(π/2) should give (0,0,1)
    BG::NES::Simulator::Geometries::Vec3D v{0.0f, 1.0f, 0.0f};
    BG::NES::Simulator::Geometries::Vec3D result = v.rotate_around_x(M_PI / 2.0f);
    ASSERT_NEAR(result.x, 0.0f, tol);
    ASSERT_NEAR(result.y, 0.0f, tol);
    ASSERT_NEAR(result.z, 1.0f, tol);
}

TEST_F(Vec3DTest, test_RotateAroundY_Pi2) {
    // (0,0,1).rotate_around_y(π/2) should give (1,0,0)
    BG::NES::Simulator::Geometries::Vec3D v{0.0f, 0.0f, 1.0f};
    BG::NES::Simulator::Geometries::Vec3D result = v.rotate_around_y(M_PI / 2.0f);
    ASSERT_NEAR(result.x, 1.0f, tol);
    ASSERT_NEAR(result.y, 0.0f, tol);
    ASSERT_NEAR(result.z, 0.0f, tol);
}

TEST_F(Vec3DTest, test_RotateAroundZ_Pi2) {
    // (1,0,0).rotate_around_z(π/2) should give (0,1,0)
    BG::NES::Simulator::Geometries::Vec3D v{1.0f, 0.0f, 0.0f};
    BG::NES::Simulator::Geometries::Vec3D result = v.rotate_around_z(M_PI / 2.0f);
    ASSERT_NEAR(result.x, 0.0f, tol);
    ASSERT_NEAR(result.y, 1.0f, tol);
    ASSERT_NEAR(result.z, 0.0f, tol);
}

TEST_F(Vec3DTest, test_RotateAroundXYZ_composition) {
    // rotate_around_xyz(a,b,c) should equal Rz(Ry(Rx(v)))
    float a = M_PI / 6.0f;  // 30 degrees
    float b = M_PI / 4.0f;  // 45 degrees
    float c = M_PI / 3.0f;  // 60 degrees
    BG::NES::Simulator::Geometries::Vec3D v{1.0f, 1.0f, 1.0f};

    BG::NES::Simulator::Geometries::Vec3D result1 = v.rotate_around_xyz(a, b, c);
    BG::NES::Simulator::Geometries::Vec3D result2 = v.rotate_around_x(a).rotate_around_y(b).rotate_around_z(c);

    ASSERT_NEAR(result1.x, result2.x, tol);
    ASSERT_NEAR(result1.y, result2.y, tol);
    ASSERT_NEAR(result1.z, result2.z, tol);
}

TEST_F(Vec3DTest, test_RotateAroundZYX_composition) {
    // rotate_around_zyx should equal Rx(Ry(Rz(v)))
    float a = M_PI / 6.0f;
    float b = M_PI / 4.0f;
    float c = M_PI / 3.0f;
    BG::NES::Simulator::Geometries::Vec3D v{1.0f, 1.0f, 1.0f};

    BG::NES::Simulator::Geometries::Vec3D result1 = v.rotate_around_zyx(a, b, c);
    BG::NES::Simulator::Geometries::Vec3D result2 = v.rotate_around_z(c).rotate_around_y(b).rotate_around_x(a);

    ASSERT_NEAR(result1.x, result2.x, tol);
    ASSERT_NEAR(result1.y, result2.y, tol);
    ASSERT_NEAR(result1.z, result2.z, tol);
}

TEST_F(Vec3DTest, test_MagnitudePreservedAfterRotation) {
    // Rotation should preserve magnitude
    BG::NES::Simulator::Geometries::Vec3D v{3.5f, 2.1f, 4.2f};
    float orig_mag = v.Magnitude();

    BG::NES::Simulator::Geometries::Vec3D rotated = v.rotate_around_xyz(M_PI/6, M_PI/4, M_PI/3);
    float rot_mag = rotated.Magnitude();

    ASSERT_NEAR(orig_mag, rot_mag, orig_mag * 1e-5f);  // Relative tolerance
}

TEST_F(Vec3DTest, test_CrossProductAnticommutative) {
    // a × b = −b × a
    BG::NES::Simulator::Geometries::Vec3D a{1.0f, 2.0f, 3.0f};
    BG::NES::Simulator::Geometries::Vec3D b{4.0f, 5.0f, 6.0f};

    BG::NES::Simulator::Geometries::Vec3D cross_ab = a.Cross(b);
    BG::NES::Simulator::Geometries::Vec3D cross_ba = b.Cross(a);

    ASSERT_NEAR(cross_ab.x, -cross_ba.x, tol);
    ASSERT_NEAR(cross_ab.y, -cross_ba.y, tol);
    ASSERT_NEAR(cross_ab.z, -cross_ba.z, tol);
}

TEST_F(Vec3DTest, test_DotCrossOrthogonality) {
    // a·(a×b) = 0 and b·(a×b) = 0
    BG::NES::Simulator::Geometries::Vec3D a{2.0f, 3.0f, 1.0f};
    BG::NES::Simulator::Geometries::Vec3D b{1.0f, 4.0f, 2.0f};

    BG::NES::Simulator::Geometries::Vec3D cross = a.Cross(b);

    ASSERT_NEAR(a.Dot(cross), 0.0f, tol);
    ASSERT_NEAR(b.Dot(cross), 0.0f, tol);
}

TEST_F(Vec3DTest, test_CrossProductMagnitude) {
    // |a×b|² = |a|²|b|² - (a·b)²
    BG::NES::Simulator::Geometries::Vec3D a{1.0f, 2.0f, 3.0f};
    BG::NES::Simulator::Geometries::Vec3D b{4.0f, 5.0f, 6.0f};

    BG::NES::Simulator::Geometries::Vec3D cross = a.Cross(b);
    float cross_mag_sq = cross.Magnitude() * cross.Magnitude();

    float a_mag_sq = a.Magnitude() * a.Magnitude();
    float b_mag_sq = b.Magnitude() * b.Magnitude();
    float dot_sq = a.Dot(b) * a.Dot(b);

    float expected = a_mag_sq * b_mag_sq - dot_sq;

    ASSERT_NEAR(cross_mag_sq, expected, tol * tol * 1000);  // Larger tolerance due to sq operations
}

TEST_F(Vec3DTest, test_SphericalRoundTrip) {
    // sphericalToCartesian(cartesianToSpherical(v)) ≈ v
    BG::NES::Simulator::Geometries::Vec3D v{3.0f, 4.0f, 5.0f};

    BG::NES::Simulator::Geometries::Vec3D spherical = v.cartesianToSpherical();
    BG::NES::Simulator::Geometries::Vec3D cartesian = spherical.sphericalToCartesian();

    ASSERT_NEAR(cartesian.x, v.x, tol);
    ASSERT_NEAR(cartesian.y, v.y, tol);
    ASSERT_NEAR(cartesian.z, v.z, tol);
}

TEST_F(Vec3DTest, test_SphericalSpecialCase1) {
    // (0,0,5) cartesian should map to (5,0,0) in spherical
    BG::NES::Simulator::Geometries::Vec3D v{0.0f, 0.0f, 5.0f};
    BG::NES::Simulator::Geometries::Vec3D spherical = v.cartesianToSpherical();

    ASSERT_NEAR(spherical.x, 5.0f, tol);       // r = 5
    ASSERT_NEAR(spherical.y, 0.0f, tol);       // theta = 0 (z points forward)
    ASSERT_NEAR(spherical.z, 0.0f, tol);       // phi = 0
}

TEST_F(Vec3DTest, test_SphericalSpecialCase2) {
    // (0,5,0) cartesian should map to (5, π/2, π/2) in spherical
    BG::NES::Simulator::Geometries::Vec3D v{0.0f, 5.0f, 0.0f};
    BG::NES::Simulator::Geometries::Vec3D spherical = v.cartesianToSpherical();

    ASSERT_NEAR(spherical.x, 5.0f, tol);              // r = 5
    ASSERT_NEAR(spherical.y, M_PI / 2.0f, tol);       // theta = π/2
    ASSERT_NEAR(spherical.z, M_PI / 2.0f, tol);       // phi = π/2
}

TEST_F(Vec3DTest, test_SphericalZeroVector) {
    // Zero vector should map to zero in spherical
    BG::NES::Simulator::Geometries::Vec3D v{0.0f, 0.0f, 0.0f};
    BG::NES::Simulator::Geometries::Vec3D spherical = v.cartesianToSpherical();

    ASSERT_NEAR(spherical.x, 0.0f, tol);
    ASSERT_NEAR(spherical.y, 0.0f, tol);
    ASSERT_NEAR(spherical.z, 0.0f, tol);
}

TEST_F(Vec3DTest, test_Normalize) {
    // Normalized vector should have magnitude 1
    BG::NES::Simulator::Geometries::Vec3D v{3.0f, 4.0f, 0.0f};
    BG::NES::Simulator::Geometries::Vec3D normalized = v.Normalize();

    float mag = normalized.Magnitude();
    ASSERT_NEAR(mag, 1.0f, tol);

    // Check direction is preserved
    ASSERT_NEAR(normalized.x, 0.6f, tol);
    ASSERT_NEAR(normalized.y, 0.8f, tol);
    ASSERT_NEAR(normalized.z, 0.0f, tol);
}

TEST_F(Vec3DTest, test_RotationOrderMatters) {
    // xyz and zyx orders differ for non-commuting angles.
    BG::NES::Simulator::Geometries::Vec3D v{1.0f, 1.0f, 1.0f};
    BG::NES::Simulator::Geometries::Vec3D a = v.rotate_around_xyz(M_PI / 6, M_PI / 4, M_PI / 3);
    BG::NES::Simulator::Geometries::Vec3D b = v.rotate_around_zyx(M_PI / 6, M_PI / 4, M_PI / 3);
    float diff = std::fabs(a.x - b.x) + std::fabs(a.y - b.y) + std::fabs(a.z - b.z);
    ASSERT_GT(diff, 0.1f);
}

TEST_F(Vec3DTest, test_SeededRandomInvariants) {
    // Cross/dot identities and spherical round trip over seeded random vectors.
    std::mt19937 rng(7);
    std::uniform_real_distribution<float> dist(-5.0f, 5.0f);
    for (int i = 0; i < 200; ++i) {
        BG::NES::Simulator::Geometries::Vec3D a{dist(rng), dist(rng), dist(rng)};
        BG::NES::Simulator::Geometries::Vec3D b{dist(rng), dist(rng), dist(rng)};
        BG::NES::Simulator::Geometries::Vec3D c = a.Cross(b);
        BG::NES::Simulator::Geometries::Vec3D d = b.Cross(a);
        float scale = a.Magnitude() * b.Magnitude();

        ASSERT_NEAR(a.Dot(c), 0.0f, 1e-4f * scale * a.Magnitude()) << i;
        ASSERT_NEAR(b.Dot(c), 0.0f, 1e-4f * scale * b.Magnitude()) << i;
        ASSERT_NEAR(c.x, -d.x, 1e-4f * scale) << i;
        ASSERT_NEAR(c.y, -d.y, 1e-4f * scale) << i;
        ASSERT_NEAR(c.z, -d.z, 1e-4f * scale) << i;

        float lhs = c.Dot(c);
        float rhs = a.Dot(a) * b.Dot(b) - a.Dot(b) * a.Dot(b);
        ASSERT_NEAR(lhs, rhs, 1e-4f * scale * scale) << i;

        BG::NES::Simulator::Geometries::Vec3D rt = a.cartesianToSpherical().sphericalToCartesian();
        ASSERT_NEAR(rt.x, a.x, 1e-4f) << i;
        ASSERT_NEAR(rt.y, a.y, 1e-4f) << i;
        ASSERT_NEAR(rt.z, a.z, 1e-4f) << i;
    }
}
