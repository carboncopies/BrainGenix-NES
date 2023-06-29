#include <Simulator/Engine.h>



namespace BG {
namespace NES {
namespace Simulator {


Engine::Engine() {
}

Engine::~Engine() {
}



void Engine::Reset(Simulation* _Sim) {
    assert(_Sim != nullptr && "Engine::Reset was passed a null _Sim (Simulation*) pointer!");

    // Reset Tools
    for (unsigned int i = 0; i < _Sim->PatchClampDACs.size(); i++) {
        Updater::PatchClampDACReset(&_Sim->PatchClampDACs[i]);
    }
    for (unsigned int i = 0; i < _Sim->PatchClampADCs.size(); i++) {
        Updater::PatchClampADCReset(&_Sim->PatchClampADCs[i]);
    }

    // Reset Connections
    for (unsigned int i = 0; i < _Sim->Receptors.size(); i++) {
        Updater::ReceptorReset(&_Sim->Receptors[i]);
    }
    for (unsigned int i = 0; i < _Sim->Staples.size(); i++) {
        Updater::StapleReset(&_Sim->Staples[i]);
    }

    

}

void Engine::RunFor(Simulation* _Sim) {
    assert(_Sim != nullptr && "Engine::RunFor was passed a null _Sim (Simulation*) pointer!");

}


}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG