//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides unit tests for the Box struct.
    Additional Notes: None
    Date Created: 2023-10-06
*/

#include <cmath>

#include <gtest/gtest.h>

#include <Simulator/Geometries/Geometry.h>


/**
 * @brief Test class for unit tests for struct Box.
 *
 */

struct BoxTest: testing::Test {
    Box* testBox = nullptr;

    void SetUp() {
        float Center_um = {10.0, 10.0, 10.0};
        float Dims_um = {5.5, 10.2, 6.7};
        float Rotations_rad = {M_PI / 4.0, M_PI / 4.0, M_PI / 2.0};

        testBox = new Box(Center_um, Dims_um, Rotations_rad);
    }

    void TearDown() {
        delete testBox;
    }
}

TEST_F( BoxTest, test_Volume_um3_default ) {

    float volume_um3 = testBox->Volume_um3();

    ASSERT_EQ(volume_um3, 5.5 * 10.2 * 6.7);
}

