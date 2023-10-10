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

#include <gtest/gtest.h>

#include <Simulator/Geometries/VecTools.h>


/**
 * @brief Test class for unit tests for struct Vec3D.
 *
 */

struct Vec3DTest: testing::Test {
    std::unique_ptr<BG::NES::Simulator::Geometries::Vec3D> testVec3D = nullptr;
    std::unique_ptr<BG::NES::Simulator::Geometries::Vec3D> testVec3D_2 = nullptr;
    float tol = 1e-3;

    void SetUp() {
        testVec3D = std::make_unique<BG::NES::Simulator::Geometries::Vec3D>();
        testVec3D_2 = std::make_unique<BG::NES::Simulator::Geometries::Vec3D>(1.2, 3.4, 5.6);
    }   

    void TearDown() {
        return;
    }
};

TEST_F( Vec3DTest, test_Initialization_default ) {

    ASSERT_EQ(testVec3D_2->x_um, 1.2f);
    ASSERT_EQ(testVec3D_2->y_um, 3.4f);
    ASSERT_EQ(testVec3D_2->z_um, 5.6f);
}

TEST_F( Vec3DTest, test_Addition_default ) {
    
    BG::NES::Simulator::Geometries::Vec3D sum = *testVec3D + *testVec3D_2;
    ASSERT_EQ(sum.x_um, 1.2f);
    ASSERT_EQ(sum.y_um, 3.4f);
    ASSERT_EQ(sum.z_um, 5.6f);
}

TEST_F( Vec3DTest, test_Subtraction_default ) {
    
    BG::NES::Simulator::Geometries::Vec3D diff = *testVec3D - *testVec3D_2;
    ASSERT_EQ(diff.x_um, -1.2f);
    ASSERT_EQ(diff.y_um, -3.4f);
    ASSERT_EQ(diff.z_um, -5.6f);
}

TEST_F( Vec3DTest, test_ScalarMultiply_default ) {
    
    BG::NES::Simulator::Geometries::Vec3D res = *testVec3D_2 * 10.0f;
    ASSERT_NEAR(res.x_um, 12.0, tol);
    ASSERT_NEAR(res.y_um, 34.0, tol);
    ASSERT_NEAR(res.z_um, 56.0, tol);
}

TEST_F( Vec3DTest, test_ScalarDivide_default ) {
    
    BG::NES::Simulator::Geometries::Vec3D res = *testVec3D_2 / 2.0f;
    ASSERT_NEAR(res.x_um, 0.6, tol);
    ASSERT_NEAR(res.y_um, 1.7, tol);
    ASSERT_NEAR(res.z_um, 2.8, tol);
}

TEST_F( Vec3DTest, test_Distance_default ) {
    
    float dist = testVec3D->Distance(*testVec3D_2);
    float expectedDist = 0.0;
    expectedDist += pow(testVec3D->x_um - testVec3D_2->x_um, 2.0) +\
                    pow(testVec3D->y_um - testVec3D_2->y_um, 2.0) +\
                    pow(testVec3D->z_um - testVec3D_2->z_um, 2.0);
    expectedDist = sqrt(expectedDist);

    ASSERT_NEAR(dist, expectedDist, tol);
}

