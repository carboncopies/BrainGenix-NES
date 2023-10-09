//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides unit tests for the Cylinder struct.
    Additional Notes: None
    Date Created: 2023-10-08
*/

#include <cmath>

#include <gtest/gtest.h>

#include <Simulator/Geometries/Cylinder.h>


/**
 * @brief Test class for unit tests for struct Cylinder.
 *
 */

struct CylinderTest: testing::Test {
    BG::NES::Simulator::Geometries::Cylinder* testCylinder = nullptr;
    float tol = 1e-3;

    void SetUp() {
        float End0Radius_um = 0.5;
        float End0Pos_um[3] = {0.1, 0.2, 0.3};
        float End1Radius_um = 1.2;
        float End1Pos_um[3] = {0.1, 10.2, 0.3};
        testCylinder = new BG::NES::Simulator::Geometries::Cylinder(End0Radius_um, End0Pos_um, End1Radius_um, End1Pos_um);
    }   

    void TearDown() {
        delete testCylinder;
    }
};

TEST_F( CylinderTest, test_Volume_um3_default ) {

    float volume_um3 = testCylinder->Volume_um3();
    float height_um = 0.0;
    float expected_volume_um3 = 0.0;
    
    // Height is the Euclidian distance between the two end points.
    for (size_t i=0; i<3; ++i)
        height_um += pow(testCylinder->End0Pos_um[i] - testCylinder->End1Pos_um[i], 2.0);
    height_um = sqrt(height_um);

    // Volume (conical frustum) = pi/3 * (R0^2 + R1^2 + R0*R1)
    // (Ref: https://mathworld.wolfram.com/ConicalFrustum.html)
    
    expected_volume_um3 = M_PI * (pow(testCylinder->End0Radius_um, 2.0) +
            pow(testCylinder->End1Radius_um, 2.0) +
            testCylinder->End0Radius_um * testCylinder->End1Radius_um
        ) * height_um / 3.0;

    ASSERT_NEAR(volume_um3, expected_volume_um3, tol) << "Volume = " << volume_um3;
}

TEST_F( CylinderTest, test_RAtPosition_um_default ) {
    float gotR_um = 0.0;
   
    gotR_um = testCylinder->RAtPosition_um(0.5);
    ASSERT_NEAR(gotR_um, 0.5 * (1.2 + 0.5), tol) << "R = " << gotR_um;

    gotR_um = testCylinder->RAtPosition_um(1.0);
    ASSERT_NEAR(gotR_um, 1.2, tol) << "R = " << gotR_um;

    gotR_um = testCylinder->RAtPosition_um(0.0);
    ASSERT_NEAR(gotR_um, 0.5, tol) << "R = " << gotR_um;
}

