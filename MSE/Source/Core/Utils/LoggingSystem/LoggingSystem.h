//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

/*
    Description: This file is responsible for implementing the logger system used by the rest of the system.
    Additonal Notes: None
    Date Created: 2021-05-31
*/

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <iomanip>
#include <iostream>
#include <string>
#include <ctime>
#include <cstring>
#include <map>
#include <list>
#include <array>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <yaml-cpp/yaml.h>


/**
 * @brief Main Logging System, Saves Logs To Database/Text File If Configured To.
 */
class LoggerClass {

    // Define "Local" Variables And Functions //
    private: 

        // Define Local Vars //
        YAML::Node LocalSystemConfiguration;


        bool PrintLogOutput;
        bool ColorizeLog;
        bool ReplaceLevelWithText;
        bool UseTextLogLevel_;
        int MinimumLogLevel = 5;

        int LogLevelTargetWidth = 6;
        int LogTimeTargetWidth = 19;
        const char* InsertString = "                                                         ";

        /**
         * @brief Stores the Red, Green, Blue values of the text color.
         */
        struct RGBColor {
            int Red;
            int Green;
            int Blue;
        };

        std::map<int, RGBColor> ColorLookup_; /*!< Lookup for converting log level to RGB values (stored in RGBColor struct). Populated based on values stored in Config.yaml */
        std::map<int, std::string> LogNameLookup_; /*!< Lookup for converting log level to name of log level (See Config.yaml for values). */


        /**
         * @brief Print out text to std::cout.
         * 
         * @param Message Message to be printed.
         * @param LogLevel Level of log (looked up in map to convert to RGB values).
         */
        void ColorizeText(std::string Message, int LogLevel);


    public: 


        /**
         * @brief Setup the logger class (connect to db if needed, open file for writing if enabled, etc.)
         * 
         * @param SystemConfiguration System configuration file in YAML::Node format from Config.yaml.
         */
        LoggerClass(YAML::Node SystemConfiguration);

        /**
         * @brief Cleanup the logger class, close db connection, close file io, etc.
         * 
         */
        ~LoggerClass();

        /**
         * @brief Add item to system log. 
         * 
         * @param LogItem Text to log.
         * @param LogLevel Importance of log entry (consult Config.yaml for more info).
         */
        void Log(const char* LogItem, int LogLevel=5);



};


