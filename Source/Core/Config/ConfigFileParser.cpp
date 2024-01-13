#include <Config/ConfigFileParser.h>


namespace BG {
namespace NES {
namespace Config {


ConfigFileParser::ConfigFileParser(Config &_Config) {

    // Create Local Config Object
    Config LocalCfg;


    YAML::Node Config;
    bool HasConfigFile = false;
    for (unsigned int i = 0; i < _Config.ConfigFilePaths.size(); i++) {
        try {
            Config = YAML::LoadFile(_Config.ConfigFilePaths[i]);
            HasConfigFile = true;
            break;
        } catch(const YAML::BadFile& e) {
        } catch(const YAML::ParserException& e) {
        }
    }
    if (!HasConfigFile) {
        std::cerr<<"[FATAL] Cannot Open Configuration File At `"<<_Config.ConfigFilePaths[0]<<"`.\nAborting.\n";
        exit(1);
    } 

    // // Declare Config File Options (note that )
    // boost::program_options::options_description FileOptions("Config File Options");
    // FileOptions.add_options()
    //     ("Network.NES.API.Port", boost::program_options::value<int>(&LocalCfg.PortNumber)->default_value(CONFIG_DEFAULT_PORT_NUMBER), "Port Number Of The Service")
    //     ("Network.NES.API.Host", boost::program_options::value<std::string>(&LocalCfg.Host)->default_value(CONFIG_DEFAULT_HOST), "Host That The Service Binds To")
    //     ;
    
    // boost::program_options::options_description ConfigFileOptions;
    // ConfigFileOptions.add(FileOptions);

    // boost::program_options::variables_map Cfg;


    // // Open Configuration File, Parse
    // bool HasConfigFile = false;
    // for (unsigned int i = 0; i < _Config.ConfigFilePaths.size(); i++) {
    //     std::ifstream ConfigFileStream(_Config.ConfigFilePaths[i].c_str());
    //     if (ConfigFileStream) {
    //         store(parse_config_file(ConfigFileStream, ConfigFileOptions), Cfg);
    //         notify(Cfg);
    //         HasConfigFile = true;
    //         break;
    //     }
    // }
    // if (!HasConfigFile) {
    //     std::cerr<<"[FATAL] Cannot Open Configuration File At `"<<_Config.ConfigFilePaths[0]<<"`.\nAborting.\n";
    //     exit(1);
    // } 


    // // Update Parent's Config As Needed
    // if (_Config.PortNumber == CONFIG_DEFAULT_PORT_NUMBER) {
    //     _Config.PortNumber = LocalCfg.PortNumber;
    // }
    // if (_Config.Host == CONFIG_DEFAULT_HOST) {
    //     _Config.Host = LocalCfg.Host;
    // }

    _Config.PortNumber = Config["Network_NES_API_Port"].as<int>();
    _Config.Host = Config["Network_NES_API_Host"].as<std::string>();



}


ConfigFileParser::~ConfigFileParser() {

}


}; // Close Namespace Config
}; // Close Namespace NES
}; // Close Namespace BG