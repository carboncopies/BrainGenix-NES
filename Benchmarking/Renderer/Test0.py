#!/bin/python3

# Simple Test Example For BG
import time

import BrainGenix.NES as NES



def main():

    # Create Client Configuration For Local Simulation
    ClientCfg = NES.Client.Configuration()
    ClientCfg.Mode = NES.Client.Modes.Remote
    ClientCfg.Host = "localhost"
    ClientCfg.Port = 8000
    ClientCfg.UseHTTPS = False
    ClientCfg.AuthenticationMethod = NES.Client.Authentication.Password
    ClientCfg.Username = "Admonishing"
    ClientCfg.Password = "Instruction"

    # Create Client Instance
    ClientInstance = NES.Client.Client(ClientCfg)

    assert(ClientInstance.IsReady())

    
    # Create A New Simulation
    SimulationCfg = NES.Simulation.Configuration()
    SimulationCfg.Name = "My First Simulation"
    MySim = ClientInstance.CreateSimulation(SimulationCfg)
    


    # Setup VSDA Renderer
    EMConfig = NES.VSDA.EM.Configuration()
    EMConfig.PixelResolution_nm = 0.2
    EMConfig.ImageWidth_px = 1024
    EMConfig.ImageHeight_px = 1024
    EMConfig.SliceThickness_nm = 100
    EMConfig.ScanRegionOverlap_percent = 0
    EMConfig.MicroscopeFOV_deg = 50
    EMConfig.NumPixelsPerVoxel_px = 2
    VSDAEMInstance = MySim.AddVSDAEM(EMConfig)

    VSDAEMInstance.DefineScanRegion([0,0,0], [90,60,20])
    
    VSDAEMInstance.QueueRenderOperation()

    VSDAEMInstance.WaitForRender()
    VSDAEMInstance.ThreadedSaveImageStack(".Test0", 5)


def GetTime():
    Start = time.time()

    main()

    End = time.time()
    print(f"Benchmark - Renderer - 0: {End - Start}s")

    return End - Start


if __name__ == "__main__":

    print("----------------------------")
    Start = time.time()

    main()

    End = time.time()
    print(f"Benchmark - Renderer - 0: {End - Start}s")
    print("----------------------------")