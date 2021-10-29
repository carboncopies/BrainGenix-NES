#include <vector>

struct TestFive {

    // Set Neuron Spacial Positions
    double X;
    double Y;
    double Z;


    // Connection Info
    long NumberOfConnections;
    std::vector<TestFive*> ConnectionIndexList; // suggest renaming to be specific: PreSynConnectionList (to other neurons)


    // Set Config Params
    double MembranePotential = 0;
    double ThresholdPotential = -50;
    long lastSpike_ms = -1;

    // Update Neuron
    void UpdateNeuron(long t) {

        MembranePotential = -70;

        // Get momentary voltage/potential of the postsynaptic response triggered by the most recent spike of connected presynaptic neuron
        for (auto & neuron_ptr: ConnectionIndexList) {
            long spike_t = neuron_ptr->lastSpike_ms;
            if (spike_t>0) {
                
                if (t-spike_t != 0) {
                    double postsyn_v = 1/((t-spike_t));
                    MembranePotential += postsyn_v;
                    
                }
                
            }
        }
        
        // Random Fire
        if (rand() % 100 == 5) {
            lastSpike_ms = t;
            //std::cout << "Random Spike!\n";
        }

        // Check Threshold
        if (MembranePotential > ThresholdPotential) {
            lastSpike_ms = t;
            //std::cout << "Spike!\n";
        }
        

    }


};