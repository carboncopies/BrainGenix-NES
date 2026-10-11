//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: Test-only builders for tiny Simulation networks (dev_frame NT-5).
                 Header-only, included only by Simulation*.test.cpp.
                 Convention used throughout: compartment ID == shape ID, shapes are
                 all added before the compartments that point at them (the
                 compartment caches a raw pointer into the geometry vector).
*/

#pragma once

#include <cstring>
#include <string>
#include <vector>

#include <Simulator/Geometries/Box.h>
#include <Simulator/Geometries/Cylinder.h>
#include <Simulator/Geometries/Sphere.h>
#include <Simulator/Structs/BS.h>
#include <Simulator/Structs/LIFC.h>
#include <Simulator/Structs/Neuron.h>
#include <Simulator/Structs/Receptor.h>
#include <Simulator/Structs/Simulation.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace SimTest {

inline int AddSphereAt(Simulation& Sim, float x, float y, float z, float Radius = 5.0f) {
    Geometries::Sphere S(Geometries::Vec3D(x, y, z), Radius);
    S.ParentID = 0;
    return Sim.AddSphere(S);
}

inline int AddCylinderAt(Simulation& Sim, float x, float y, float z) {
    Geometries::Cylinder C(0.5f, Geometries::Vec3D(x, y, z), 0.5f, Geometries::Vec3D(x, y + 10.0f, z));
    C.ParentID = 0;
    return Sim.AddCylinder(C);
}

inline int AddBoxAt(Simulation& Sim, float x, float y, float z) {
    Geometries::Box B(Geometries::Vec3D(x, y, z), Geometries::Vec3D(2.0f, 2.0f, 2.0f));
    B.ParentID = 0;
    return Sim.AddBox(B);
}

inline Compartments::BS MakeBSCompartment(int ShapeID) {
    Compartments::BS C;
    C.ShapeID = ShapeID;
    C.MembranePotential_mV = -60.0f;
    C.SpikeThreshold_mV = -50.0f;
    C.DecayTime_ms = 30.0f;
    C.RestingPotential_mV = -60.0f;
    C.AfterHyperpolarizationAmplitude_mV = -20.0f;
    return C;
}

inline Compartments::LIFC MakeLIFCCompartment(int ShapeID) {
    Compartments::LIFC C;
    C.ShapeID = ShapeID;
    C.RestingPotential_mV = -70.0f;
    C.ResetPotential_mV = -70.0f;
    C.SpikeThreshold_mV = -50.0f;
    C.MembraneResistance_MOhm = 100.0f;
    C.MembraneCapacitance_pF = 100.0f;
    C.AfterHyperpolarizationAmplitude_mV = -10.0f;
    return C;
}

// Postsynaptic amplitude 0 keeps downstream neurons silent, so spike counts are deterministic.
inline CoreStructs::BSNeuronStruct MakeBSNeuronStruct(Simulation& Sim, int SomaCompID, int AxonCompID) {
    CoreStructs::BSNeuronStruct N;
    N.SomaCompartmentID = SomaCompID;
    N.AxonCompartmentID = AxonCompID;
    N.MembranePotential_mV = -60.0f;
    N.RestingPotential_mV = -60.0f;
    N.SpikeThreshold_mV = -50.0f;
    N.DecayTime_ms = 30.0f;
    N.AfterHyperpolarizationAmplitude_mV = -20.0f;
    N.PostsynapticPotentialRiseTime_ms = 5.0f;
    N.PostsynapticPotentialDecayTime_ms = 25.0f;
    N.PostsynapticPotentialAmplitude_nA = 0.0f;
    N.SomaCompartmentPtr = Sim.FindBSCompartmentByID(SomaCompID);
    N.AxonCompartmentPtr = Sim.FindBSCompartmentByID(AxonCompID);
    return N;
}

inline CoreStructs::SCNeuronStruct MakeSCNeuronStruct(const std::vector<int>& Soma,
                                                      const std::vector<int>& Dendrite = {},
                                                      const std::vector<int>& Axon = {}) {
    CoreStructs::SCNeuronStruct N;
    N.SomaCompartmentIDs = Soma;
    N.DendriteCompartmentIDs = Dendrite;
    N.AxonCompartmentIDs = Axon;
    N.MembranePotential_mV = -60.0f;
    N.RestingPotential_mV = -60.0f;
    N.SpikeThreshold_mV = -50.0f;
    N.DecayTime_ms = 30.0f;
    N.AfterHyperpolarizationAmplitude_mV = -20.0f;
    N.PostsynapticPotentialRiseTime_ms = 5.0f;
    N.PostsynapticPotentialDecayTime_ms = 25.0f;
    N.PostsynapticPotentialAmplitude_nA = 0.0f;
    return N;
}

// All time constants differ pairwise (compute_normalization throws when rise == decay).
inline CoreStructs::LIFCNeuronStruct MakeLIFCNeuronStruct(const std::vector<int>& Soma,
                                                          const std::vector<int>& Dendrite = {},
                                                          const std::vector<int>& Axon = {}) {
    CoreStructs::LIFCNeuronStruct N{};
    N.SomaCompartmentIDs = Soma;
    N.DendriteCompartmentIDs = Dendrite;
    N.AxonCompartmentIDs = Axon;
    N.RestingPotential_mV = -70.0f;
    N.ResetPotential_mV = -70.0f;
    N.SpikeThreshold_mV = -50.0f;
    N.MembraneResistance_MOhm = 100.0f;
    N.MembraneCapacitance_pF = 100.0f;
    N.RefractoryPeriod_ms = 2.0f;
    N.SpikeDepolarization_mV = 30.0f;
    N.UpdateMethod = CoreStructs::EXPEULER_CM;
    N.ResetMethod = CoreStructs::TOVM;
    N.AfterHyperpolarizationReversalPotential_mV = -90.0f;
    N.FastAfterHyperpolarizationRise_ms = 0.5f;
    N.FastAfterHyperpolarizationDecay_ms = 5.0f;
    N.FastAfterHyperpolarizationPeakConductance_nS = 0.1f;
    N.FastAfterHyperpolarizationMaxPeakConductance_nS = 1.0f;
    N.FastAfterHyperpolarizationHalfActConstant = 1.0f;
    N.SlowAfterHyperpolarizationRise_ms = 5.0f;
    N.SlowAfterHyperpolarizationDecay_ms = 50.0f;
    N.SlowAfterHyperpolarizationPeakConductance_nS = 0.01f;
    N.SlowAfterHyperpolarizationMaxPeakConductance_nS = 0.1f;
    N.SlowAfterHyperpolarizationHalfActConstant = 1.0f;
    N.AfterHyperpolarizationSaturationModel = CoreStructs::AHPCLIP;
    N.FatigueThreshold = 0.0f;
    N.FatigueRecoveryTime_ms = 100.0f;
    N.AfterDepolarizationReversalPotential_mV = 0.0f;
    N.AfterDepolarizationRise_ms = 1.0f;
    N.AfterDepolarizationDecay_ms = 10.0f;
    N.AfterDepolarizationPeakConductance_nS = 0.01f;
    N.AfterDepolarizationSaturationMultiplier = 2.0f;
    N.AfterDepolarizationRecoveryTime_ms = 100.0f;
    N.AfterDepolarizationDepletion = 0.1f;
    N.AfterDepolarizationSaturationModel = CoreStructs::ADPCLIP;
    N.AdaptiveThresholdDiffPerSpike = 0.0f;
    N.AdaptiveTresholdRecoveryTime_ms = 100.0f;
    N.AdaptiveThresholdDiffPotential_mV = 0.0f;
    N.AdaptiveThresholdFloor_mV = -80.0f;
    N.AdaptiveThresholdFloorDeltaPerSpike_mV = 0.0f;
    N.AdaptiveThresholdFloorRecoveryTime_ms = 100.0f;
    return N;
}

inline Connections::Receptor MakeReceptor(int SrcCompID, int DstCompID, const char* Neurotransmitter = "AMPA",
                                          float Conductance_nS = 1.0f) {
    Connections::Receptor R;
    R.SourceCompartmentID = SrcCompID;
    R.DestinationCompartmentID = DstCompID;
    R.Conductance_nS = Conductance_nS;
    R.safeset_Neurotransmitter(Neurotransmitter);
    return R;
}

inline Connections::LIFCReceptor MakeLIFCReceptor(int SrcCompID, int DstCompID,
                                                  Connections::NeurotransmitterType Type = Connections::AMPA,
                                                  float PeakConductance_nS = 0.5f) {
    Connections::LIFCReceptor R;
    R.SourceCompartmentID = SrcCompID;
    R.DestinationCompartmentID = DstCompID;
    R.Neurotransmitter = Type;
    R.PeakConductance_nS = PeakConductance_nS;
    R.PSPRise_ms = 0.5f;
    R.PSPDecay_ms = 3.0f;
    R.Weight = 1.0f;
    R.ReversalPotential_mV = (Type == Connections::GABA) ? -80.0f : 0.0f;
    return R;
}

/**
 * Ball-and-stick net of N neurons. Neuron i has soma sphere = shape 2i = compartment 2i and
 * axon cylinder = shape 2i+1 = compartment 2i+1. Soma i is centred at (100*i, 0, 0), radius 5.
 */
inline void BuildBSNet(Simulation& Sim, int N) {
    for (int i = 0; i < N; i++) {
        AddSphereAt(Sim, 100.0f * i, 0.0f, 0.0f, 5.0f);
        AddCylinderAt(Sim, 100.0f * i, 5.0f, 0.0f);
    }
    for (int i = 0; i < 2 * N; i++) {
        Compartments::BS C = MakeBSCompartment(i);
        Sim.AddSCCompartment(C, BSNEURONS);
    }
    for (int i = 0; i < N; i++) {
        CoreStructs::BSNeuronStruct NS = MakeBSNeuronStruct(Sim, 2 * i, 2 * i + 1);
        Sim.AddBSNeuron(NS);
    }
}

inline int SomaComp(int NeuronID) { return 2 * NeuronID; }
inline int AxonComp(int NeuronID) { return 2 * NeuronID + 1; }

inline int ConnectBS(Simulation& Sim, int PreNeuron, int PostNeuron, const char* NT = "AMPA", float G = 1.0f) {
    Connections::Receptor R = MakeReceptor(AxonComp(PreNeuron), SomaComp(PostNeuron), NT, G);
    return Sim.AddReceptor(R);
}

/** Same layout as BuildBSNet but with single-compartment LIFC neurons (soma only, shapes 0..N-1). */
inline void BuildLIFCNet(Simulation& Sim, int N) {
    for (int i = 0; i < N; i++) AddSphereAt(Sim, 100.0f * i, 0.0f, 0.0f, 5.0f);
    for (int i = 0; i < N; i++) {
        Compartments::LIFC C = MakeLIFCCompartment(i);
        Sim.AddLIFCCompartment(C);
    }
    for (int i = 0; i < N; i++) {
        CoreStructs::LIFCNeuronStruct NS = MakeLIFCNeuronStruct({i});
        Sim.AddLIFCNeuron(NS);
    }
}

inline int ConnectLIFC(Simulation& Sim, int PreNeuron, int PostNeuron,
                       Connections::NeurotransmitterType T = Connections::AMPA, float Peak = 0.5f) {
    Connections::LIFCReceptor R = MakeLIFCReceptor(PreNeuron, PostNeuron, T, Peak);
    return Sim.AddLIFCReceptor(R);
}

}; // namespace SimTest
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
