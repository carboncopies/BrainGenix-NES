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
// #include <boost/program_options.hpp>

// Internal Libraries (BG convention: use <> instead of "")
// #include <Config/ArgumentParser.h>
#include <Config/ConfigFileParser.h>
#include <Config/Config.h>



namespace BG {
namespace NES {
namespace Config {

/**
 * @brief Configuration Manager Class - Provides program configuration capabillities to the NES server.
 * 
 * @details This class manages the argument and config file parsers, which each generate a config struct 
 * which is given to the rest of the system. This class manages creating and owning that config struct.
 */
class Manager {

private:

    Config Config_; /**Configuration struct instance, owned by this class and given to others as needed*/

public:

    /**
     * @brief Construct a new Manager object
     * You need to pass this class the command line data so it can parse the arguments.
     * 
     * @param _NumArgs 
     * @param _Args 
     */
    Manager(int _NumArgs, char** _Args);

    /**
     * @brief Destroy the Manager object
     * 
     */
    ~Manager();

    /**
     * @brief Returns a reference to the configuration struct.
     * 
     * @return Config& 
     */
    Config& GetConfig();

};

}; // Close Namespace Config
}; // Close Namespace NES
}; // Close Namespace BG