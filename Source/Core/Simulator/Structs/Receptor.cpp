#include <Simulator/Structs/Receptor.h>

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
             float norm, float onset_delay, float spike_dt_delta = 1000, float history_delta = 0.001) {
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
