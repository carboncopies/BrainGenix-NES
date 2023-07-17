#include <Config/ArgumentParser.h>


namespace BG {
namespace NES {
namespace Config {

ArgumentParser::ArgumentParser(Config& _Config, int _NumArguments, char** _Args) {


    // Declare Command Line Options
    boost::program_options::options_description Generic("CLI Options");
    Generic.add_options()
        ("Help,h", "Show Help Message")
        ("Version,v", "Print BG-NES Server Version")
        ("CompileTimeStamp,q", "Print BG-NES Compile Date Timestamp")
        ("CompilePlatformInfo,w", "Print BG-NES Compile Platform Information")
        ("Port, p", boost::program_options::value<int>(&_Config.PortNumber)->default_value(CONFIG_DEFAULT_PORT_NUMBER), "Port Number Of The Service")
        ("Host, o", boost::program_options::value<std::string>(&_Config.Host)->default_value(CONFIG_DEFAULT_HOST), "Host That The Service Binds To")
        ("Config,c", boost::program_options::value<std::string>(&_Config.ConfigFilePaths[0])->default_value(CONFIG_DEFAULT_CFG_FILE_PATH1), "Path To Config File")
        ;
    
    boost::program_options::options_description CommandLineOptions;
    CommandLineOptions.add(Generic);

    // Now, Parse The Options
    boost::program_options::variables_map ArgMap;
    store(boost::program_options::command_line_parser(_NumArguments, _Args).options(CommandLineOptions).run(), ArgMap);
    notify(ArgMap);


    // Print Arguments
    if (ArgMap.count("Help")) {
        std::cout<<Generic<<std::endl;
        exit(0);
    }
    if (ArgMap.count("Version")) {
        std::cout<<"Version: "<<VERSION<<std::endl;
        exit(0);
    }
    if (ArgMap.count("CompileTimeStamp")) {
        std::cout<<"Compile Time Stamp: "<<COMPILE_TIME_STAMP<<std::endl;
        exit(0);
    }
    if (ArgMap.count("CompilePlatformInfo")) {
        std::string Info = "Compile Platform Information:\n    ";
        Info += std::string("Target OS: ") + TARGET_OS_NAME + "\n    ";
        Info += std::string("Target OS Version: ") + TARGET_OS_VERSION + "\n    ";
        Info += std::string("Target Arch: ") + TARGET_PROCESSOR + "\n    ";
        Info += std::string("Host OS: ") + HOST_OS_NAME + "\n    ";
        Info += std::string("Host OS Version: ") + HOST_OS_VERSION + "\n    ";
        Info += std::string("Host Arch: ") + HOST_PROCESSOR + "\n";
        std::cout<<Info;
        exit(0);
    }
}


ArgumentParser::~ArgumentParser() {

}


}; // Close Namespace Config
}; // Close Namespace NES
}; // Close Namespace BG