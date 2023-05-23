# Simple Test Example For BG
import BrainGenix.NES as NES


def main():

    # Start Tests
    print("Starting BG-NES Simple Test")


    # Create Client Configuration For Local Simulation
    ClientCfg = NES.Client.Configuration()
    ClientCfg.Mode = NES.Client.Modes.Local

    # Create client Instance
    ClientInstance = NES.Client.Client(ClientCfg)

    print(ClientInstance.IsReady())



if __name__ == "__main__":
    main()