#include <vector>

struct TestFive {

    // Set Neuron Spacial Positions
    double X;
    double Y;
    double Z;


    // Connection Info
    long NumberOfConnections;
    std::vector<long> *ConnectionIndexList;


    // Set Config Params
    double MembranePotential = 0;

    std::vector<double> Voltages = {-70.0, 40.0, -90.0};
    int VoltageIndex = 0;

    // Update Neuron
    int UpdateNeuron() {

        // Get Voltages Of Connected Neurons

        // Sum Them
        
        // Check Threshold
        
        // Fire
        std::cout << ConnectionIndexList->size()<<"\n";
        // long double MembranePotential = Voltages[VoltageIndex];
        // for (int i = 0; i < ConnectionIndexList->size(); i++) {
        // }




        MembranePotential;

        return 7;

    }


};