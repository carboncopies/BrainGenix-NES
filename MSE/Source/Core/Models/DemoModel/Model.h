#include <vector>
#include <math.h>

struct TestFive {

    // Set Neuron Spacial Positions
    double X;
    double Y;
    double Z;


    // Connection Info
    long NumberOfConnections;
    std::vector<TestFive*> PreSynConnectionList;


    // Set Config Params
    double MembranePotential = -70;
    double ThresholdPotential = -50;
    double TimeStep;
    double MembranePotentialDecayFactor = -0.7;
    long lastSpike_ms = -1;
    double g_peak = 1.0; // should be definable per synapse
    double tau_rise = 10.0; // should be definable per synapse 
    double tau_decay = 50.0; // should be definable per synapse
    double absolute_refractory_period_ms = 2.0;
    double a_AHP = -90;
    double V_AHPdiffperms = 0.5;

    // Update Neuron
    void UpdateNeuron(long t) {


        StepMembranePotentialDecay();

        AfterHyperPolarization();

        // Get momentary voltage/potential of the postsynaptic response triggered by the most recent spike of connected presynaptic neuron
        for (auto & neuron_ptr: PreSynConnectionList) {
            long spike_t = neuron_ptr->lastSpike_ms;
            if (spike_t>0) {
                
                if (t-spike_t != 0) {
                    MembranePotential += ReceptorCurrent(t-spike_t);
                    
                }
                
            }
        }
        
        // Random Fire
        if (rand() % 100 == 5) {
            lastSpike_ms = t;
            //std::cout << "Random Spike!\n";
        }

        // Check Threshold
        if ((MembranePotential > ThresholdPotential) && ((t-lastSpike_ms > absolute_refractory_period_ms))) {
            lastSpike_ms = t;
            MembranePotential = a_AHP;
            //std::cout << "Spike!\n";
        }
        

    }

    double ReceptorCurrent(long t_diff) {
        return g_peak * (-exp(-t_diff/tau_rise) + exp(-t_diff/tau_decay));
    }

    void StepMembranePotentialDecay() {
        if (MembranePotential > -70) {
            MembranePotential -= MembranePotentialDecayFactor;
        }
    }

    void AfterHyperPolarization() {
        if (MembranePotential < -70) {
            MembranePotential += V_AHPdiffperms;
        }
    }


};