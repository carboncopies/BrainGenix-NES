//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file is responsible for implementing a template model for testing.
    Documentation Status: Complete
    Additonal Notes: None
    Date Created: 2021-12-03
*/ 

#pragma once

// Standard Libraries (BG convention: use <> instead of "")

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")


class EmptyModel {

    private:

        // Set Neuron Metadata
        float PositionX;
        float PositionY;
        float PositionZ;


    public:

        /**
         * @brief Construct a new Empty Model object
         * 
         */
        EmptyModel();
        EmptyModel(float PosX, float PosY, float PosZ);

        /**
         * @brief Destroy the Empty Model object
         * 
         */
        ~EmptyModel();



};