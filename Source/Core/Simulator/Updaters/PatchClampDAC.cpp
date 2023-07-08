#include <Simulator/Updaters/PatchClampDAC.h>


namespace BG {
namespace NES {
namespace Simulator {
namespace Updater {



void PatchClampDACReset(Tools::PatchClampDAC* _Target, Simulation* _Sim) {
    assert(_Target != nullptr && "You have passed _Target a null pointer!");

    // Nothing to do here - we probably don't need to clear the DAC's voltages so nothing to do...

}



void PatchClampDACUpdate(Tools::PatchClampDAC* _Target, Simulation* _Sim) {
    assert(_Target != nullptr && "You have passed _Target a null pointer!");

    // To be filled in

}



}; // Close Namespace Updater
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG