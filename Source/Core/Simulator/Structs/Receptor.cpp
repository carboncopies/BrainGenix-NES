#include <Simulator/Structs/Receptor.h>

namespace BG {
namespace NES {
namespace Simulator {
namespace Connections {

// Helper function to compute normalization factor for LIFC PSPs, fAHP, sAHP, ADP.
float compute_normalization(float tau_rise, float tau_decay) {
    if (tau_rise == tau_decay) {
        throw std::invalid_argument("tau_rise must be different from tau_decay for normalization.");
    }
    float t_peak = (tau_rise * tau_decay) / (tau_decay - tau_rise) * log(tau_decay / tau_rise);
    float norm = exp(-t_peak / tau_decay) - exp(-t_peak / tau_rise);
    return norm;
}

// Function to compute normalized synaptic conductance for LIFC PSPs, fAHP, sAHP, ADP.
float g_norm(float t, const std::vector<float>& spike_times, float tau_rise, float tau_decay, 
             float norm, float onset_delay, float spike_dt_delta, float history_delta) {
    t -= onset_delay;
    history_delta *= norm;
    float gnorm = 0;
    
    // Iterate in reverse to process most recent spikes first
    for (auto it = spike_times.rbegin(); it != spike_times.rend(); ++it) {
        float spike_dt = t - *it;
        if (spike_dt >= 0) {
            float g_norm_contribution = exp(-spike_dt / tau_decay) - exp(-spike_dt / tau_rise);
            if (spike_dt > spike_dt_delta && g_norm_contribution < history_delta) {
                return gnorm / norm;
            }
            gnorm += g_norm_contribution;
        }
    }
    return gnorm / norm;
}

std::string ReceptorBase::str() const {
    std::stringstream ss;
    ss << "ID: " << ID;
    ss << "\nShapeID: " << ShapeID;
    ss << "\nSourceCompartmentID: " << SourceCompartmentID;
    ss << "\nDestinationCompartmentID: " << DestinationCompartmentID;
    ss << "\nConductance_nS: " << Conductance_nS;
    ss << "\nTimeConstantRise_ms: " << TimeConstantRise_ms;
    ss << "\nTimeConstantDecay_ms: " << TimeConstantDecay_ms;
    ss << "\nNeurotransmitter: " << Neurotransmitter;
    return ss.str();
}

void ReceptorBase::safeset_Neurotransmitter(const char* s) {
    strncpy(Neurotransmitter, s, NeurotransmitterLEN-1);
    Neurotransmitter[NeurotransmitterLEN-1] = '\0';
}

std::string LIFCReceptorBase::str() const {
    std::stringstream ss;
    ss << "ID: " << ID;
    ss << "\nShapeID: " << ShapeID;
    ss << "\nSourceCompartmentID: " << SourceCompartmentID;
    ss << "\nDestinationCompartmentID: " << DestinationCompartmentID;
    ss << "\nReversalPotential_mV: " << ReversalPotential_mV; 
    ss << "\nPSPRise_ms: " << PSPRise_ms;
    ss << "\nPSPDecay_ms: " << PSPDecay_ms;
    ss << "\nPeakConductance_nS: " << PeakConductance_nS;
    ss << "\nWeight: " << Weight;
    ss << "\nOnsetDelay_ms: " << OnsetDelay_ms;
    ss << "\nvoltage_gated: " << voltage_gated;
    ss << "\nSTDP_Method: " << STDP_Method;
    ss << "\nSTDP_A_pos: " << STDP_A_pos;
    ss << "\nSTDP_A_neg: " << STDP_A_neg;
    ss << "\nSTDP_Tau_pos: " << STDP_Tau_pos;
    ss << "\nSTDP_Tau_neg: " << STDP_Tau_neg;
    ss << "\nSTDP_Shift: " << STDP_Shift;
    ss << "\nNeurotransmitter: " << Neurotransmitter;
    return ss.str();
}

}; // namespace Connections
}; // namespace Simulator
}; // namespace NES
}; // namespace BG
