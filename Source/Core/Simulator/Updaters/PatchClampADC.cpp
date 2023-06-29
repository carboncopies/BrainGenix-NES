#include <Simulator/Updaters/PatchClampADC.h>


namespace BG {
namespace NES {
namespace Simulator {
namespace Updater {



void PatchClampADCReset(Tools::PatchClampADC* _Target, Simulation* _Sim) {
    assert(_Target != nullptr && "You have passed _Target a null pointer!");

    // Clear Recorded Data
    _Target->RecordedData_mV.clear();

}



void PatchClampADCUpdate(Tools::PatchClampADC* _Target, Simulation* _Sim) {
    assert(_Target != nullptr && "You have passed _Target a null pointer!");

    float TargetCompartmentVoltage = _Sim->BSCompartments[_Target->SourceCompartmentID].MembranePotential_mV;

    // To be filled in

}



}; // Close Namespace Updater
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG