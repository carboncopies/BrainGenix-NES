//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file is responsible for implementing a template neuron model for testing.
    Documentation Status: Complete
    Additonal Notes: None
    Date Created: 2021-12-03
*/ 

#include <NES_EmptyNeuronModel.h>


// Constructor
EmptyNeuronModel::EmptyNeuronModel() {

    // Set Default Pos XYZ
    PositionX = 0.0f;
    PositionY = 0.0f;
    PositionZ = 0.0f;

}

EmptyNeuronModel::EmptyNeuronModel(float PosX, float PosY, float PosZ) {

    // Set Pos XYZ
    PositionX = PosX;
    PositionY = PosY;
    PositionZ = PosZ;

}

// Destructor
EmptyNeuronModel::~EmptyNeuronModel() {

    // Nothing To Do For Now

}


// Neuron Update Function
void EmptyNeuronModel::UpdateNeuron() {

    // Empty Test Model, So Nothing To Do Here Either

}