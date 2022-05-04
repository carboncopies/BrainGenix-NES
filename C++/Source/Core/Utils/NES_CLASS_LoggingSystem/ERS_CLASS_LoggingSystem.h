//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

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
#include <memory>
#include <mutex>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <yaml-cpp/yaml.h>

// Interna; Libraries (BG convention: use <> instead of "")
#include <ERS_STRUCT_RGBColor.h>


/**
 * @brief Main Logging System, Saves Logs To Database/Text File If Configured To.
 */
class ERS_CLASS_LoggingSystem {

private: 

    YAML::Node LocalSystemConfiguration;


    bool PrintLogOutput;
    bool ColorizeLog;
    bool ReplaceLevelWithText;
    bool UseTextLogLevel_;
    int MinimumLogLevel = 5;

    int LogLevelTargetWidth = 6;
    int LogTimeTargetWidth = 19;
    const char* InsertString = "                                                         ";
    
    std::mutex LogMutex_; /**<Used to ensure thread safety during vector operations*/


    std::map<int, ERS_STRUCT_RGBColor> ColorLookup_; /*!< Lookup for converting log level to RGB values (stored in RGBColor struct). Populated based on values stored in Config.yaml */
    std::map<int, std::string> LogNameLookup_; /*!< Lookup for converting log level to name of log level (See Config.yaml for values). */

private:

    /**
     * @brief Print out text to std::cout.
     * 
     * @param Message Message to be printed.
     * @param LogLevel Level of log (looked up in map to convert to RGB values).
     */
    void ColorizeText(std::string Message, int LogLevel);


public:

    std::vector<std::string> LogMessages_; /**<List Of Messages*/
    std::vector<std::string> FullLogMessages_; /**<Full Messages*/
    std::vector<int> LogLevels_; /**<List of Log Levels*/
    std::vector<std::string> LogTimes_; /**<List Of Times*/
    std::vector<ERS_STRUCT_RGBColor> LogColors_; /**<List of Colors For Messages*/


public: 


    /**
     * @brief Setup the logger class (connect to db if needed, open file for writing if enabled, etc.)
     * 
     * @param SystemConfiguration System configuration file in YAML::Node format from Config.yaml.
     */
    ERS_CLASS_LoggingSystem(YAML::Node SystemConfiguration);

    /**
     * @brief Cleanup the logger class, close db connection, close file io, etc.
     * 
     */
    ~ERS_CLASS_LoggingSystem();

    /**
     * @brief Add item to system log. 
     * 
     * @param LogItem Text to log.
     * @param LogLevel Importance of log entry (consult Config.yaml for more info).
     */
    void Log(const char* LogItem, int LogLevel=5);

    /**
     * @brief Log that takes std::string instead of const char ptr
     * 
     */
    void Log(std::string LogMessage, int LogLevel=5);

};


