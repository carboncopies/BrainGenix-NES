//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides a class for managing configuration data from all sources.
    Additional Notes: None
    Date Created: 2023-06-23
*/

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <iostream>
#include <fstream>
#include <iterator>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <Config/ConfigFileParser.h>
#include <Config/Config.h>



namespace BG {
namespace NES {
namespace Config {

/**
 * @brief Configuration Manager Class - Provides program configuration capabilities to the NES service.
 *
 * The Manager class is responsible for managing the configuration file parser, which generates a config struct.
 * This struct is then provided to the rest of the system for configuration purposes.
 */
class Manager {

private:

    Config Config_; /**Configuration struct instance, owned by this class and given to others as needed*/

public:

    /**
     * @brief Constructor for the Manager class.
     *
     * Initializes a Manager instance, parsing command line data and loading the configuration file to configure the program.
     *
     * @param _NumArgs Number of command line arguments.
     * @param _Args Array of command line arguments.
     */
    Manager(int _NumArgs, char** _Args);

    /**
     * @brief Destructor for the Manager class.
     * 
     */
    ~Manager();

    /**
     * @brief Returns a reference to the configuration struct.
     *
     * @return Config& Reference to the configuration struct.
     */
    Config& GetConfig();

};

}; // Close Namespace Config
}; // Close Namespace NES
}; // Close Namespace BG