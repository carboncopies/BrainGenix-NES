# Test Driver

import random

import NES.Client
import NES.Models
import NES.Simulation


def RandXYZ():
    X = random.random()*50
    Y = random.random()*50
    Z = random.random()*50
    return (X,Y,Z)


def BuildSynapse(_Simulation:NES.Simulation, _NumReceptors:int):

    # Create ion channels for each synapse
    ReceptorIDs = []
    for i in range(_NumReceptors):
        Receptor = NES.Models.Receptors.TestReceptor()

        # Configure the properties here

        ReceptorID = _Simulation.AddReceptor(Receptor)
        if (ReceptorID == -1) {
            print("Error creating receptor!")
            exit()
        }
        ReceptorIDs.append(ReceptorID)


    Synapse = NES.Models.Synapses.TestSynapse()

    # Configure other properties here

    Synapse.Receptors = ReceptorIDs

    SynapseID = _Simulation.AddSynapse(Synapse)
    if (SynapseID == -1) {
        print("Error creating synapse!")
        exit()
    }

    return SynapseID


def BuildEndCompartment(_Simulation:NES.Simulation, _NumSynapses:int, _NumReceptors:int):

    



def main():

    print("Starting NES Frontend Tests")

    # Setup Client (use local config)
    ClientConfiguration = NES.Client.Configuration.Configuration()
    ClientConfiguration.Mode = NES.Client.Modes.Local

    ClientInstance = NES.Client.Client.Client(ClientConfiguration)

    if not ClientInstance.IsConnected():
        print("Cannot Connect To Client")


    # Create Simulation
    MySim = NES.Simulation.Simulation()
    MySim.Name = "Test Sim"
    
    SimID = ClientInstance.CreateSimulation(MySim)
    if (SimID == -1):
        print("Failed To Create Simulation!")
        exit()
    ClientInstance.BindSimulation(SimID)

    
    # Now Add Neurons
    NumNeurons = 100
    NumAxonCompartments = 100 # we're just making a line here
    NumDendriteCompartments = 100 # again just making a line
    NumSynapsesPerEndCompartment = 10 # just some simple synapses
    NumReceptersPerSynapse = 5 # ramdom num
    
    NeuronIDs = []
    for i in range(NumNeurons):

        # We represent neurons as follows:
        #               0 0  <- Synapese+Receptors (too small to show)              
        #               | |                
        #               ===  <- Compartments               
        #                |                 
        #              +---+                   
        #              |   | <- Soma                 
        #              +---+                   
        #                |                 
        #         ============== <- More Compartments          
        #         0     |  0   |              
        #               0      0  <- More Synapses+Receptors          
        #
        # So, this means that we need to make the following
        # Create and configure receptor structs
        # Then add these to the simulation, and note the IDs returned
        # Now, create the synapses and connect the receptors by id to them
        # Then add these to the sim and note the ID
        # Then create the compartments 
        # (this may be a multi-step process since compartments can link to eachother)
        # Finally, link the root axon/dendrite compartments to the neuron struct

        # Build Synapse


        NeuronConfig = NES.Models.Neurons.TestNeuron()
        NeuronConfig.Position = RandXYZ() 

        NeuronConfig.Axons = []



if __name__ == "__main__":
    main()