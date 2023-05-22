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

        NeuronConfig = NES.Models.Neurons.TestNeuron()
        NeuronConfig.Position = RandXYZ() 

        NeuronConfig.Axons = []



if __name__ == "__main__":
    main()