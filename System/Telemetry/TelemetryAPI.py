###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

'''
Name: API Functions
Description: This file is used to connect the system's API to the leader.
Date-Created: 2021-03-03
'''

class TelemetryAPI(): # Exposes Telemetry Functions To The Leader #

    def __init__(self, Zookeeper, Logger, LeaderAttributes): # Initializes the System #

        # Set Local Variables #
        self.LeaderAttributes = LeaderAttributes
        self.Zookeeper = Zookeeper
        self.Logger = Logger

        # Log Message #
        self.Logger.Log('Starting Telemetry API Handler')



    def ReadZKCommands(self): # Reads the command and creates a response # 

        pass


    def GetClusterSize(self): # Gets the cluster's size #
        
        # Get The Number Of Currently Connected Nodes In the System #
        return len(self.LeaderAttributes.Info)


    def GetOperatingSystem(self, NodeName) # Gets The Operating System Name #

        # Get The Info # 
        NodeJSON = self.LeaderAttributes.Info[NodeName]
        OSName = NodeJSON['OperatingSystemName']
        
        # Return The Information # 
        return OSName

    
    def GetNodeHostname(self, NodeName): # Gets The Node's Hostname # 

        # Get The Info # 
        NodeJSON = self.LeaderAttributes.Info[NodeName]
        NodeName = NodeJSON['NodeName']
        
        # Return The Information # 
        return NodeName


    def GetOperatingSystemRelease(self, NodeName) # Gets The Operating System Version #

        # Get The Info # 
        NodeJSON = self.LeaderAttributes.Info[NodeName]
        Info = NodeJSON['OperatingSystemRelease']
        
        # Return The Information # 
        return Info