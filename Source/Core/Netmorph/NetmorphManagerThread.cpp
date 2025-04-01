//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")
#include <cmath>
#include <vector>
#include <memory>
#include <cstring>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <Include/Netmorph.h>
#include <Include/Embeddable.h>
#include <global.hh>
#include <network.hh>
#include <synapse_structure.hh>
#include <fibre_structure.hh>
//#include <diagnostic.hh>
//#include <Sampled_Output.hh>
//#include <Txt_Object.hh>
//#include <prepost_structure.hh>
//#include <synapse.hh>
//#include <synapse_formation_model.hh>
//#include <axon_direction_model.hh>
//#include <environment_physics.hh>
//#include <slice.hh>
//#include <event.hh>

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
class NeuriteBuild: public fibre_tree_op {
public:
    bool IsAxon;
    NetmorphParameters& _Params;
    CoreStructs::SCNeuronStruct& N;
    size_t num_errors = 0;
    NeuriteBuild(bool _IsAxon, NetmorphParameters& Params, CoreStructs::SCNeuronStruct& _N): IsAxon(_IsAxon), _Params(Params), N(_N) {}
    virtual void op(fibre_segment* fs) {
        //*** MAYBE FIX REPEATED NAMES

        // Build neurite cylinder.
        Geometries::Cylinder S;
        S.End1Radius_um = fs->Diameter() <= 0.0 ? 1.0 : fs->Diameter();
        if (!fs->Parent()) {
            S.End0Radius_um = S.End1Radius_um;
        } else {
            S.End0Radius_um = fs->Parent()->Diameter();
            if (S.End0Radius_um <= 0.0) S.End0Radius_um = S.End1Radius_um;
        }
        S.End0Pos_um.x = fs->P0.X();
        S.End0Pos_um.y = fs->P0.Y();
        S.End0Pos_um.z = fs->P0.Z();
        S.End1Pos_um.x = fs->P1.X();
        S.End1Pos_um.y = fs->P1.Y();
        S.End1Pos_um.z = fs->P1.Z();
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
            C.Name = "dendrite-"+N.Name;
        }
        if (_Params.Sim->AddSCCompartment(C)<0) { // invalid C.ID
            num_errors++;
            NETMORPH_PARAMS_FAIL("RecursiveDendriteBuild failed: Missing Cylinder for neurite compartment build.");
        } else { // valid C.ID
            if (IsAxon) {
                N.AxonCompartmentIDs.emplace_back(C.ID);
            } else {
                N.DendriteCompartmentIDs.emplace_back(C.ID);
            }
            fs->cache.i = C.ID;
        }

    }
    void logerrors() const {
        if (num_errors>0) {
            _Params.Sim->Logger_->Log("NeuriteBuild: Number of errors: "+std::to_string(num_errors), 7);
        }
    }
};

const std::map<synapse_type, float> conductances_nS = {
    { syntype_AMPAR, 40.0 },
    { syntype_NMDAR, 60.0 },
    { syntype_GABAR, -40.0 },
    { syntype_candidate, 0.0 },
    { syntype_GluR, 40.0 },
    { syntype_iGluR, 40.0 },
    { syntype_mGluR, 40.0 },
};

// *** We may want to obtain these from somewhere else:
struct DynamicPars {
    float neuron_Vm_mV = -60.0;
    float neuron_Vrest_mV = -60.0;
    float neuron_Vact_mV = -50.0;
    float neuron_Vahp_mV = -20.0;
    float neuron_tau_AHP_ms = 30.0;
    float neuron_IPSP = 870.0; // nA
};

struct PSPTiming {
    float neuron_tau_PSPr = 5.0;
    float neuron_tau_PSPd = 25.0;
};

/**
 * In Netmorph, after growth, candidate synapse identification, synapse selectin, and
 * receptor type specification, a logical connection between two neurons ('connection')
 * has a linked list of synapses ('synapse'). The 'connection' specifies the
 * 'PreSynaptic()' and 'PostSynaptic()' neurons. Each 'synapse' stores a 'synapse_type',
 * as well as morphological information in a 'synapse_structure'.
 */
class SynapseBuild: public synapse_tree_op {
public:
    NetmorphParameters& _Params;
    const PSPTiming& psp_timing;
    size_t num_errors = 0;
    SynapseBuild(NetmorphParameters& Params, const PSPTiming& _psp_timing): _Params(Params), psp_timing(_psp_timing) {}
    virtual void op(synapse* s) {
        // Morphology shape.
        Geometries::Box S;
        // *** This needs better detailing to take what are clearly pre-
        //     and postsynaptic locations and to transform them into a
        //     morphology for synapses, with spines, terminals and receptors.

        auto center = (s->Structure()->P0 + s->Structure()->P1)/2.0;
        double x_absdiff = fabs(s->Structure()->P0.X() - s->Structure()->P1.X());
        double y_absdiff = fabs(s->Structure()->P0.Y() - s->Structure()->P1.Y());
        double z_absdiff = fabs(s->Structure()->P0.Z() - s->Structure()->P1.Z());
        S.Center_um.x = center.X();
        S.Center_um.y = center.Y();
        S.Center_um.z = center.Z();
        S.Dims_um.x = x_absdiff;
        S.Dims_um.y = y_absdiff;
        S.Dims_um.z = z_absdiff;
        // S.Rotations_rad = ...;
        S.Name = "synbox";

        S.ID = _Params.Sim->AddBox(S);

        // Morphology compartment.
        Connections::Receptor C;
        C.SourceCompartmentID = s->Structure()->AxonSegment()->cache.i;
        C.DestinationCompartmentID = s->Structure()->DendriteSegment()->cache.i;
        if (C.SourceCompartmentID<0) {
            num_errors++;
            NETMORPH_PARAMS_FAIL("SynapsesBuild failed: Presynaptic neurite segment not found.");
            return;
        }
        if (C.DestinationCompartmentID<0) {
            num_errors++;
            NETMORPH_PARAMS_FAIL("SynapsesBuild failed: Postsynaptic neurite segment not found.");
            return;
        }

        C.ShapeID = S.ID;

        // Dynamics compartment.
        // *** This should probably set in accordance with receptor types and
        //     tuning.
        float connection_data_weight = 1.0; // *** E.g. right here!

        // *** We may have to do a mapping from Netmorph types to NES types so
        //     that the Neuron::UpdateType() function works correctly, and
        //     so that GetConnectionType() works correctly.
        C.safeset_Neurotransmitter(synapse_type_name[s->type_ID()]);

        float conductance = conductances_nS.at(s->type_ID());
        float receptor_conductance = conductance / connection_data_weight; // Divided by weight to avoid counter-intuitive weight interpretation.
        C.Conductance_nS = receptor_conductance;

        C.TimeConstantRise_ms = psp_timing.neuron_tau_PSPr;
        C.TimeConstantDecay_ms = psp_timing.neuron_tau_PSPd;

        C.Name = "synapse";

        C.ID = _Params.Sim->AddReceptor(C);
        if (C.ID<0) {
            num_errors++;
            NETMORPH_PARAMS_FAIL("SynapsesBuild failed: Source neuron or destination neuron not found.");
            return;
        }

    }
    void logerrors() const {
        if (num_errors>0) {
            _Params.Sim->Logger_->Log("SynapseBuild: Number of errors: "+std::to_string(num_errors), 7);
        }
    }
};

class FindUnParsedSegments: public fibre_tree_op {
public:
    NetmorphParameters& _Params;
    FindUnParsedSegments(NetmorphParameters& Params): _Params(Params) {}
    virtual void op(fibre_segment* fs) {
        if (fs->cache.i<0) {
            _Params.Sim->Logger_->Log("Unparsed segment: "+std::to_string(uint64_t(fs)), 7);
        }
    }
};

class CountUnParsedSegments: public fibre_tree_op {
public:
    NetmorphParameters& _Params;
    size_t num_unparsed = 0;
    CountUnParsedSegments(NetmorphParameters& Params): _Params(Params) {}
    virtual void op(fibre_segment* fs) {
        if (fs->cache.i<0) {
            num_unparsed++;
        }
    }
    void log() const {
        if (num_unparsed>0) {
            _Params.Sim->Logger_->Log("Number of unparsed segments: "+std::to_string(num_unparsed), 7);
        }
    }
};

class NeuronBuild: public neuron_list_op {
public:
    NetmorphParameters& _Params;
    const DynamicPars& dynpars;
    const PSPTiming& psp_timing;
    size_t num_errors = 0;
    std::map<long, int> NetmorphNeuronID2NESNeuronIDMap;
    NeuronBuild(NetmorphParameters& Params, const DynamicPars& _dynpars, const PSPTiming& _psp_timing): _Params(Params), dynpars(_dynpars), psp_timing(_psp_timing) {}
    virtual void op(neuron* n) {

        // 0. Prepare neuron scaffold with lists.
        CoreStructs::SCNeuronStruct N;
        N.Name = std::to_string(uint64_t(n));
        N.MembranePotential_mV = dynpars.neuron_Vm_mV;
        N.RestingPotential_mV = dynpars.neuron_Vrest_mV;
        N.SpikeThreshold_mV = dynpars.neuron_Vact_mV;
        N.DecayTime_ms = dynpars.neuron_tau_AHP_ms;
        N.AfterHyperpolarizationAmplitude_mV = dynpars.neuron_Vahp_mV;
        N.PostsynapticPotentialRiseTime_ms = psp_timing.neuron_tau_PSPr;
        N.PostsynapticPotentialDecayTime_ms = psp_timing.neuron_tau_PSPd;
        N.PostsynapticPotentialAmplitude_nA = dynpars.neuron_IPSP;

        // 1. Build soma spheres.
        Geometries::Sphere S;
        S.Radius_um = n->Radius();
        S.Center_um.x = n->Pos().X();
        S.Center_um.y = n->Pos().Y();
        S.Center_um.z = n->Pos().Z();
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
            num_errors++;
            NETMORPH_PARAMS_FAIL("NeuronBuild failed: Missing Sphere for soma compartment build.");
            return;
        }

        N.SomaCompartmentIDs.emplace_back(C.ID);

        // 3. Build dendrites.
        //    *** Note: Because Netmorph almost makes all the nodes available
        //        we could quite easily also store information about the
        //        parentage dependencies of compartments, wherer there are
        //        terminal segments (and growth cones), etc.
        _Params.Result.base->fs->parsing_fs_type = dendrite_fs;
        //glb_fs->parsing_fs_type = dendrite_fs;
        NeuriteBuild dendrites_build(false, _Params, N);
        n->tree_op(dendrites_build);
        dendrites_build.logerrors();

        // 4. Build axons.
        _Params.Result.base->fs->parsing_fs_type = axon_fs;
        //glb_fs->parsing_fs_type = axon_fs;
        NeuriteBuild axons_build(true, _Params, N);
        n->tree_op(axons_build);
        axons_build.logerrors();

        // 5. Build neurons.
        if (_Params.Sim->AddSCNeuron(N)<0) {
            num_errors++;
            NETMORPH_PARAMS_FAIL("NeuronBuild failed: Missing soma compartment for neuron build.");
            return;
        }

        // Store NES Neuron ID and Netmorph numerical ID pair for later use.
        NetmorphNeuronID2NESNeuronIDMap[n->numerical_ID()] = N.ID;

    }
    void logerrors() const {
        if (num_errors>0) {
            _Params.Sim->Logger_->Log("BuildFromNetmorphNetwork: Number of errors: "+std::to_string(num_errors), 7);
        }
    }
    int get_mapped_id(long numID) const {
        auto it = NetmorphNeuronID2NESNeuronIDMap.find(numID);
        if (it==NetmorphNeuronID2NESNeuronIDMap.end()) return -1;
        return it->second;
    }
    int get_mapped_id_by_ptr(neuron* n) const {
        return get_mapped_id(n->numerical_ID());
    }
};

/**
 * For each region:
 * - Create a corresponding region and neural circuit in the NES simulation.
 * - Add to each region the neurons that were created that correspond with the neurons
 *   in those regions in Netmorph.
 */
class RegionBuild: public region_list_op {
public:
    NetmorphParameters& _Params;
    NeuronBuild& neuron_build;
    int CurrentCircuitID = -1;
    size_t num_errors = 0;
    RegionBuild(NetmorphParameters& Params, NeuronBuild& _neuronbuild): _Params(Params), neuron_build(_neuronbuild) { }
    virtual void neuron_op(neuron* n) {
        // Find the corresponding NES neuron.
        int NeuronID = neuron_build.get_mapped_id_by_ptr(n);
        if (NeuronID<0) {
            num_errors++;
            NETMORPH_PARAMS_FAIL("RegionBuild error: Unrecognized neuron in region.");
            return;
        }
        // Add that neuron to the current region.
        if (CurrentCircuitID<0) {
            num_errors++;
            NETMORPH_PARAMS_FAIL("RegionBuild error: No current circuit.");
            return;
        }
        if (CurrentCircuitID>=_Params.Sim->NeuralCircuits.size()) {
            num_errors++;
            NETMORPH_PARAMS_FAIL("RegionBuild error: CurrentCircuitID exceeds number of circuits.");
            return;
        }
        auto CircuitPtr = _Params.Sim->NeuralCircuits.at(CurrentCircuitID).get();
        if (!CircuitPtr) {
            num_errors++;
            NETMORPH_PARAMS_FAIL("RegionBuild error: Circuit pointer is null pointer.");
            return;
        }
        CircuitPtr->AddNeuronByID(NeuronID);
    }
    virtual void op(region* r) {
        BrainRegions::BrainRegion R;
        R.SetName(r->Name().chars());
        if (_Params.Sim->AddRegion(R)<0) {
            num_errors++;
            NETMORPH_PARAMS_FAIL("RegionBuild failed: Missing data for region build.");
            return;
        }
        CurrentCircuitID = R.CircuitID;
    }
    void logerrors() const {
        if (num_errors>0) {
            _Params.Sim->Logger_->Log("RegionBuild: Number of errors: "+std::to_string(num_errors), 7);
        }
    }
};

// If NetmorphParams.Status==true then the NetmorphParams.net unique_ptr now owns the
// generated Netmorph network object. We can now use this to build our model in the
// simulation referenced by Handle.Sim().
bool BuildFromNetmorphNetwork(NetmorphParameters& _Params) {
    assert(_Params.Sim != nullptr);

    DynamicPars dynpars;
    PSPTiming psp_timing;

    network& Net = *(_Params.Result.net);

    // Make sure the common cache integer valus of all fiber segments are
    // initialized to -1.
    set_cache_int op;
    op.value = -1;
    Net.tree_op(op);

    CountUnParsedSegments unparsed_pre(_Params);
    Net.tree_op(unparsed_pre);
    unparsed_pre.log();

    NeuronBuild neuron_build(_Params, dynpars, psp_timing);
    Net.neuron_op(neuron_build);
    neuron_build.logerrors();

    CountUnParsedSegments unparsed_post(_Params);
    Net.tree_op(unparsed_post);
    unparsed_post.log();

    // FindUnParsedSegments findunparsed(_Params);
    // Net.tree_op(findunparsed);

    // Synapses need to be processed after all segments have received IDs.
    SynapseBuild synapse_build(_Params, psp_timing);
    Net.synapse_op(synapse_build);
    synapse_build.logerrors();

    RegionBuild region_build(_Params, neuron_build);
    Net.region_op(region_build);
    region_build.logerrors();

    NETMORPH_PARAMS_SUCCESS("Build NES model based on Netmorph output.");
    return true;
}
// ---

Netmorph NetmorphRun(int* _StatusPercent, const std::string& _Modelfile, Netmorph2NESLogging* _embedlog, const std::string& _OverrideOutputDir) {

  Netmorph res(_embedlog, _StatusPercent);
  res.embedded_run(_Modelfile, _OverrideOutputDir);

  return res;
}

int ExecuteNetmorphOperation(BG::Common::Logger::LoggingSystem* _Logger, NetmorphParameters* _Params) {

    // Create link to redirect messaging from Netmorph to the NES Logger.
    std::unique_ptr<Nm2NESLogging> _embedlog = std::make_unique<Nm2NESLogging>(*_Logger);

    _Logger->Log("Starting Netmorph Simulation", 5);
    _Params->Result = NetmorphRun(&_Params->Progress_percent, _Params->ModelContent, _embedlog.release(), _Params->OutputDirectory);

    _Logger->Log("Netmorph Simulation Finished", 5);

    if (_Params->Result.Status) {
        BuildFromNetmorphNetwork(*_Params);
    }

    _Params->Result.postop();
    _Params->State = Netmorph_DONE;

    return 0;
}


}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG
