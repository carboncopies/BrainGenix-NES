# Simple Test Example For BG
import BrainGenix.NES as NES


def main():

    # Start Tests
    print("----------------------------")
    print("Starting BG-NES Simple Test")


    # Create Client Configuration For Local Simulation
    print("Creating Client Configuration For Local Simulation")
    ClientCfg = NES.Client.Configuration()
    ClientCfg.Mode = NES.Client.Modes.Local

    # Create Client Instance
    print("Creating Client Instance")
    ClientInstance = NES.Client.Client(ClientCfg)

    assert(ClientInstance.IsReady())

    
    # Create A New Simulation
    print("Creating Simulation Configuration Event")
    SimulationCfg = NES.Simulation.Configuration()
    SimulationCfg.Name = "TestSimulation"
    SimulationCfg.Description = "Example Description"

    # Create Simulation Instance
    SimulationID = ClientInstance.CreateSimulation(SimulationCfg)
    assert(SimulationID != -1)
    



if __name__ == "__main__":
    main()