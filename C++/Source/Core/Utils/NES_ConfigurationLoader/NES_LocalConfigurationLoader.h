//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file is responsible for implementing a function to convert a local yaml file into a YAML::Node object.
    Documentation Status: Complete
    Additonal Notes: None
    Date Created: 2021-05-31
*/ 

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <string>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <yaml-cpp/yaml.h>

// Internal Libraries (BG convention: use <> instead of "")


/**
 * @brief Loads and parses file at given path, returns YAML::Node object.
 * 
 * @param ConfigurationPath 
 * @return YAML::Node 
 */
YAML::Node LoadConfig(const char* ConfigurationPath);