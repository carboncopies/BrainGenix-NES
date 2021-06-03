###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

import copy

'''
Name: ZKManager
Description: This manages Zookeeper Modes.
Date-Created: 2021-01-29
'''


from Core.Management.Telemetry.SystemTelemetry import Follower
from Core.Management.Telemetry.SystemTelemetry import Leader



class SystemTelemetryManager(): # Manages the system telemetry leader class #

    def __init__(self, Logger, Zookeeper):

        # Create Local Pointers To These Objects #
        self.Logger = Logger
        self.Zookeeper = Zookeeper


    def TransitionLeader(self): # Transitions The System To Follower Mode #

        # Log Instantiation #
        self.Logger.Log('System Telemetry Transition Asserted, Called For Mode "LEADER"')

        # Instantiate #
        self.SysTelFollower = Follower(Logger=self.Logger, Zookeeper=self.Zookeeper)
        self.SysTelLeader = Leader(Logger=self.Logger, Zookeeper=self.Zookeeper)

        # Log Finish Message #
        self.Logger.Log('Initalized System Telemetry Subsystem In Mode [Leader, Follower]')


    def TransitionFollower(self): # Transitions The System To Follower Mode #

        # Log Instantiation #
        self.Logger.Log('System Telemetry Transition Asserted, Called For Mode "FOLLOWER"')

        # Instantiate #
        self.SysTelFollower = Follower(Logger=self.Logger, Zookeeper=self.Zookeeper)
        self.SysTelLeader = None

        # Log Finish Message #
        self.Logger.Log('Initalized System Telemetry Subsystem In Mode [None, Follower]')

    
    def UpdateLeader(self): # Called To Update Anything That Needs Updating #

        # Skip, Nothing To Do #
        pass


    def UpdateFollower(self): # Called To Update Anything In Follower Mode #

        # Skip, Nothing To Do #
        pass

    