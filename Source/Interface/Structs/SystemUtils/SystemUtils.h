//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <memory.h>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <yaml-cpp/yaml.h>

// Internal Libraries (BG convention: use <> instead of "")
#include <LoggingSystem.h>
#include <HardwareInformation.h>



/**
 * @brief The System utils struct contains most system utils prepopulated as shared pointers within.
 * 
 */
struct SystemUtils {


    std::vector<std::pair<std::string, std::string>> Arguments_; /**<Pair of key and value arguments used to launch the program*/
    std::string ArgumentString_; /**<String version of the argument pair*/

    std::unique_ptr<YAML::Node> LocalSystemConfiguration_; /**<Pointer To Config.yaml Data From Local Hard Drive*/
    std::unique_ptr<bool> SystemShouldRun_; /**<Pointer To Variable Setting If System Should Run Or Not*/

    std::unique_ptr<LoggingSystem> Logger_; /**<Pointer To BG-ERS Logging System*/


};

