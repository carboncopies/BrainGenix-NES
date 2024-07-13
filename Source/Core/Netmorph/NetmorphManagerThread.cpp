//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")
#include <cmath>
#include <vector>
#include <memory>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <Include/Netmorph.h>
#include <Include/Embeddable.h>
#include <network.hh>
#include <synapse_structure.hh>

// Internal Libraries (BG convention: use <> instead of "")
#include <Netmorph/NetmorphManagerThread.h>
#include <Simulator/Geometries/Sphere.h>
#include <Simulator/Structs/BS.h>
#include <Simulator/Structs/Neuron.h>
#include <Simulator/Structs/Simulation.h>


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

// --- The following may be moved
// The following macros are used for easy access to linked lists built
// in the manner of 2009 Netmorph.
// #define PLL_LOOP_FORWARD(PLLType,Init_El,Extra_Test) \
//     for (PLLType * e = Init_El; ((e) && (Extra_Test)); e = e->Next())
// #define PLL_LOOP_FORWARD_NESTED(PLLType,Init_El,Extra_Test,RefVar) \
//     for (PLLType * RefVar = Init_El; ((RefVar) && (Extra_Test)); RefVar = RefVar->Next())

#define NETMORPH_PARAMS_FAIL(msg) { \
        _Params.Sim->Logger_->Log(msg, 7); \
        _Params.Result.Status = false; \
        _Params.Progress_percent = 100; \
    }

#define NETMORPH_PARAMS_SUCCESS(msg) { \
        _Params.Sim->Logger_->Log(msg, 5); \
        _Params.Result.Status = true; \
        _Params.Progress_percent = 100; \
    }

/**
 * @brief Recursively reach from the a root node at the soma down to the growth cones
 * at the ends of terminal segments, while building NES neurite segment components
 * along the branching tree.
 * 
 * Notes:
 * 1. The Netmorph fiber structure class `solid_fibre_segment` is not yet
 *    acively integrated for use in model growth. Consequently, Netmorph
 *    does not provide its R0 and R1 dual segment radii. Instead, we have
 *    to make do with the single `diameter` parameter of `fibre_segment`
 *    that is optionally calculated using the "neurite_diameter_model"
 *    settings. To make that work, we apply the following protocol:
 *    a) If a segment is a root segment then R0=R1=diameter.
 *    b) Otherwise, R0=parent->diameter, R1=diameter.
 */
bool RecursiveNeuriteBuild(bool IsAxon, NetmorphParameters& _Params, std::map<fibre_segment*, int>& SegmentIDMap, CoreStructs::SCNeuronStruct& N, fibre_segment* fsptr) {

    //*** MAYBE FIX REPEATED NAMES

    // Build neurite cylinder.
    Geometries::Cylinder S;
    S.End1Radius_um = fsptr->Diameter() <= 0.0 ? 1.0 : fsptr->Diameter();
    if (!fsptr->Parent()) {
        S.End0Radius_um = S.End1Radius_um;
    } else {
        S.End0Radius_um = fsptr->Parent()->Diameter();
        if (S.End0Radius_um <= 0.0) S.End0Radius_um = S.End1Radius_um;
    }
    S.End0Pos_um.x = fsptr->P0.X();
    S.End0Pos_um.y = fsptr->P0.Y();
    S.End0Pos_um.z = fsptr->P0.Z();
    S.End1Pos_um.x = fsptr->P1.X();
    S.End1Pos_um.y = fsptr->P1.Y();
    S.End1Pos_um.z = fsptr->P1.Z();
    S.Name = "cyl-"+N.Name;

    S.ID = _Params.Sim->AddCylinder(S);

    // Build neurite compartment.
    Compartments::BS C;
    C.ShapeID = S.ID;
    C.MembranePotential_mV = N.MembranePotential_mV;
    C.RestingPotential_mV = N.RestingPotential_mV;
    C.SpikeThreshold_mV = N.SpikeThreshold_mV;
    C.AfterHyperpolarizationAmplitude_mV = N.AfterHyperpolarizationAmplitude_mV;
    C.DecayTime_ms = N.DecayTime_ms;
    if (IsAxon) {
        C.Name = "axon-"+N.Name;
    } else {
        C.Name = "dendcomp-"+N.Name;
    }
    if (_Params.Sim->AddSCCompartment(C)<0) {
        NETMORPH_PARAMS_FAIL("RecursiveDendriteBuild failed: Missing Cylinder for neurite compartment build.");
        return false;
    }

    if (IsAxon) {
        N.AxonCompartmentIDs.emplace_back(C.ID);
    } else {
        N.DendriteCompartmentIDs.emplace_back(C.ID);
    }

    if ((!fsptr->Branch1()) && (!fsptr->Branch2())) {
        return true; // This is a terminal branch.
    }

    SegmentIDMap.emplace(fsptr, C.ID);

    //bool EndIsBifurcation = (Branch1() && Branch2()); // *** We may find this useful to know later.

    if (fsptr->Branch1()) if (!RecursiveNeuriteBuild(IsAxon, _Params, SegmentIDMap, N, fsptr->Branch1())) {
        NETMORPH_PARAMS_FAIL("RecursiveDendriteBuild failed on Branch1.");
        return false;
    }

    if (fsptr->Branch2()) if (!RecursiveNeuriteBuild(IsAxon, _Params, SegmentIDMap, N, fsptr->Branch2())) {
        NETMORPH_PARAMS_FAIL("RecursiveDendriteBuild failed on Branch2.");
        return false;
    }
    return true;
}

const std::map<synapse_type, float> conductances_nS = {
    { syntype_AMPAR, 40.0 },
    { syntype_NMDAR, 60.0 },
    { syntype_GABAR, -40.0 },
    { syntype_candidate, 0.0 },
    { syntype_GluR, 40.0 },
    { syntype_iGluR, 40.0 },
    { syntype_mGluR, 40.0 },
};

/**
 * In Netmorph, after growth, candidate synapse identification, synapse selectin, and
 * receptor type specification, a logical connection between two neurons ('connection')
 * has a linked list of synapses ('synapse'). The 'connection' specifies the
 * 'PreSynaptic()' and 'PostSynaptic()' neurons. Each 'synapse' stores a 'synapse_type',
 * as well as morphological information in a 'synapse_structure'.
 */
bool SynapsesBuild(NetmorphParameters& _Params, const std::map<fibre_segment*, int>& SegmentIDMap, CoreStructs::SCNeuronStruct& N, connection& conn, synapse& syn) {

    // Morphology shape.
    Geometries::Box S;
    // *** This needs better detailing to take what are clearly pre-
    //     and postsynaptic locations and to transform them into a
    //     morphology for synapses, with spines, terminals and receptors.

    auto center = (syn.Structure()->P0 + syn.Structure()->P1)/2.0;
    double x_absdiff = fabs(syn.Structure()->P0.X() - syn.Structure()->P1.X());
    double y_absdiff = fabs(syn.Structure()->P0.Y() - syn.Structure()->P1.Y());
    double z_absdiff = fabs(syn.Structure()->P0.Z() - syn.Structure()->P1.Z());
    S.Center_um.x = center.X();
    S.Center_um.y = center.Y();
    S.Center_um.z = center.Z();
    S.Dims_um.x = x_absdiff;
    S.Dims_um.y = y_absdiff;
    S.Dims_um.z = z_absdiff;
    // S.Rotations_rad = ...;
    S.Name = "box-"+N.Name;

    S.ID = _Params.Sim->AddBox(S);

    // Morphology compartment.
    Connections::Receptor C;
    auto it_presegment = SegmentIDMap.find(syn.Structure()->AxonSegment());
    if (it_presegment==SegmentIDMap.end()) {
        NETMORPH_PARAMS_FAIL("SynapsesBuild failed: Presynaptic neurite segment not found.");
        return false;
    }
    C.SourceCompartmentID  = it_presegment->second;
    auto it_postsegment = SegmentIDMap.find(syn.Structure()->AxonSegment());
    if (it_postsegment==SegmentIDMap.end()) {
        NETMORPH_PARAMS_FAIL("SynapsesBuild failed: Postsynaptic neurite segment not found.");
        return false;
    }
    C.DestinationCompartmentID = it_postsegment->second;

    C.ShapeID = S.ID;

    // Dynamics compartment.
    // *** This should probably set in accordance with receptor types and
    //     tuning.
    float connection_data_weight = 1.0; // *** E.g. right here!

    C.Neurotransmitter = synapse_type_name[syn.type_ID()];

    float conductance = conductances_nS.at(syn.type_ID());
    float receptor_conductance = conductance / connection_data_weight; // Divided by weight to avoid counter-intuitive weight interpretation.
    C.Conductance_nS = receptor_conductance;

    C.TimeConstantRise_ms = N.PostsynapticPotentialRiseTime_ms;
    C.TimeConstantDecay_ms = N.PostsynapticPotentialDecayTime_ms;

    C.Name = "syn-"+N.Name;

    C.ID = _Params.Sim->AddReceptor(C);
    if (C.ID<0) {
        NETMORPH_PARAMS_FAIL("SynapsesBuild failed: Source neuron or destination neuron not found.");
        return false;
    }

    return true;
}

// If NetmorphParams.Status==true then the NetmorphParams.net unique_ptr now owns the
// generated Netmorph network object. We can now use this to build our model in the
// simulation referenced by Handle.Sim().
bool BuildFromNetmorphNetwork(NetmorphParameters& _Params) {
    assert(_Params.Sim != nullptr);

    // *** We may want to obtain these from somewhere else:
    float neuron_Vm_mV = -60.0;
    float neuron_Vrest_mV = -60.0;
    float neuron_Vact_mV = -50.0;
    float neuron_Vahp_mV = -20.0;
    float neuron_tau_AHP_ms = 30.0;
    float neuron_tau_PSPr = 5.0;
    float neuron_tau_PSPd = 25.0;
    float neuron_IPSP = 870.0; // nA

    network& Net = *(_Params.Result.net);

    // The SegmentIDMap is used to map Netmorph neurite fibre segments to
    // NES compartment IDs. This is needed when building synapse receptor objects.
    // Note that there are no segments mapped to soma compartment IDs, because
    // Netmorph is exclusively involved with neurite outgrowth and does not define
    // soma morphology.
    std::map<fibre_segment*, int> SegmentIDMap;

    size_t nnum = 0;
    PLL_LOOP_FORWARD(neuron, Net.PLLRoot<neuron>::head(), 1) {

        // 0. Prepare neuron scaffold with lists.
        CoreStructs::SCNeuronStruct N;
        N.Name = std::to_string(uint64_t(e));
        N.MembranePotential_mV = neuron_Vm_mV;
        N.RestingPotential_mV = neuron_Vrest_mV;
        N.SpikeThreshold_mV = neuron_Vact_mV;
        N.DecayTime_ms = neuron_tau_AHP_ms;
        N.AfterHyperpolarizationAmplitude_mV = neuron_Vahp_mV;
        N.PostsynapticPotentialRiseTime_ms = neuron_tau_PSPr;
        N.PostsynapticPotentialDecayTime_ms = neuron_tau_PSPd;
        N.PostsynapticPotentialAmplitude_nA = neuron_IPSP;

        // 1. Build soma spheres.
        Geometries::Sphere S;
        S.Radius_um = e->Radius();
        S.Center_um.x = e->Pos().X();
        S.Center_um.y = e->Pos().Y();
        S.Center_um.z = e->Pos().Z();
        S.Name = "sphere-"+N.Name;
        _Params.Sim->AddSphere(S);

        // 2. Build soma compartments.
        Compartments::BS C;
        C.ShapeID = S.ID;
        C.MembranePotential_mV = N.MembranePotential_mV;
        C.RestingPotential_mV = N.RestingPotential_mV;
        C.SpikeThreshold_mV = N.SpikeThreshold_mV;
        C.AfterHyperpolarizationAmplitude_mV = N.AfterHyperpolarizationAmplitude_mV;
        C.DecayTime_ms = N.DecayTime_ms;
        C.Name = "somacomp-"+N.Name;
        if (_Params.Sim->AddSCCompartment(C)<0) {
            NETMORPH_PARAMS_FAIL("BuildFromNetmorphNetwork failed: Missing Sphere for soma compartment build.");
            return false;
        }

        N.SomaCompartmentIDs.emplace_back(C.ID);

        // 3. Build dendrites.
        //    *** Note: Because Netmorph almost makes all the nodes available
        //        we could quite easily also store information about the
        //        parentage dependencies of compartments, wherer there are
        //        terminal segments (and growth cones), etc.
        parsing_fs_type = dendrite_fs;
        PLL_LOOP_FORWARD_NESTED(fibre_structure, e->InputStructure()->head(), 1, dptr) {
            if (!RecursiveNeuriteBuild(false, _Params, SegmentIDMap, N, dptr)) {
                NETMORPH_PARAMS_FAIL("BuildFromNetmorphNetwork failed: Error in dendrite build.");
                return false;
            }
        }

        // 4. Build axons.
        parsing_fs_type = axon_fs;
        PLL_LOOP_FORWARD_NESTED(fibre_structure, e->OutputStructure()->head(), 1, aptr) {
            if (!RecursiveNeuriteBuild(true, _Params, SegmentIDMap, N, aptr)) {
                NETMORPH_PARAMS_FAIL("BuildFromNetmorphNetwork failed: Error in axon build.");
                return false;
            }
        }

        // 5. Build neurons.
        if (_Params.Sim->AddSCNeuron(N)<0) {
            NETMORPH_PARAMS_FAIL("BuildFromNetmorphNetwork failed: Missing soma compartment for neuron build.");
            return false;
        }

        // 8. Build synapse receptors.
        if (e->OutputConnections()->head()) {
            PLL_LOOP_FORWARD_NESTED(connection, e->OutputConnections()->head(), 1, conn) {
                PLL_LOOP_FORWARD_NESTED(synapse, conn->Synapses()->head(), 1, syn) {
                    if (!SynapsesBuild(_Params, SegmentIDMap, N, *conn, syn)) {
                        NETMORPH_PARAMS_FAIL("BuildFromNetmorphNetwork failed: Error in synapse build.");
                        return false;
                    }
                }
            }
        }

        nnum++;
    }

    NETMORPH_PARAMS_SUCCESS("Build NES model based on Netmorph output.");
    return true;
}
// ---
    

int ExecuteNetmorphOperation(BG::Common::Logger::LoggingSystem* _Logger, NetmorphParameters* _Params) {

    // Create link to redirect messaging from Netmorph to the NES Logger.
    embedlog = std::make_unique<Nm2NESLogging>(*_Logger);

    _Logger->Log("Starting Netmorph Simulation", 5);
    _Params->Result = Netmorph(&_Params->Progress_percent, _Params->ModelFile);
    _Logger->Log("Netmorph Simulation Finished", 5);

    if (_Params->Result.Status) {
        BuildFromNetmorphNetwork(*_Params);
    }

    _Params->State = Netmorph_DONE;

    return 0;
}


}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG
