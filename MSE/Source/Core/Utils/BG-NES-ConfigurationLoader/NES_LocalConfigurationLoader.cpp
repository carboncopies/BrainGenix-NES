//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file is responsible for implementing a function to convert a local yaml file into a YAML::Node object.
    Documentation Status: Complete
    Additonal Notes: None
    Date Created: 2021-05-31
*/ 

#include <NES_LocalConfigurationLoader.h>


// Define Configuration Loader
YAML::Node LoadConfig(const char* ConfigurationPath) {

    // Read Configuration //
    YAML::Node ConfigurationDictionary = YAML::LoadFile(ConfigurationPath);

    // Return Dictionary //
    return ConfigurationDictionary;

}