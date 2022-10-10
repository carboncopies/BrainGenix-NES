//======================================================================//
// This file is part of the BrainGenix-ERS Environment Rendering System //
//======================================================================//

#include <ERS_LogSystemInfo.h>



void ERS_LogSystemInfo(ERS_LoggingSystem* Logger) {

    // Log Logo Text
    Logger->Log("Starting BrainGenix-ERS Instance", 2);
    Logger->Log("", 5);
    Logger->Log("---------------------------------------------------------------------------", 5);
    Logger->Log("\x1b[38;2;0;128;55m██████╗ ██████╗  █████╗ ██╗███╗   ██╗\x1b[38;2;130;68;208m ██████╗ ███████╗███╗   ██╗██╗██╗  ██╗", 5);
    Logger->Log("\x1b[38;2;0;128;55m██╔══██╗██╔══██╗██╔══██╗██║████╗  ██║\x1b[38;2;130;68;208m██╔════╝ ██╔════╝████╗  ██║██║╚██╗██╔╝", 5);
    Logger->Log("\x1b[38;2;0;128;55m██████╔╝██████╔╝███████║██║██╔██╗ ██║\x1b[38;2;130;68;208m██║  ███╗█████╗  ██╔██╗ ██║██║ ╚███╔╝ ", 5);
    Logger->Log("\x1b[38;2;0;128;55m██╔══██╗██╔══██╗██╔══██║██║██║╚██╗██║\x1b[38;2;130;68;208m██║   ██║██╔══╝  ██║╚██╗██║██║ ██╔██╗ ", 5);
    Logger->Log("\x1b[38;2;0;128;55m██████╔╝██║  ██║██║  ██║██║██║ ╚████║\x1b[38;2;130;68;208m╚██████╔╝███████╗██║ ╚████║██║██╔╝ ██╗", 5);
    Logger->Log("\x1b[38;2;0;128;55m╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═╝╚═╝╚═╝  ╚═══╝\x1b[38;2;130;68;208m ╚═════╝ ╚══════╝╚═╝  ╚═══╝╚═╝╚═╝  ╚═╝", 5);
    Logger->Log("---------------------------------------------------------------------------", 5);
    Logger->Log("", 5);
    Logger->Log("    +-----------------------------------------------------------------+", 4);
    Logger->Log("    |     BrainGenix-ERS Real-Time Environment Rendering System       |", 4);
    Logger->Log("    +-----------------------------------------------------------------+", 4);
    Logger->Log("", 4);

    Logger->Log(std::string("Version: ")                       + ERS_VERSION,            4);
    Logger->Log(std::string("Compile Date: ")                  + ERS_COMPILE_TIME_STAMP, 4);
    Logger->Log(std::string("Target OS: ")                     + ERS_TARGET_OS_NAME,     4);
    Logger->Log(std::string("Target OS Version: ")             + ERS_TARGET_OS_VERSION,  4);
    Logger->Log(std::string("Target Processor Architecture: ") + ERS_TARGET_PROCESSOR,   4);
    Logger->Log(std::string("Host OS: ")                       + ERS_HOST_OS_NAME,       4);
    Logger->Log(std::string("Host OS Version: ")               + ERS_HOST_OS_VERSION,    4);
    Logger->Log(std::string("Host Processor Architecture:  ")  + ERS_HOST_PROCESSOR,     4);

}