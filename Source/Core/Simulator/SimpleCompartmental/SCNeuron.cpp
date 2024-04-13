#include <Simulator/Structs/Simulation.h>
#include <Simulator/SimpleCompartmental/SCNeuron.h>

#include <algorithm>
#include <iostream>

namespace BG {
namespace NES {
namespace Simulator {

// *** TODO: We could clean up all of these this-> pointers. The virtual functions
//           should still work the same way.

//! Constructors
// SCNeuron::SCNeuron(int _ID, std::unordered_map<std::string, Geometries::Geometry*> & morphology): BSNeuron(ID) {
//     // *** Add more here...
// }

SCNeuron::SCNeuron(const CoreStructs::SCNeuronStruct & scneuronstruct, Simulation & _Sim): BSNeuron(scneuronstruct.ID), Sim(_Sim) {
    build_data = scneuronstruct;

    //ID = scneuronstruct.ID;
    Vm_mV = scneuronstruct.MembranePotential_mV;
    VRest_mV = scneuronstruct.RestingPotential_mV;
    VAct_mV = scneuronstruct.SpikeThreshold_mV;
    VAHP_mV = scneuronstruct.AfterHyperpolarizationAmplitude_mV;
    TauAHP_ms = scneuronstruct.DecayTime_ms;
    TauPSPr_ms = scneuronstruct.PostsynapticPotentialRiseTime_ms;
    TauPSPd_ms = scneuronstruct.PostsynapticPotentialDecayTime_ms;
    IPSP_nA = scneuronstruct.PostsynapticPotentialAmplitude_nA;

}

//! Returns the geometric center of the neuron.
//! In case the soma is constructed of multiple compartments,
//! this is a center of gravity of the set of compartment centers.
Geometries::Vec3D &SCNeuron::GetCellCenter() {
    Geometries::Vec3D geoCenter_um{};
    for (const auto & CompID : build_data.SomaCompartmentIDs) {
        auto CompPtr = Sim.FindCompartmentByID(CompID);
        assert(CompPtr != nullptr);
        auto ShapeID = CompPtr->ShapeID;
        auto ShapePtr = Sim.FindShapeByID(ShapeID);
        assert(ShapePtr != nullptr);
        geoCenter_um = geoCenter_um + ShapePtr->Center_um;
    }
    GeoCenter_um = geoCenter_um / float(build_data.SomaCompartmentIDs.size());
    return GeoCenter_um;
};

}; // namespace Simulator
}; // namespace NES
}; // namespace BG
