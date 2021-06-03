###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

'''
Name: System telemetry management api.
Description: This interface connects the system telemetry subsystem to the management api.
Date-Created: 2021-06-03
''' 

class SystemTelemetryAPIInterface():
    
    def __init__(self, SystemTelemetryInstance): # Get Some Params Here #

        # Create Local Pointer #
        self.Systel = SystemTelemetryInstance

    def ListAllNodes(*Kwargs): # Gets the system telemetry from all nodes, and accepts parameters to reduce this #

        # Get List Of Nodes #
        self.Nodes = self.Systel

        