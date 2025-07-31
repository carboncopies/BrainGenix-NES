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

#include <Simulator/Structs/Neuron.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace CoreStructs {

// Use this to warn about accidentally having reached the wrong
// OOP level:
#define WARNWRONGOOPLEVEL() {\
    std::cout << "DEBUG --> Wrong OOP level: " << __func__ << '\n'; std::cout.flush();\
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
    norm = compute_normalization(tau_rise_ms, tau_decay_ms);
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
                     g_norm(t, syn_times, tau_rise_ms, tau_decay_ms, norm, onset_delay_ms));
    } else {
        g_k = std::min(g_peak_sum_nS, weight * g_peak_sum_nS * 
                     g_norm(t, syn_times, tau_rise_ms, tau_decay_ms, norm, onset_delay_ms));
    }
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
