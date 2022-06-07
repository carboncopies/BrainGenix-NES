//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file is responsible for implementing a template neuron model for testing.
    Documentation Status: Complete
    Additonal Notes: None
    Date Created: 2021-12-03
*/ 

#pragma once

// Standard Libraries (BG convention: use <> instead of "")

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")


class EmptyNeuronModel {

private:

    // Set Neuron Metadata
    float PositionX;
    float PositionY;
    float PositionZ;


public:


    /**
     * @brief Construct a new Empty neuron Model object
     * 
     */
    EmptyNeuronModel();
    EmptyNeuronModel(float PosX, float PosY, float PosZ);


    /**
     * @brief Destroy the Empty neuron Model object
     * 
     */
    ~EmptyNeuronModel();


    /**
     * @brief Called every time the simulation advances by one timestep. Used to update the model.
     * 
     */
    void UpdateNeuron();

};