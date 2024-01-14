#include <Config/ConfigurationManager.h>



namespace BG {
namespace NES {
namespace Config {


Manager::Manager(int _NumArgs, char** _Args) {

    // // Parse Arguments
    // ArgumentParser ArgParser(Config_, _NumArgs, _Args);
    // ArgParser.~ArgumentParser();

    // Now Load Configuration File
    ConfigFileParser ConfigParser(Config_);
    ConfigParser.~ConfigFileParser();

}

Manager::~Manager() {

}



Config& Manager::GetConfig() {
    return Config_;
}


}; // Close Namespace Config
}; // Close Namespace NES
}; // Close Namespace BG