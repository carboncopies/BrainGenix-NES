//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the Neuron abstract struct.
    Additional Notes: None
    Date Created: 2024-01-13
*/

#include <iostream>
#include <algorithm> // for std::sort(), std::min(), std::max()

#include <Simulator/Structs/Simulation.h>
#include <Simulator/Structs/Neuron.h>
#include <Simulator/LIFCompartmental/LIFCNeuron.h>
#include <Simulator/Structs/Receptor.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace CoreStructs {

// Use this to warn about accidentally having reached the wrong
// OOP level:
#define WARNWRONGOOPLEVEL() {\
    std::cout << "DEBUG --> Wrong OOP level: " << __func__ << '\n'; std::cout.flush();\
}

ReceptorDataBase::ReceptorDataBase(Neuron * _SNPtr, Neuron * _DNPtr):
    SrcNeuronID(_SNPtr->ID), DstNeuronID(_DNPtr->ID),
    SrcNeuronPtr(_SNPtr), DstNeuronPtr(_DNPtr) {
}

ReceptorData::ReceptorData(int _RID, Connections::Receptor * _RPtr, Neuron * _SNPtr, Neuron * _DNPtr):
    ReceptorDataBase(_SNPtr, _DNPtr), ReceptorID(_RID), ReceptorPtr(_RPtr) {
}

LIFCReceptorData::LIFCReceptorData(int _RID, Connections::LIFCReceptor * _RPtr, Neuron * _SNPtr, Neuron * _DNPtr):
    ReceptorDataBase(_SNPtr, _DNPtr) {

    ReceptorIDs.emplace_back(_RID);
    ReceptorPtrs.emplace_back(_RPtr);

    g_peak_sum_nS = _RPtr->PeakConductance_nS;
    weight = _RPtr->Weight;
    weight_g_peak_sum = _RPtr->Weight*_RPtr->PeakConductance_nS;
    tau_rise_ms = _RPtr->PSPRise_ms;
    tau_decay_ms = _RPtr->PSPDecay_ms;
    onset_delay_ms = _RPtr->OnsetDelay_ms;
    if (!dynamic_cast<LIFCNeuron*>(_SNPtr)->Sim.use_abstracted_LIF_receptors) {
        norm = Connections::compute_normalization(tau_rise_ms, tau_decay_ms);
    }
}

float LIFCReceptorData::E_k() {
    return ReceptorPtrs.at(0)->ReversalPotential_mV;
}

bool LIFCReceptorData::voltage_gated() {
    return ReceptorPtrs.at(0)->voltage_gated;
}

Connections::NeurotransmitterType LIFCReceptorData::Type() {
    return ReceptorPtrs.at(0)->Neurotransmitter;
}

Connections::LIFCSTDPMethodEnum LIFCReceptorData::STDP_Method() {
    return ReceptorPtrs.at(0)->STDP_Method;
}

// This is for the abstracted parameters that do not use a median.
void LIFCReceptorData::AddToAbstractedFunctional(int _RID, Connections::LIFCReceptor * _RPtr) {
    g_peak_sum_nS += _RPtr->PeakConductance_nS;
    weight_g_peak_sum += _RPtr->Weight*_RPtr->PeakConductance_nS;
    weight = weight_g_peak_sum / g_peak_sum_nS;
}

// This is for the abstracted parameters that do use a median.
// It is run once at the start of simulation.
void LIFCReceptorData::Calculate_Abstracted_PSP_Medians() {
    std::vector<float> tau_rises;
    std::vector<float> tau_decays;
    std::vector<float> onset_delays;
    for (auto& LIFCRptr : ReceptorPtrs) {
        tau_rises.push_back(LIFCRptr->PSPRise_ms);
        tau_decays.push_back(LIFCRptr->PSPDecay_ms);
        onset_delays.push_back(LIFCRptr->OnsetDelay_ms);
    }
    std::sort(tau_rises.begin(), tau_rises.end());
    std::sort(tau_decays.begin(), tau_decays.end());
    std::sort(onset_delays.begin(), onset_delays.end());
    size_t size = tau_rises.size();
    if (size % 2 == 1) {
        tau_rise_ms = tau_rises[size / 2];
        tau_decay_ms = tau_decays[size / 2];
        onset_delay_ms = onset_delays[size / 2];
    } else {
        tau_rise_ms = (tau_rises[size / 2 - 1] + tau_rises[size / 2]) / 2.0;
        tau_decay_ms = (tau_decays[size / 2 - 1] + tau_decays[size / 2]) / 2.0;
        onset_delay_ms = (onset_delays[size / 2 - 1] + onset_delays[size / 2]) / 2.0;
    }
    norm = Connections::compute_normalization(tau_rise_ms, tau_decay_ms);

    if (STDP_Method() != Connections::STDPNONE) {
        std::vector<float> STDP_A_poses;
        std::vector<float> STDP_A_negs;
        std::vector<float> STDP_tau_poses;
        std::vector<float> STDP_tau_negs;
        for (auto& LIFCRptr : ReceptorPtrs) {
            STDP_A_poses.push_back(LIFCRptr->STDP_A_pos);
            STDP_A_negs.push_back(LIFCRptr->STDP_A_neg);
            STDP_tau_poses.push_back(LIFCRptr->STDP_Tau_pos);
            STDP_tau_negs.push_back(LIFCRptr->STDP_Tau_neg);
        }
        std::sort(STDP_A_poses.begin(), STDP_A_poses.end());
        std::sort(STDP_A_negs.begin(), STDP_A_negs.end());
        std::sort(STDP_tau_poses.begin(), STDP_tau_poses.end());
        std::sort(STDP_tau_negs.begin(), STDP_tau_negs.end());
        if (size % 2 == 1) {
            STDP_A_pos = STDP_A_poses[size / 2];
            STDP_A_neg = STDP_A_negs[size / 2];
            STDP_Tau_pos = STDP_tau_poses[size / 2];
            STDP_Tau_neg = STDP_tau_negs[size / 2];
        } else {
            STDP_A_pos = (STDP_A_poses[size / 2 - 1] + STDP_A_poses[size / 2]) / 2.0;
            STDP_A_neg = (STDP_A_negs[size / 2 - 1] + STDP_A_negs[size / 2]) / 2.0;
            STDP_Tau_pos = (STDP_tau_poses[size / 2 - 1] + STDP_tau_poses[size / 2]) / 2.0;
            STDP_Tau_neg = (STDP_tau_negs[size / 2 - 1] + STDP_tau_negs[size / 2]) / 2.0;
        }
    }
}

// Effect of voltage-gated Mg2+ block.
float B_NMDA(float V, float Mg = 1.0) {
    float gamma = 0.33;  // per mM
    float beta = 0.062;  // per mV
    return 1.0 / (1.0 + gamma * Mg * exp(-beta * V));
}

void LIFCReceptorData::Update_Conductance(float t, float Vm) {
    auto& syn_times = SrcNeuronPtr->TAct_ms;
    if (voltage_gated()) {
        g_k = std::min(g_peak_sum_nS, B_NMDA(Vm) * weight * g_peak_sum_nS * 
                     Connections::g_norm(t, syn_times, tau_rise_ms, tau_decay_ms, norm, onset_delay_ms));
    } else {
        g_k = std::min(g_peak_sum_nS, weight * g_peak_sum_nS * 
                     Connections::g_norm(t, syn_times, tau_rise_ms, tau_decay_ms, norm, onset_delay_ms));
    }
}

// See the platform paper for a detailed description of the biophysics and
// equations underlying this STDP implementation.
void LIFCReceptorData::STDP_Update(float tfire) {
    if (SrcNeuronPtr->TAct_ms.empty()) return;

    float t_pre = SrcNeuronPtr->TAct_ms.back();
    
    if (STDP_Method() == Connections::STDPNONE) return;
    
    float dt_spikes;
    if (STDP_Method() == Connections::STDPANTIHEBBIAN) {
        dt_spikes = t_pre - tfire;
    } else {
        dt_spikes = tfire - t_pre;
    }
    
    float dw;
    if (dt_spikes > 0) {
        dw = STDP_A_pos * exp(-dt_spikes / STDP_Tau_pos);
    } else {
        dw = -STDP_A_neg * exp(dt_spikes / STDP_Tau_neg);
    }
    
    weight += dw;
    weight = std::max(0.0f, std::min(1.0f, weight));
}

//! Update the assumed neuron type based on its neurotransmitters.
//! Note: For now, we only use the first 4 characters to match, so
//!       that we can also handle some variants, such as AMPAR that
//!       are used in Netmorph.
void Neuron::UpdateType(const std::string & neurotransmitter) {
    // Only do this if it is still "unknown".
    if (Type_==UnknownNeuron) {
        auto it = Neurotransmitter2NeuronType.find(neurotransmitter.substr(0, 4));
        if (it != Neurotransmitter2NeuronType.end()) {
            Type_ = it->second;
        }
    }
}

//! Returns the time since the action potential threshold was
//! crossed last.
float Neuron::DtAct_ms(float t_ms) {
    WARNWRONGOOPLEVEL();
    return 0.0; // Placeholder.
};

//! Tells if the action potential threshold has been crossed.
bool Neuron::HasSpiked() {
    WARNWRONGOOPLEVEL();
    return false; // Placeholder.
}

//! Returns the geometric center of the neuron.
Geometries::Vec3D &Neuron::GetCellCenter() {
    WARNWRONGOOPLEVEL();
    return cell_center; // Placeholder.
}

void Neuron::AddSpecificAPTime(float t_ms) {
    TDirectStim_ms.emplace_back(t_ms);
}

void Neuron::Update(float t_ms, bool recording) {
    // Placeholder.
    WARNWRONGOOPLEVEL();
}

nlohmann::json Neuron::GetSpikeTimesJSON() const {
    nlohmann::json spiketimes;
    spiketimes["tSpike_ms"] = nlohmann::json(this->TAct_ms);
    return spiketimes;
}

nlohmann::json Neuron::GetRecordingJSON() const {
    // Placeholder.
    nlohmann::json recording;
    WARNWRONGOOPLEVEL();
    return recording;
}

void Neuron::SetSpontaneousActivity(float mean, float stdev, int Seed) {
    WARNWRONGOOPLEVEL();
}

void Neuron::InputReceptorAdded(ReceptorData* RData) {
    WARNWRONGOOPLEVEL();
}

void Neuron::OutputTransmitterAdded(ReceptorData* RData) {
    WARNWRONGOOPLEVEL();
}

std::string SCNeuronBase::str() const {
    std::stringstream ss;
    ss << "ID: " << ID;
    ss << "\nMembranePotential_mV: " << MembranePotential_mV;
    ss << "\nRestingPotential_mV: " << RestingPotential_mV;
    ss << "\nSpikeThreshold_mV: " << SpikeThreshold_mV;
    ss << "\nDecayTime_ms: " << DecayTime_ms;
    ss << "\nAfterHyperpolarizationAmplitude_mV: " << AfterHyperpolarizationAmplitude_mV;
    ss << "\nPostsynapticPotentialRiseTime_ms: " << PostsynapticPotentialRiseTime_ms;
    ss << "\nPostsynapticPotentialDecayTime_ms: " << PostsynapticPotentialDecayTime_ms;
    ss << "\nPostsynapticPotentialAmplitude_nA: " << PostsynapticPotentialAmplitude_nA << '\n';
    return ss.str();
}


std::string SCNeuronStructFlatHeader::str() const {
    std::stringstream ss;
    ss << "FlatBufSize: " << FlatBufSize;
    ss << "\nNameSize: " << NameSize;
    ss << "\nNameOffset: " << NameOffset;
    ss << "\nSomaCompartmentIDsSize: " << SomaCompartmentIDsSize;
    ss << "\nSomaCompartmentIDsOffset: " << SomaCompartmentIDsOffset;
    ss << "\nDendriteCompartmentIDsSize: " << DendriteCompartmentIDsSize;
    ss << "\nDendriteCompartmentIDsOffset: " << DendriteCompartmentIDsOffset;
    ss << "\nAxonCompartmentIDsSize: " << AxonCompartmentIDsSize;
    ss << "\nAxonCompartmentIDsOffset: " << AxonCompartmentIDsOffset << '\n';
    ss << Base.str();
    return ss.str();
}

std::string SCNeuronStructFlatHeader::name_str() const {
    std::stringstream ss;
    ss << "Name: " << (const char*) ADD_BYTES_TO_POINTER(this, NameOffset) << '\n';
    return ss.str();
}

std::string SCNeuronStructFlatHeader::scid_str() const {
    std::stringstream ss;
    int* scidptr = (int*) ADD_BYTES_TO_POINTER(this, SomaCompartmentIDsOffset);
    //ss << "flat header address = " << uint64_t(this) << '\n';
    //ss << "scidptr = " << uint64_t(scidptr) << '\n';
    ss << "SomaCompartmentIDs: ";
    for (size_t idx = 0; idx < SomaCompartmentIDsSize; idx++) {
        ss << scidptr[idx] << ' ';
    }
    ss << '\n';
    return ss.str();
}

std::string SCNeuronStructFlatHeader::dcid_str() const {
    std::stringstream ss;
    int* dcidptr = (int*) ADD_BYTES_TO_POINTER(this, DendriteCompartmentIDsOffset);
    ss << "DendriteCompartmentIDs: ";
    for (size_t idx = 0; idx < DendriteCompartmentIDsSize; idx++) {
        ss << dcidptr[idx] << ' ';
    }
    ss << '\n';
    return ss.str();
}

std::string SCNeuronStructFlatHeader::acid_str() const {
    std::stringstream ss;
    int* acidptr = (int*) ADD_BYTES_TO_POINTER(this, AxonCompartmentIDsOffset);
    ss << "AxonCompartmentIDs: ";
    for (size_t idx = 0; idx < AxonCompartmentIDsSize; idx++) {
        ss << acidptr[idx] << ' ';
    }
    ss << '\n';
    return ss.str();
}

/**
 * Return a flattened struct describing name and compartment indices of
 * the SCNeuron. This is useful for storage, among other things.
 */
std::unique_ptr<uint8_t[]> SCNeuronStruct::GetFlat() const {
    SCNeuronStructFlatHeader header;

    header.Base = *this;
    header.FlatBufSize = sizeof(header);

    header.NameSize = Name.size()+1; // for c_str()
    header.NameOffset = sizeof(header);
    size_t NameSizeOf = sizeof(decltype(Name)::value_type)*header.NameSize;
    header.FlatBufSize += NameSizeOf;

    header.SomaCompartmentIDsSize = SomaCompartmentIDs.size();
    header.SomaCompartmentIDsOffset = header.NameOffset + NameSizeOf;
    size_t SomaCompartmentIDsSizeOf = sizeof(decltype(SomaCompartmentIDs)::value_type)*header.SomaCompartmentIDsSize;
    header.FlatBufSize += SomaCompartmentIDsSizeOf;

    header.DendriteCompartmentIDsSize = DendriteCompartmentIDs.size();
    header.DendriteCompartmentIDsOffset = header.SomaCompartmentIDsOffset + SomaCompartmentIDsSizeOf;
    size_t DendriteCompartmentIDsSizeOf = sizeof(decltype(DendriteCompartmentIDs)::value_type)*header.DendriteCompartmentIDsSize;
    header.FlatBufSize += DendriteCompartmentIDsSizeOf;

    header.AxonCompartmentIDsSize = AxonCompartmentIDs.size();
    header.AxonCompartmentIDsOffset = header.DendriteCompartmentIDsOffset + DendriteCompartmentIDsSizeOf;
    size_t AxonCompartmentIDsSizeOf = sizeof(decltype(AxonCompartmentIDs)::value_type)*header.AxonCompartmentIDsSize;
    header.FlatBufSize += AxonCompartmentIDsSizeOf;
    

    std::unique_ptr<uint8_t[]> flatbuf = std::make_unique<uint8_t[]>(header.FlatBufSize);

    memcpy(flatbuf.get(), &header, sizeof(header));
    memcpy(flatbuf.get()+header.NameOffset, Name.c_str(), header.NameSize);
    memcpy(flatbuf.get()+header.SomaCompartmentIDsOffset, SomaCompartmentIDs.data(), SomaCompartmentIDsSizeOf);
    memcpy(flatbuf.get()+header.DendriteCompartmentIDsOffset, DendriteCompartmentIDs.data(), DendriteCompartmentIDsSizeOf);
    memcpy(flatbuf.get()+header.AxonCompartmentIDsOffset, AxonCompartmentIDs.data(), AxonCompartmentIDsSizeOf);

    return flatbuf;
}

/**
 * (Re)instantiate this object from a flat representation, e.g. from
 * storage.
 */
bool SCNeuronStruct::FromFlat(SCNeuronStructFlatHeader* header) {
    SCNeuronBase::operator=(header->Base);

    Name = (const char*) ADD_BYTES_TO_POINTER(header, header->NameOffset);

    SomaCompartmentIDs.resize(header->SomaCompartmentIDsSize);
    memcpy(SomaCompartmentIDs.data(), ADD_BYTES_TO_POINTER(header, header->SomaCompartmentIDsOffset), header->DendriteCompartmentIDsOffset - header->SomaCompartmentIDsOffset);

    DendriteCompartmentIDs.resize(header->DendriteCompartmentIDsSize);
    memcpy(DendriteCompartmentIDs.data(), ADD_BYTES_TO_POINTER(header, header->DendriteCompartmentIDsOffset), header->AxonCompartmentIDsOffset - header->DendriteCompartmentIDsOffset);

    AxonCompartmentIDs.resize(header->AxonCompartmentIDsSize);
    memcpy(AxonCompartmentIDs.data(), ADD_BYTES_TO_POINTER(header, header->AxonCompartmentIDsOffset), header->FlatBufSize - header->AxonCompartmentIDsOffset);

    return true;
}

std::string LIFCNeuronBase::str() const {
    std::stringstream ss;
    ss << "ID: " << ID;
    ss << "\nRestingPotential_mV: " << RestingPotential_mV;
    ss << "\nResetPotential_mV: " << ResetPotential_mV;
    ss << "\nSpikeThreshold_mV: " << SpikeThreshold_mV;
    ss << "\nMembraneResistance_MOhm: " << MembraneResistance_MOhm;
    ss << "\nMembraneCapacitance_pF: " << MembraneCapacitance_pF;
    ss << "\nRefractoryPeriod_ms: " << RefractoryPeriod_ms;
    ss << "\nSpikeDepolarization_mV: " << SpikeDepolarization_mV;
    ss << "\nUpdateMethod: " << UpdateMethod;
    ss << "\nResetMethod: " << ResetMethod;
    ss << "\nAfterHyperpolarizationReversalPotential_mV: " << AfterHyperpolarizationReversalPotential_mV;
    ss << "\nFastAfterHyperpolarizationRise_ms: " << FastAfterHyperpolarizationRise_ms;
    ss << "\nFastAfterHyperpolarizationDecay_ms: " << FastAfterHyperpolarizationDecay_ms;
    ss << "\nFastAfterHyperpolarizationPeakConductance_nS: " << FastAfterHyperpolarizationPeakConductance_nS;
    ss << "\nFastAfterHyperpolarizationMaxPeakConductance_nS: " << FastAfterHyperpolarizationMaxPeakConductance_nS;
    ss << "\nFastAfterHyperpolarizationHalfActConstant: " << FastAfterHyperpolarizationHalfActConstant;
    ss << "\nSlowAfterHyperpolarizationRise_ms: " << SlowAfterHyperpolarizationRise_ms;
    ss << "\nSlowAfterHyperpolarizationDecay_ms: " << SlowAfterHyperpolarizationDecay_ms;
    ss << "\nSlowAfterHyperpolarizationPeakConductance_nS: " << SlowAfterHyperpolarizationPeakConductance_nS;
    ss << "\nSlowAfterHyperpolarizationMaxPeakConductance_nS: " << SlowAfterHyperpolarizationMaxPeakConductance_nS;
    ss << "\nSlowAfterHyperpolarizationHalfActConstant: " << SlowAfterHyperpolarizationHalfActConstant;
    ss << "\nAfterHyperpolarizationSaturationModel: " << AfterHyperpolarizationSaturationModel;
    ss << "\nFatigueThreshold: " << FatigueThreshold;
    ss << "\nFatigueRecoveryTime_ms: " << FatigueRecoveryTime_ms;
    ss << "\nAfterDepolarizationReversalPotential_mV: " << AfterDepolarizationReversalPotential_mV;
    ss << "\nAfterDepolarizationRise_ms: " << AfterDepolarizationRise_ms;
    ss << "\nAfterDepolarizationDecay_ms: " << AfterDepolarizationDecay_ms;
    ss << "\nAfterDepolarizationPeakConductance_nS: " << AfterDepolarizationPeakConductance_nS;
    ss << "\nAfterDepolarizationSaturationMultiplier: " << AfterDepolarizationSaturationMultiplier;
    ss << "\nAfterDepolarizationRecoveryTime_ms: " << AfterDepolarizationRecoveryTime_ms;
    ss << "\nAfterDepolarizationDepletion: " << AfterDepolarizationDepletion;
    ss << "\nAfterDepolarizationSaturationModel: " << AfterDepolarizationSaturationModel;
    ss << "\nAdaptiveThresholdDiffPerSpike: " << AdaptiveThresholdDiffPerSpike;
    ss << "\nAdaptiveTresholdRecoveryTime_ms: " << AdaptiveTresholdRecoveryTime_ms;
    ss << "\nAdaptiveThresholdDiffPotential_mV: " << AdaptiveThresholdDiffPotential_mV;
    ss << "\nAdaptiveThresholdFloor_mV: " << AdaptiveThresholdFloor_mV;
    ss << "\nAdaptiveThresholdFloorDeltaPerSpike_mV: " << AdaptiveThresholdFloorDeltaPerSpike_mV;
    ss << "\nAdaptiveThresholdFloorRecoveryTime_ms: " << AdaptiveThresholdFloorRecoveryTime_ms << '\n';
    return ss.str();
}


std::string LIFCNeuronStructFlatHeader::str() const {
    std::stringstream ss;
    ss << "FlatBufSize: " << FlatBufSize;
    ss << "\nNameSize: " << NameSize;
    ss << "\nNameOffset: " << NameOffset;
    ss << "\nSomaCompartmentIDsSize: " << SomaCompartmentIDsSize;
    ss << "\nSomaCompartmentIDsOffset: " << SomaCompartmentIDsOffset;
    ss << "\nDendriteCompartmentIDsSize: " << DendriteCompartmentIDsSize;
    ss << "\nDendriteCompartmentIDsOffset: " << DendriteCompartmentIDsOffset;
    ss << "\nAxonCompartmentIDsSize: " << AxonCompartmentIDsSize;
    ss << "\nAxonCompartmentIDsOffset: " << AxonCompartmentIDsOffset << '\n';
    ss << Base.str();
    return ss.str();
}

std::string LIFCNeuronStructFlatHeader::name_str() const {
    std::stringstream ss;
    ss << "Name: " << (const char*) ADD_BYTES_TO_POINTER(this, NameOffset) << '\n';
    return ss.str();
}

std::string LIFCNeuronStructFlatHeader::scid_str() const {
    std::stringstream ss;
    int* scidptr = (int*) ADD_BYTES_TO_POINTER(this, SomaCompartmentIDsOffset);
    //ss << "flat header address = " << uint64_t(this) << '\n';
    //ss << "scidptr = " << uint64_t(scidptr) << '\n';
    ss << "SomaCompartmentIDs: ";
    for (size_t idx = 0; idx < SomaCompartmentIDsSize; idx++) {
        ss << scidptr[idx] << ' ';
    }
    ss << '\n';
    return ss.str();
}

std::string LIFCNeuronStructFlatHeader::dcid_str() const {
    std::stringstream ss;
    int* dcidptr = (int*) ADD_BYTES_TO_POINTER(this, DendriteCompartmentIDsOffset);
    ss << "DendriteCompartmentIDs: ";
    for (size_t idx = 0; idx < DendriteCompartmentIDsSize; idx++) {
        ss << dcidptr[idx] << ' ';
    }
    ss << '\n';
    return ss.str();
}

std::string LIFCNeuronStructFlatHeader::acid_str() const {
    std::stringstream ss;
    int* acidptr = (int*) ADD_BYTES_TO_POINTER(this, AxonCompartmentIDsOffset);
    ss << "AxonCompartmentIDs: ";
    for (size_t idx = 0; idx < AxonCompartmentIDsSize; idx++) {
        ss << acidptr[idx] << ' ';
    }
    ss << '\n';
    return ss.str();
}

/**
 * Return a flattened struct describing name and compartment indices of
 * the LIFCNeuron. This is useful for storage, among other things.
 */
std::unique_ptr<uint8_t[]> LIFCNeuronStruct::GetFlat() const {
    LIFCNeuronStructFlatHeader header;

    header.Base = *this;
    header.FlatBufSize = sizeof(header);

    header.NameSize = Name.size()+1; // for c_str()
    header.NameOffset = sizeof(header);
    size_t NameSizeOf = sizeof(decltype(Name)::value_type)*header.NameSize;
    header.FlatBufSize += NameSizeOf;

    header.SomaCompartmentIDsSize = SomaCompartmentIDs.size();
    header.SomaCompartmentIDsOffset = header.NameOffset + NameSizeOf;
    size_t SomaCompartmentIDsSizeOf = sizeof(decltype(SomaCompartmentIDs)::value_type)*header.SomaCompartmentIDsSize;
    header.FlatBufSize += SomaCompartmentIDsSizeOf;

    header.DendriteCompartmentIDsSize = DendriteCompartmentIDs.size();
    header.DendriteCompartmentIDsOffset = header.SomaCompartmentIDsOffset + SomaCompartmentIDsSizeOf;
    size_t DendriteCompartmentIDsSizeOf = sizeof(decltype(DendriteCompartmentIDs)::value_type)*header.DendriteCompartmentIDsSize;
    header.FlatBufSize += DendriteCompartmentIDsSizeOf;

    header.AxonCompartmentIDsSize = AxonCompartmentIDs.size();
    header.AxonCompartmentIDsOffset = header.DendriteCompartmentIDsOffset + DendriteCompartmentIDsSizeOf;
    size_t AxonCompartmentIDsSizeOf = sizeof(decltype(AxonCompartmentIDs)::value_type)*header.AxonCompartmentIDsSize;
    header.FlatBufSize += AxonCompartmentIDsSizeOf;
    

    std::unique_ptr<uint8_t[]> flatbuf = std::make_unique<uint8_t[]>(header.FlatBufSize);

    memcpy(flatbuf.get(), &header, sizeof(header));
    memcpy(flatbuf.get()+header.NameOffset, Name.c_str(), header.NameSize);
    memcpy(flatbuf.get()+header.SomaCompartmentIDsOffset, SomaCompartmentIDs.data(), SomaCompartmentIDsSizeOf);
    memcpy(flatbuf.get()+header.DendriteCompartmentIDsOffset, DendriteCompartmentIDs.data(), DendriteCompartmentIDsSizeOf);
    memcpy(flatbuf.get()+header.AxonCompartmentIDsOffset, AxonCompartmentIDs.data(), AxonCompartmentIDsSizeOf);

    return flatbuf;
}

/**
 * (Re)instantiate this object from a flat representation, e.g. from
 * storage.
 */
bool LIFCNeuronStruct::FromFlat(LIFCNeuronStructFlatHeader* header) {
    LIFCNeuronBase::operator=(header->Base);

    Name = (const char*) ADD_BYTES_TO_POINTER(header, header->NameOffset);

    SomaCompartmentIDs.resize(header->SomaCompartmentIDsSize);
    memcpy(SomaCompartmentIDs.data(), ADD_BYTES_TO_POINTER(header, header->SomaCompartmentIDsOffset), header->DendriteCompartmentIDsOffset - header->SomaCompartmentIDsOffset);

    DendriteCompartmentIDs.resize(header->DendriteCompartmentIDsSize);
    memcpy(DendriteCompartmentIDs.data(), ADD_BYTES_TO_POINTER(header, header->DendriteCompartmentIDsOffset), header->AxonCompartmentIDsOffset - header->DendriteCompartmentIDsOffset);

    AxonCompartmentIDs.resize(header->AxonCompartmentIDsSize);
    memcpy(AxonCompartmentIDs.data(), ADD_BYTES_TO_POINTER(header, header->AxonCompartmentIDsOffset), header->FlatBufSize - header->AxonCompartmentIDsOffset);

    return true;
}


}; // namespace CoreStructs
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
