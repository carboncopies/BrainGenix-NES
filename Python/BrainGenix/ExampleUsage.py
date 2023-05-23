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
        ReceptorCfg = NES.Models.Receptors.TestReceptor()

        # Configure the properties here

        ReceptorID = _Simulation.AddReceptor(ReceptorCfg)
        if (ReceptorID == -1) {
            print("Error creating receptor!")
            exit()
        }
        ReceptorIDs.append(ReceptorID)


    SynapseCfg = NES.Models.Synapses.TestSynapse()

    # Configure other properties here

    SynapseCfg.Receptors = ReceptorIDs

    SynapseID = _Simulation.AddSynapse(SynapseCfg)
    if (SynapseID == -1):
        print("Error creating synapse!")
        exit()


    return SynapseID


def BuildEndCompartment(_Simulation:NES.Simulation, _NumSynapses:int, _NumReceptors:int):

    # Create Synapses For This Compartment
    SynapseIDs = []
    for i in range(_NumSynapses):
        SynapseID = BuildSynapse(_Simulation, _NumReceptors)
        SynapseIDs.append(SynapseID)

    CompartmentCfg = NES.Models.Compartments.TestCompartment()
    
    # Configure other stuff here (like membrane conductance/capacitance, etc.)
    CompartmentCfg.Synapses = SynapseIDs

    CompartmentID = _Simulation.AddCompartment(CompartmentCfg)
    if (CompartmentID == -1):
        print("Error creating end compartment with synapses!")
        exit()
    
    return CompartmentID


def BuildCompartment(_Simulation:NES.Simulation, _Length:int, _NumSynapses:int, _NumReceptors:int):

    # Create Child
    ChildCompartmentID = None
    
    # Check if we're at the end
    if (_Length == 0):
        ChildCompartmentID = BuildEndCompartment(_Simulation, _NumSynapses, _NumReceptors)
    else:
        ChildCompartmentID = BuildCompartment(_Simulation, _Length-1, _NumSynapses, _NumReceptors)

    # Create This Compartment, configure and build
    CompartmentCfg = NES.Models.Compartments.TestCompartment()
    
    # Configure other stuff here (like membrane conductance/capacitance, etc.)
    CompartmentCfg.ChildCompartments = [ChildCompartmentID]

    CompartmentID = _Simulation.AddCompartment(CompartmentCfg)
    if (CompartmentID == -1):
        print("Error creating end compartment with synapses!")
        exit()

    return CompartmentID



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
    SimulationInstance = ClientInstance.BindSimulation(SimID)

    
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

        # Build Axon Compartments, Dendrite Compartments
        AxonCompartmentIDs = [
                            BuildCompartment(SimulationInstance, 
                                            NumAxonCompartments,
                                            NumSynapsesPerEndCompartment,
                                            NumReceptersPerSynapse
                                            )
                            ]
        DendriteCompartmentIDs = [
                            BuildCompartment(SimulationInstance, 
                                            NumDendriteCompartments,
                                            NumSynapsesPerEndCompartment,
                                            NumReceptersPerSynapse
                                            )
                            ]
        

        # Create Neuron
        NeuronCfg = NES.Models.Neurons.TestNeuron()

        # Configure, this will have more stuff in it later...
        NeuronCfg.Position = RandXYZ()
        NeuronCfg.Axons = AxonCompartmentIDs
        NeuronCfg.Dendrites = DendriteCompartmentIDs


        # Create Neuron
        NeuronID = SimulationInstance.AddNeuron(NeuronCfg)
        if (NeuronID == -1):
            print("Error creating Neuron!")
            exit()
        NeuronIDs.append(NeuronID)



if __name__ == "__main__":
    main()