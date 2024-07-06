//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")
#include <vector>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <Include/Netmorph.h>

// Internal Libraries (BG convention: use <> instead of "")
#include <Netmorph/NetmorphManagerThread.h>





namespace BG {
namespace NES {
namespace Simulator {

int ExecuteNetmorphOperation(BG::Common::Logger::LoggingSystem* _Logger, NetmorphParameters* _Params) {

    _Logger->Log("Starting Netmorph Simulation", 5);
    _Params->Result = Netmorph(&_Params->Progress_percent, _Params->ModelFile);
    _Logger->Log("Netmorph Simulation Finished", 5);

    _Params->State = Netmorph_DONE;

    return 0;
}


}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG
