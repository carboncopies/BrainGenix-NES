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

    if (_NumArgs > 1) {
        std::cout<<"Detected that you're specifying a profiling argument, NES isn't intended to take arguments normally, so if you didn't expect this, use the config file instead!\n";
        Config_.ProfilingStatus_ = (ProfilingStatus)atoi(_Args[1]);
    }

}

Manager::~Manager() {
    // Destructor
    // No explicit cleanup needed as no dynamic memory is allocated.
}



Config& Manager::GetConfig() {
    return Config_;
}


}; // Close Namespace Config
}; // Close Namespace NES
}; // Close Namespace BG