###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

'''
Name: API Functions
Description: This file is used to connect the system's API to the leader.
Date-Created: 2021-03-03
'''

class TelemetryAPI(): # Exposes Telemetry Functions To The Leader # 

    def __init__(self, ZooKeeper, Logger, LeaderAttributes): # Initializes the System #

        # Set Local Variables #
        self.LeaderAttributes = LeaderAttributes
        self.Zookeeper = Zookeeper
        self.Logger = Logger

        # Log Message #
        self.Logger.Log('Starting Telemetry API Handler')