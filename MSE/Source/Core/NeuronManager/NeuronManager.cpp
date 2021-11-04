 
#include <vector>
#include <map>
#include <iostream>
#include <chrono>

#include "Core/NeuronManager/NeuronManager.h"

int RandomNumber(int Maxnum) {
    return rand() % Maxnum;
}

void NeuronManager::CreateNeurons() {

    std::cout << "Creating Neurons\n";

    int NumberNeurons = 5000;
    int NumberSynapses = 1000;

    std::vector<TestFive> Neurons;
    // gen neurons
    for (int i = 0; i < NumberNeurons; i++) {

        TestFive Neuron;
        Neuron.X = RandomNumber(255);
        Neuron.Y = RandomNumber(255);
        Neuron.Z = RandomNumber(255);
            

        Neurons.insert(Neurons.end(), Neuron);
    }

    std::cout << "Connecting Neurons\n";

    // Connect Neurons
    //std::map<int, std::vector<TestFive *>> ConnectionList;

    for (int i = 0; i < NumberNeurons; i++) {

        // Make a list of neuron indices, our target neurons
        //std::vector<long> NeuronConnectionMap;
        for (int x = 0; x < NumberSynapses; x++) {
            //NeuronConnectionMap.emplace_back(RandomNumber(NumberNeurons));
            Neurons[i].PreSynConnectionList.emplace_back(&Neurons[RandomNumber(NumberNeurons)]);
        }

    }



    std::cout << "Simulating Neurons\n";

    // Enter Main Loop
    long t =0;
    while (true) {

        t++;

        // Time it
        std::chrono::time_point<std::chrono::system_clock> start, end;
        start = std::chrono::system_clock::now();

        // Update Neurons
        uint64_t TotalVoltage = 0;
        for (int i = 0; i < NumberNeurons; i++) {
            Neurons[i].UpdateNeuron(t);
            TotalVoltage += Neurons[i].MembranePotential;
        }

        std::cout << TotalVoltage << "\n";

        // end time
        end = std::chrono::system_clock::now();
    
        std::chrono::duration<double> elapsed_seconds = end - start;
        std::time_t end_time = std::chrono::system_clock::to_time_t(end);


        // print out voltages
        //std::cout << elapsed_seconds.count() << "\n";

    }

}