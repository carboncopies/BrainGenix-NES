//=================================================================//
// This file is part of the BrainGenix-NES Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides the Neuron abstract struct.
    Additional Notes: None
    Date Created: 2024-01-13
*/

#include <iostream>

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

ReceptorData::ReceptorData(int _RID, Connections::Receptor * _RPtr, Neuron * _SNPtr, Neuron * _DNPtr):
        ReceptorID(_RID), SrcNeuronID(_SNPtr->ID), DstNeuronID(_DNPtr->ID), ReceptorPtr(_RPtr), SrcNeuronPtr(_SNPtr), DstNeuronPtr(_DNPtr) {
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

void Neuron::InputReceptorAdded(ReceptorData RData) {
    WARNWRONGOOPLEVEL();
}

void Neuron::OutputTransmitterAdded(ReceptorData RData) {
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

}; // namespace CoreStructs
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
