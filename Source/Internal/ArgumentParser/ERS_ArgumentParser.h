//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <thread>
#include <chrono>
#include <iostream>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <ERS_LoggingSystem.h>


namespace BrainGenix {
namespace ERS {
namespace Module {


/**
 * @brief Create Hardware Information Class
 * 
 */
class ArgumentParser {

    private:

        std::vector<std::pair<std::string, std::string>> ArgumentPairs_; /**<List of argument values and their paires, etc.*/
        std::vector<std::string> Arguments_; /**<Just a list of all strings In Command*/
        std::string ArgumentString_; /**<Argument String including All Arguments*/

        ERS_LoggingSystem* Logger_; /**<Pointer To Logger, Allowing The System To List Argument Value Stuff*/

    public:

        /**
         * @brief Construct a new Hardware Information object
         * 
         * @param Logger
         */
        ArgumentParser(ERS_LoggingSystem* Logger);

        /**
         * @brief Destroy the Hardware Information object
         * 
         */
        ~ArgumentParser();


        /**
         * @brief Parses the arguments, populating the argument pairs and other internal vars.
         * Returns true on success, false on failure.
         * Will Clear Internal State Of The Arg Parser, So Only Call Once.
         * 
         * @param NumberArguments 
         * @param ArgumentValues 
         * @return true 
         * @return false 
         */
        bool ParseArguments(int NumberArguments, char** ArgumentValues);



        /**
         * @brief Returns the entire string that was used to launch ERS.
         * 
         * @return std::string 
         */
        std::string GetArgumentString();


        /**
         * @brief Returns a list of each argument with it's key being the first value in the pair and the value the second.
         * 
         * @return std::vector<std::pair<std::string, std::string>> 
         */
        std::vector<std::pair<std::string, std::string>> GetArgumentPairs();



};



}
}
}
