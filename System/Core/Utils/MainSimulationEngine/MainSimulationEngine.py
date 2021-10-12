###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################
 
import brian2

from Core.MainSimulationEngine.Brian2Interface.CPPLoader import LoadCPP

'''
Name: Main Simulation Engine
Description: This file is responsible for creating the Main Simulation Engine Class.
Date-Created: 2021-10-08
'''

# Main Simulation Engine Class
class MainSimulationEngine():

    # Create Initialization Function
    def __init__(self, Logger:object, SystemConfiguration:object):

        # Log System Initialization
        Logger.Log('Initializing Main Simulation Engine', 5)

        # Create Local Pointers
        self.Logger = Logger
        self.SystemConfiguration = SystemConfiguration


        # Load CPP
        self.Logger.Log('Brian2Interface Loading C++ Callback', 5)

        CPPSource, HeaderSource = LoadCPP(
            Logger, 
            'CPPSource/BrianInterface.cpp',
            'CPPSource/BrianInterface.h'
            )


        # Create Callback
        self.Logger.Log('Creating Brian2Interface Callback', 4)

        @brian2.core.functions.implementation('cpp', CPPSource, headers=HeaderSource) ## TODO: Make this make sense, it's not super clear right now how this works
        @brian2.units.fundamentalunits.check_units(x=1, y=1, result=1)
        def BrianInterfaceCallback(x, y):
            
            pass

        self.Logger.Log('Brian2Interface Callback Created', 3)