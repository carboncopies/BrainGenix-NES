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

#include <gtest/gtest.h>

#include <Simulator/Geometries/Box.h>


/**
 * @brief Test class for unit tests for struct Box.
 *
 */

struct BoxTest: testing::Test {
    BG::NES::Simulator::Geometries::Box* testBox = nullptr;
    float tol = 1e-3;
    float minY = 0.0, maxY = 0.0;

    void SetUp() {
        float Center_um[3] = {10.0, 10.0, 10.0};
        float Dims_um[3] = {5.5, 10.2, 6.7};
        float Rotations_rad[3] = {M_PI / 4.0, M_PI / 4.0, M_PI / 2.0};

        testBox = new BG::NES::Simulator::Geometries::Box(Center_um, Dims_um, Rotations_rad);

        minY = testBox->Center_um[1] - testBox->Dims_um[1] / 2.0;
        maxY = testBox->Center_um[1] + testBox->Dims_um[1] / 2.0;
    }

    void TearDown() {
        delete testBox;
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

    ASSERT_EQ(gotSides[0], testBox->Dims_um[0]) << "Length = " << gotSides[0];
    ASSERT_EQ(gotSides[1], testBox->Dims_um[1]) <<  "Breadth = " << gotSides[1];
    ASSERT_EQ(gotSides[2], testBox->Dims_um[2]) <<  "Height = " << gotSides[2];
}
