 
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

    int NumberNeurons = 500;
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
    //std::map<int, std::vector<long>> ConnectionList;

    for (int i = 0; i < NumberNeurons; i++) {

        std::vector<long> NeuronConnectionMap;
        for (int x = 0; x < NumberSynapses; x++) {
            NeuronConnectionMap.insert(NeuronConnectionMap.end(), RandomNumber(NumberNeurons));
        }


        Neurons[i].ConnectionIndexList = &NeuronConnectionMap;

        //ConnectionList.insert({i, NeuronConnectionMap});
    }

    // std::cout << "Assigning Connections To Neurons\n";

    // // Assign Connections
    // for (int i = 0; i < NumberNeurons; i++) {

        

    // }


    std::cout << "Simulating Neurons\n";

    // Enter Main Loop
    while (true) {

        // Time it
        std::chrono::time_point<std::chrono::system_clock> start, end;
        start = std::chrono::system_clock::now();

        // Update Neurons
        uint64_t TotalVoltage = 0;
        for (int i = 0; i < NumberNeurons; i++) {
            TotalVoltage += Neurons[i].UpdateNeuron();
        }

        // end time
        end = std::chrono::system_clock::now();
    
        std::chrono::duration<double> elapsed_seconds = end - start;
        std::time_t end_time = std::chrono::system_clock::to_time_t(end);


        // print out voltages
        std::cout << elapsed_seconds.count() << "\n";

    }

}