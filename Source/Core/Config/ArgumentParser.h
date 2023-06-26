//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file manages parsing cli arguments.
    Additional Notes: None
    Date Created: 2023-06-23
*/

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <iostream>
#include <fstream>
#include <string>
#include <iterator>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <boost/program_options.hpp>

// Internal Libraries (BG convention: use <> instead of "")
#include <Config/Config.h>
#include <Config/ConfigDefaults.h>

#include <CompilePlatformInfo.h>
#include <CompileTimeStamp.h>
#include <Version.h>



namespace BG {
namespace NES {
namespace Config {

/**
 * @brief Argument parsing class, does what the name says. Is responsible for editing the config class as needed.
 * 
 * @details This class is given a configuration object from the configuration manager where it populates any supported arguments.
 * From there, the data is then passed back to the config manager for populating with config file data through another class.
 * Note that any arguments (if set) will override configuration file options.
 * 
 * For example, if you set the port number to 5000 in the config file, but call the program with an argument setting 
 * the port number to 6000, the program will bind to port 6000 since that's an argument.
 *  
 */
class ArgumentParser {

public:

    /**
     * @brief Construct a new Argument Parser object
     * Takes in a reference to a config struct (to be populated with config data).
     * Also takes the number of args, and the arg char array pointer themselves.
     * 
     * @param _Config 
     * @param _NumArguments 
     * @param _Args 
     */
    ArgumentParser(Config& _Config, int _NumArguments, char** _Args);

    /**
     * @brief Destroy the Argument Parser object
     * This is probably called after using the constructor.
     * We keep this as a class since it might need to stick around for the application lifetime in the future.
     * 
     */
    ~ArgumentParser();

};

}; // Close Namespace Config
}; // Close Namespace NES
}; // Close Namespace BG