#!/bin/python3

# Simple Test Example For BG
import time



import BrainGenix.NES as NES
import BrainGenix
import numpy as np 
import random

random.seed(42)



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

    NeuronLocations:list = []
    for i in range(500):
        # Create Sphere
        SphereCfg = NES.Shapes.Sphere.Configuration()
        SphereCfg.Name = "Randomly Positioned Ball"
        SphereCfg.Radius_um = random.randint(15,30)/10
        NeuronLocation:list = [random.randint(1, 350) for _ in range(3)]
        SphereCfg.Center_um = NeuronLocation
        NeuronLocations.append(NeuronLocation)

        MySphere = MySim.AddSphere(SphereCfg)

        #Create Compartments
        Cfg = NES.Models.Compartments.BS.Configuration()
        Cfg.Name = "My Random Compartment"
        Cfg.SpikeThreshold_mV = 0.0
        Cfg.DecayTime_ms = 0.0
        Cfg.MembranePotential_mV = 0.0
        Cfg.RestingPotential_mV = 0.0
        Cfg.AfterHyperpolarizationAmplitude_mV = 0.0
        Cfg.Shape = MySphere
        MySphereCompartment = MySim.AddBSCompartment(Cfg)


    for i in range(1000):

        Position1:list = NeuronLocations[random.randint(0, len(NeuronLocations)-1)]
        Position2:list = NeuronLocations[random.randint(0, len(NeuronLocations)-1)]

        # Create Cylinder
        CylinderCfg = NES.Shapes.Cylinder.Configuration()
        CylinderCfg.Name = "Connection"
        CylinderCfg.Point1Position_um = Position1
        CylinderCfg.Point2Position_um = Position2
        CylinderCfg.Point1Radius_um = random.randint(2,12)/10
        CylinderCfg.Point2Radius_um = random.randint(2,12)/10
        MyCylinder = MySim.AddCylinder(CylinderCfg)

        Cfg = NES.Models.Compartments.BS.Configuration()
        Cfg.Name = "My Compartment 5"
        Cfg.SpikeThreshold_mV = 0.0
        Cfg.DecayTime_ms = 0.0
        Cfg.MembranePotential_mV = 0.0
        Cfg.RestingPotential_mV = 0.0
        Cfg.AfterHyperpolarizationAmplitude_mV = 0.0
        Cfg.Shape = MyCylinder
        MyCylinderCompartment = MySim.AddBSCompartment(Cfg)


    # Setup VSDA Renderer
    EMConfig = NES.VSDA.EM.Configuration()
    # actually microns
    EMConfig.PixelResolution_nm = 0.65
    EMConfig.ImageWidth_px = 1500
    EMConfig.ImageHeight_px = 1500
    EMConfig.SliceThickness_nm = 100
    EMConfig.ScanRegionOverlap_percent = 0
    EMConfig.MicroscopeFOV_deg = 50
    EMConfig.NumPixelsPerVoxel_px = 1
    VSDAEMInstance = MySim.AddVSDAEM(EMConfig)

    VSDAEMInstance.DefineScanRegion([0, 0, 0], [350, 350, 350])
    VSDAEMInstance.QueueRenderOperation()
    VSDAEMInstance.WaitForRender()
    VSDAEMInstance.SaveImageStack(".Test6")


    # BrainGenix.Tools.StackStitcher.StackStitcher(".Test6", ".ReconstructedImages6")



def GetTime():
    Start = time.time()

    main()

    End = time.time()
    print(f"Benchmark - Renderer - 6: {End - Start}s")
    return End - Start

if __name__ == "__main__":

    print("----------------------------")
    Start = time.time()

    main()

    End = time.time()
    print(f"Benchmark - Renderer - 6: {End - Start}s")
    print("----------------------------")
