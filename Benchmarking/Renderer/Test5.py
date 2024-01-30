#!/bin/python3

# Simple Test Example For BG
import argparse

import BrainGenix.NES as NES
import BrainGenix
import numpy as np 
import time
import random

random.seed(42)

def main():

    # Handle Arguments for Host, Port, etc
    Parser = argparse.ArgumentParser(description="BrainGenix-API Simple Python Test Script")
    Parser.add_argument("-Host", default="localhost", type=str, help="Host to connect to")
    Parser.add_argument("-Port", default=8000, type=int, help="Port number to connect to")
    Parser.add_argument("-UseHTTPS", default=False, type=bool, help="Enable or disable HTTPS")
    Args = Parser.parse_args()


    Size = 350
    NumShapes = 100
    NumIters = 10

    # Start Tests
    print("----------------------------")
    print("Starting BG-NES Bulk Create Test")

    # Create Client Configuration For Local Simulation
    print(" -- Creating Client Configuration For Local Simulation")
    ClientCfg = NES.Client.Configuration()
    ClientCfg.Mode = NES.Client.Modes.Remote
    ClientCfg.Host = Args.Host
    ClientCfg.Port = Args.Port
    ClientCfg.UseHTTPS = Args.UseHTTPS
    ClientCfg.AuthenticationMethod = NES.Client.Authentication.Password
    ClientCfg.Username = "Admonishing"
    ClientCfg.Password = "Instruction"

    # Create Client Instance
    print(" -- Creating Client Instance")
    ClientInstance = NES.Client.Client(ClientCfg)

    assert(ClientInstance.IsReady())

    # Create A New Simulation
    print(" -- Creating Simulation")
    SimulationCfg = NES.Simulation.Configuration()
    SimulationCfg.Name = "My First Simulation"
    MySim = ClientInstance.CreateSimulation(SimulationCfg)



    for x in range(NumIters):

        print(" -- Creating Spheres")
        SphereCfgs:list = []
        for i in range(NumShapes):
            SphereCfg = NES.Shapes.Sphere.Configuration()
            SphereCfg.Name = "Randomly Positioned Ball"
            SphereCfg.Radius_um = random.randint(15,30)/10
            NeuronLocation:list = [random.randint(1, Size) for _ in range(3)]
            SphereCfg.Center_um = NeuronLocation
            SphereCfgs.append(SphereCfg)
        Spheres = MySim.AddSpheres(SphereCfgs)
        print(" -- Creating BS Sphere Compartments")
        Compartments:list = []
        for Sphere in Spheres:
            Cfg = NES.Models.Compartments.BS.Configuration()
            Cfg.Name = "My Random Compartment"
            Cfg.SpikeThreshold_mV = 0.0
            Cfg.DecayTime_ms = 0.0
            Cfg.MembranePotential_mV = 0.0
            Cfg.RestingPotential_mV = 0.0
            Cfg.AfterHyperpolarizationAmplitude_mV = 0.0
            Cfg.Shape = Sphere
            Compartments.append(Cfg)
        MySim.AddBSCompartments(Compartments)



        print(" -- Creating Cylinders")
        CylinderCfgs:list = []
        for i in range(NumShapes):
            CylinderCfg = NES.Shapes.Cylinder.Configuration()
            CylinderCfg.Name = "Connection"
            CylinderCfg.Point1Position_um = [random.randint(1, Size) for _ in range(3)]
            CylinderCfg.Point2Position_um = [random.randint(1, Size) for _ in range(3)]
            CylinderCfg.Point1Radius_um = random.randint(2,12)/10
            CylinderCfg.Point2Radius_um = random.randint(2,12)/10
            CylinderCfgs.append(CylinderCfg)
        Cylinders = MySim.AddCylinders(CylinderCfgs)
        print(" -- Creating BS Cylinder Compartments")
        Compartments:list = []
        for Cylinder in Cylinders:

            #Create Compartments
            Cfg = NES.Models.Compartments.BS.Configuration()
            Cfg.Name = "My Random Compartment"
            Cfg.SpikeThreshold_mV = 0.0
            Cfg.DecayTime_ms = 0.0
            Cfg.MembranePotential_mV = 0.0
            Cfg.RestingPotential_mV = 0.0
            Cfg.AfterHyperpolarizationAmplitude_mV = 0.0
            Cfg.Shape = Cylinder
            Compartments.append(Cfg)
        MySim.AddBSCompartments(Compartments)



        print(" -- Creating Boxes")
        Cfgs:list = []
        for i in range(NumShapes):
            Cfg = NES.Shapes.Box.Configuration()
            Cfg.Name = "Connection"
            Cfg.CenterPosition_um = [random.randint(1, Size) for _ in range(3)]
            Cfg.Dimensions_um = [random.randint(1, 10) for _ in range(3)]
            Cfg.Rotation_rad = [random.randint(1, 720)/360 for _ in range(3)]
            Cfgs.append(Cfg)
        Boxes = MySim.AddBoxes(Cfgs)
        print(" -- Creating BS Box Compartments")
        Compartments:list = []
        for Box in Boxes:

            #Create Compartments
            Cfg = NES.Models.Compartments.BS.Configuration()
            Cfg.Name = "My Random Compartment"
            Cfg.SpikeThreshold_mV = 0.0
            Cfg.DecayTime_ms = 0.0
            Cfg.MembranePotential_mV = 0.0
            Cfg.RestingPotential_mV = 0.0
            Cfg.AfterHyperpolarizationAmplitude_mV = 0.0
            Cfg.Shape = Box
            Compartments.append(Cfg)
        MySim.AddBSCompartments(Compartments)



    # Setup VSDA Renderer
    print(" -- Setting Up VSDA EM Renderer")
    EMConfig = NES.VSDA.EM.Configuration()
    # actually microns
    EMConfig.PixelResolution_nm = 0.1
    EMConfig.ImageWidth_px = 1024
    EMConfig.ImageHeight_px = 1024
    EMConfig.SliceThickness_nm = 100
    EMConfig.ScanRegionOverlap_percent = 0
    EMConfig.MicroscopeFOV_deg = 50
    EMConfig.NumPixelsPerVoxel_px = 1
    VSDAEMInstance = MySim.AddVSDAEM(EMConfig)

    VSDAEMInstance.DefineScanRegion([0, 0, 0], [Size, Size, 30])
    VSDAEMInstance.QueueRenderOperation()
    VSDAEMInstance.WaitForRender()
    VSDAEMInstance.SaveImageStack(".BulkImages")


    # print(" -- Reconstructing Image Stack")
    # BrainGenix.Tools.StackStitcher.StackStitcher(".BulkImages", ".BulkImagesR")


def GetTime():
    Start = time.time()

    main()

    End = time.time()
    print(f"Benchmark - Renderer - 5: {End - Start}s")
    return End - Start

if __name__ == "__main__":

    print("----------------------------")
    Start = time.time()

    main()

    End = time.time()
    print(f"Benchmark - Renderer - 5: {End - Start}s")
    print("----------------------------")