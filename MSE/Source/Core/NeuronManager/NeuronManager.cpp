 
#include <list>
#include "Core/NeuronManager/NeuronManager.h"

int RandomNumber(int Maxnum) {
    return rand() % Maxnum;
}

void NeuronManager::CreateNeurons() {

    std::list<TestFive> Neurons;

    // gen neurons
    for (int i = 0; i < 12; i++) {

        TestFive Neuron;
        Neuron.X = RandomNumber(255);
        Neuron.Y = RandomNumber(255);
        Neuron.Z = RandomNumber(255);
            

        Neurons.push_back(Neuron);
    }

    // connect neurons
    

    // enter run loop

        // update neurons

        // print out voltages


}