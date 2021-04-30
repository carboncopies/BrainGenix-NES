###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

import copy

'''
Name: ZKManager
Description: This manages Zookeeper Modes.
Date-Created: 2020-01-29
'''

class SystemTelemetryManager(): # Manages the system telemetry leader class #

    def __init__(self, Zookeeper:object, SystemTelemetry:object):

        # Create Local Pointers To These Objects #
        self.Zookeeper = Zookeeper
        self.SystemTelemetry = SystemTelemetry
        self.SystemTelemetryInitialState = copy.copy(SystemTelemetry)

        self.LastCheckedMode = 'Follower' # <-- Default value, don't change this!


    def UpdateSysTel(self): # Updates The System Telemetry Mode #

        if self.Zookeeper.ZookeeperMode != self.LastCheckedMode:
            if self.Zookeeper.ZookeeperMode == 'Leader':
                self.SystemTelemetry.Start()
            else:
                del self.SystemTelemetry
                self.SystemTelemetry = self.SystemTelemetryInitialState

            self.LastCheckedMode = self.Zookeeper.ZookeeperMode