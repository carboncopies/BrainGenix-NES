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
    

    # # Create Sphere
    SphereCfg = NES.Shapes.Sphere.Configuration()
    SphereCfg.Name = "My Sphere"
    SphereCfg.Radius_um = 10.
    SphereCfg.Center_um = [2, 11, 11]
    MySphere = MySim.AddSphere(SphereCfg)

    # Create Compartments
    Cfg = NES.Models.Compartments.BS.Configuration()
    Cfg.Name = "My Compartment 1"
    Cfg.SpikeThreshold_mV = 0.0
    Cfg.DecayTime_ms = 0.0
    Cfg.MembranePotential_mV = 0.0
    Cfg.RestingPotential_mV = 0.0
    Cfg.AfterHyperpolarizationAmplitude_mV = 0.0
    Cfg.Shape = MySphere
    MySphereCompartment = MySim.AddBSCompartment(Cfg)



    # Create Box
    BoxCfg = NES.Shapes.Box.Configuration()
    BoxCfg.Name = "My Box"
    BoxCfg.CenterPosition_um = [13, 11, -1]
    BoxCfg.Dimensions_um = [20, 20, 8]
    BoxCfg.Rotation_rad = [0, 0, 0.785398]
    MyBox = MySim.AddBox(BoxCfg)
    
    Cfg = NES.Models.Compartments.BS.Configuration()
    Cfg.Name = "My Compartment 2"
    Cfg.SpikeThreshold_mV = 0.0
    Cfg.DecayTime_ms = 0.0
    Cfg.MembranePotential_mV = 0.0
    Cfg.RestingPotential_mV = 0.0
    Cfg.AfterHyperpolarizationAmplitude_mV = 0.0
    Cfg.Shape = MyBox
    MyBoxCompartment = MySim.AddBSCompartment(Cfg)


    # Setup VSDA Renderer
    EMConfig = NES.VSDA.EM.Configuration()
    EMConfig.PixelResolution_nm = 0.2
    EMConfig.ImageWidth_px = 512
    EMConfig.ImageHeight_px = 512
    EMConfig.SliceThickness_nm = 100
    EMConfig.ScanRegionOverlap_percent = 0
    EMConfig.MicroscopeFOV_deg = 50
    EMConfig.NumPixelsPerVoxel_px = 4
    VSDAEMInstance = MySim.AddVSDAEM(EMConfig)

    VSDAEMInstance.DefineScanRegion([-11,-11,-1], [25,25,8])
    
    VSDAEMInstance.QueueRenderOperation()

    VSDAEMInstance.WaitForRender()
    VSDAEMInstance.SaveImageStack(".Test1")


def GetTime():
    Start = time.time()

    main()

    End = time.time()
    print(f"Benchmark - Renderer - 1: {End - Start}s")

    return End - Start


if __name__ == "__main__":

    print("----------------------------")
    Start = time.time()

    main()

    End = time.time()
    print(f"Benchmark - Renderer - 1: {End - Start}s")
    print("----------------------------")