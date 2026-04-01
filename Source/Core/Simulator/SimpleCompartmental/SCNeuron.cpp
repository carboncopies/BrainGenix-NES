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
    Class_ = CoreStructs::_SCNeuron;

    Vm_mV = scneuronstruct.MembranePotential_mV;
    VRest_mV = scneuronstruct.RestingPotential_mV;
    VAct_mV = scneuronstruct.SpikeThreshold_mV;
    VAHP_mV = scneuronstruct.AfterHyperpolarizationAmplitude_mV;
    TauAHP_ms = scneuronstruct.DecayTime_ms;
    TauPSPr_ms = scneuronstruct.PostsynapticPotentialRiseTime_ms;
    TauPSPd_ms = scneuronstruct.PostsynapticPotentialDecayTime_ms;
    IPSP_nA = scneuronstruct.PostsynapticPotentialAmplitude_nA;

}

bool SCNeuron::Edit(const CoreStructs::SCNeuronStruct & scneuronstruct, const CoreStructs::SCEdit & edit) {
    if (edit.MembranePotential_mV) {
        build_data.MembranePotential_mV = scneuronstruct.MembranePotential_mV;
        Vm_mV = scneuronstruct.MembranePotential_mV;
    }
    if (edit.RestingPotential_mV) {
        build_data.RestingPotential_mV = scneuronstruct.RestingPotential_mV;
        VRest_mV = scneuronstruct.RestingPotential_mV;
    }
    if (edit.SpikeThreshold_mV) {
        build_data.SpikeThreshold_mV = scneuronstruct.SpikeThreshold_mV;
        VAct_mV = scneuronstruct.SpikeThreshold_mV;
    }
    if (edit.AfterHyperpolarizationAmplitude_mV) {
        build_data.AfterHyperpolarizationAmplitude_mV = scneuronstruct.AfterHyperpolarizationAmplitude_mV;
        VAHP_mV = scneuronstruct.AfterHyperpolarizationAmplitude_mV;
    }
    if (edit.DecayTime_ms) {
        build_data.DecayTime_ms = scneuronstruct.DecayTime_ms;
        TauAHP_ms = scneuronstruct.DecayTime_ms;
    }
    return true;
}

//! Returns the geometric center of the neuron.
//! In case the soma is constructed of multiple compartments,
//! this is a center of gravity of the set of compartment centers.
Geometries::Vec3D &SCNeuron::GetCellCenter() {
    Geometries::Vec3D geoCenter_um{};
    for (const auto & CompID : build_data.SomaCompartmentIDs) {
        if (CompID < Sim.BSCompartments.size()) {
            int ShapeID = Sim.BSCompartments.at(CompID).ShapeID;
            auto ShapePtr = Sim.FindShapeByID(ShapeID);
            if (ShapePtr != nullptr) {
                geoCenter_um = geoCenter_um + ShapePtr->Center_um;
            }
        }
    }
    GeoCenter_um = geoCenter_um / float(build_data.SomaCompartmentIDs.size());
    return GeoCenter_um;
};

BoundingBox SCNeuron::GetSomaBoundingBox(NES::VSDA::WorldInfo& _WorldInfo) {
    BoundingBox bb;
    bool is_first = true;
    for (const auto & CompID : build_data.SomaCompartmentIDs) {
        if (CompID < Sim.LIFCCompartments.size()) {
            int ShapeID = Sim.LIFCCompartments.at(CompID).ShapeID;
            auto ShapePtr = Sim.FindShapeByID(ShapeID);
            if (ShapePtr != nullptr) {
                if (is_first) {
                    bb = ShapePtr->GetBoundingBox(_WorldInfo);
                    is_first = false;
                } else {
                    bb.Enclosing(ShapePtr->GetBoundingBox(_WorldInfo));
                }
            }
        }
    }
    if (is_first) bb.Singularity();
    return bb;
}

/**
 * Here, we use the bounding box information for all the compartments in
 * the soma to obtain maximum extents in x,y,z and estimate a max
 * radius from that.
 */
float SCNeuron::GetSomaRadius() {
    // Find the overall bounding box of the soma.
    VSDA::WorldInfo WorldInfo_; // Using default values.
    BoundingBox bb = GetSomaBoundingBox(WorldInfo_);
    // Return half of the largest dimension as radius equivalent.
    return bb.LargestDim()/2.0;
}

}; // namespace Simulator
}; // namespace NES
}; // namespace BG
