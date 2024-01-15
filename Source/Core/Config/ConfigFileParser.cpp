#include <Config/ConfigFileParser.h>


namespace BG {
namespace NES {
namespace Config {


ConfigFileParser::ConfigFileParser(Config &_Config) {

    // Create Local Config Object
    Config LocalCfg;

    // Load YAML From Configuration File
    YAML::Node Config;
    bool HasConfigFile = false;

    // Enumerate Possible Locations For Configuration Files, Attempt To Load Them
    for (size_t i = 0; i < _Config.ConfigFilePaths.size(); i++) {

        // Attempt to load this given file, if that fails, catch exceptions and move on
        try {

            Config = YAML::LoadFile(_Config.ConfigFilePaths[i]);
            HasConfigFile = true;
            break;

        } catch(const YAML::BadFile& e) {
        } catch(const YAML::ParserException& e) {
            std::cerr<<"[WARN] Parser Exception Occured When Reading File At "<< _Config.ConfigFilePaths[i]<<std::endl;
        }

    }


    // If we couldn't find any config files, log an error and exit
    if (!HasConfigFile) {
        std::cerr<<"[FATAL], Failed To Open Config File, Attempted To Load Files From:\n";
        for(size_t i = 0; i < _Config.ConfigFilePaths.size(); i++){
            std::cerr << "    " << _Config.ConfigFilePaths[i] << "\n";
        }
        exit(1);
    } 


    // Populate Configuration Struct With Data From Configuration File
    _Config.PortNumber = Config["Network_NES_API_Port"].as<int>();
    _Config.Host = Config["Network_NES_API_Host"].as<std::string>();

}


ConfigFileParser::~ConfigFileParser() {
    // Destructor
    // No explicit cleanup needed as ConfigFileParser doesn't dynamically allocate memory
}


}; // Close Namespace Config
}; // Close Namespace NES
}; // Close Namespace BG