//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")
#include <vector>
#include <memory>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <Include/Netmorph.h>
#include <Include/Embeddable.h>

// Internal Libraries (BG convention: use <> instead of "")
#include <Netmorph/NetmorphManagerThread.h>


namespace BG {
namespace NES {
namespace Simulator {

class Nm2NESLogging: public Netmorph2NESLogging {
protected:
    BG::Common::Logger::LoggingSystem& logger;
public:
    Nm2NESLogging(BG::Common::Logger::LoggingSystem& _logger): logger(_logger) {}

    virtual void error(const std::string & msg) {
        logger.Log(msg, 7);
    }
    virtual void warning(const std::string & msg) {
        logger.Log(msg, 6);
    }
    virtual void report(const std::string & msg) {
        logger.Log(msg, 3);
    }
    virtual void progress(const std::string & msg) {
        logger.Log(msg, 5);
    }
};

int ExecuteNetmorphOperation(BG::Common::Logger::LoggingSystem* _Logger, NetmorphParameters* _Params) {

    // Create link to redirect messaging from Netmorph to the NES Logger.
    embedlog = std::make_unique<Nm2NESLogging>(*_Logger);

    _Logger->Log("Starting Netmorph Simulation", 5);
    _Params->Result = Netmorph(&_Params->Progress_percent, _Params->ModelFile);
    _Logger->Log("Netmorph Simulation Finished", 5);

    _Params->State = Netmorph_DONE;

    return 0;
}


}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG
