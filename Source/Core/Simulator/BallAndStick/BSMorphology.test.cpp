//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides unit tests for the functions in the
                 BSMorphology.h header.
    Additional Notes: None
    Date Created: 2023-10-14
*/

#include <gtest/gtest.h>

#include <Simulator/BallAndStick/BSMorphology.h>

/**
 * @brief Test class for unit tests for functions used to create morphologies
 * for ball and stick neural circuits.
 *
 */

struct BSMorphologyTest : testing::Test {
    float tol = 1e-3;

    void SetUp() {}

    void TearDown() { return; }
};
