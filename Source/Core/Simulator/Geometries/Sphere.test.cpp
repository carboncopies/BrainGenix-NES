//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides unit tests for the Sphere struct.
    Additional Notes: None
    Date Created: 2023-10-08
*/

#include <cmath>

#include <gtest/gtest.h>

#include <Simulator/Geometries/Sphere.h>


/**
 * @brief Test class for unit tests for struct Sphere.
 *
 */

struct SphereTest: testing::Test {
    BG::NES::Simulator::Geometries::Sphere* testSphere = nullptr;
    float tol = 1e-3;

    void SetUp() {
        float center_um[3] = {10.0, 10.0, 10.0};
        float radius_um = 2.32;
        testSphere = new BG::NES::Simulator::Geometries::Sphere(center_um, radius_um);
    }   

    void TearDown() {
        delete testSphere;
    }
};

TEST_F( SphereTest, test_Volume_um3_default ) {

    float volume_um3 = testSphere->Volume_um3();
    float expected_volume_um3 = 0.0;
    
    expected_volume_um3 = 4.0 / 3.0 * M_PI * pow(testSphere->Radius_um, 3.0);

    ASSERT_NEAR(volume_um3, expected_volume_um3, tol) << "Volume = " << volume_um3;
}

