//======================================================================//
// This file is part of the BrainGenix-NES Environment Rendering System //
//======================================================================//

/*
    Description: This is the main file of the system.
    Additional Notes: None
    Date Created: 2021-11-01
*/


// Standard Libraries (BG convention: use <> instead of "")


// Third-Party Libraries (BG convention: use <> instead of "")
#include <yaml-cpp/yaml.h>


// Internal Libraries (BG convention: use <> instead of "")
#include <BG-NES-LocalConfigurationLoader.h>
#include <BG-NES-LoggingSystem.h>




/**
 * @brief Entry point.
 * 
 * @return int 
 */
int main() {

    // Load Local System Configuration File
    YAML::Node sNESLocalSystemConfiguration = LoadConfig("Config.yaml");

    // Instantiate Logging Subsystem
    NES_CLASS_LoggerClass sNESLogger(sNESLocalSystemConfiguration);
    sNESLogger.Log("Initialized Logging System", 5);


    // Log Logo Text
    sNESLogger.Log("Starting BrainGenix-NES Instance", 2);
    sNESLogger.Log("", 5);
    sNESLogger.Log("---------------------------------------------------------------------------", 5);
    sNESLogger.Log("\x1b[38;2;0;128;55m██████╗ ██████╗  █████╗ ██╗███╗   ██╗\x1b[38;2;130;68;208m ██████╗ ███████╗███╗   ██╗██╗██╗  ██╗", 5);
    sNESLogger.Log("\x1b[38;2;0;128;55m██╔══██╗██╔══██╗██╔══██╗██║████╗  ██║\x1b[38;2;130;68;208m██╔════╝ ██╔════╝████╗  ██║██║╚██╗██╔╝", 5);
    sNESLogger.Log("\x1b[38;2;0;128;55m██████╔╝██████╔╝███████║██║██╔██╗ ██║\x1b[38;2;130;68;208m██║  ███╗█████╗  ██╔██╗ ██║██║ ╚███╔╝ ", 5);
    sNESLogger.Log("\x1b[38;2;0;128;55m██╔══██╗██╔══██╗██╔══██║██║██║╚██╗██║\x1b[38;2;130;68;208m██║   ██║██╔══╝  ██║╚██╗██║██║ ██╔██╗ ", 5);
    sNESLogger.Log("\x1b[38;2;0;128;55m██████╔╝██║  ██║██║  ██║██║██║ ╚████║\x1b[38;2;130;68;208m╚██████╔╝███████╗██║ ╚████║██║██╔╝ ██╗", 5);
    sNESLogger.Log("\x1b[38;2;0;128;55m╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═╝╚═╝╚═╝  ╚═══╝\x1b[38;2;130;68;208m ╚═════╝ ╚══════╝╚═╝  ╚═══╝╚═╝╚═╝  ╚═╝", 5);
    sNESLogger.Log("---------------------------------------------------------------------------", 5);
    sNESLogger.Log("", 5);
    sNESLogger.Log("    +-------------------------------------------------------------------------+", 4);
    sNESLogger.Log("    |  BrainGenix-NES: High-Performance Biologically-Based Neuron Simulation  |", 4);
    sNESLogger.Log("    +-------------------------------------------------------------------------+", 4);
    sNESLogger.Log("", 4);


   

    // Program Exit (Destructors Automatically Called)
    return 0;

}