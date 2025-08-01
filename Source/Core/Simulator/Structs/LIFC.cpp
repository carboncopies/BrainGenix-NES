#include <Simulator/Structs/LIFC.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace Compartments {

std::string LIFCBaseData::str() const {
    std::stringstream ss;
    ss << "ID: " << ID;
    ss << "\nShapeID: " << ShapeID;
    ss << "\nRestingPotential_mV: " << RestingPotential_mV;
    ss << "\nResetPotential_mV: " << ResetPotential_mV;
    ss << "\nSpikeThreshold_mV: " << SpikeThreshold_mV;
    ss << "\nMembraneResistance_MOhm: " << MembraneResistance_MOhm;
    ss << "\nMembraneCapacitance_pF: " << MembraneCapacitance_pF;
    ss << "\nAfterHyperpolarizationAmplitude_mV: " << AfterHyperpolarizationAmplitude_mV << '\n';
    return ss.str();
}

}; // Close Namespace Compartments
}; // Close Namespace Simulator
}; // Close Namespace NES
}; // Close Namespace BG
