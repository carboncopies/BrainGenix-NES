###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

#import copy

'''
Name: ZKManager
Description: This manages Zookeeper Modes.
Date-Created: 2021-01-29
'''


from System.Core.Management.Telemetry.SystemTelemetry import Follower
from System.Core.Management.Telemetry.SystemTelemetry import Leader



class SystemTelemetryManager(): # Manages the system telemetry leader class #

    def __init__(self, Logger, Zookeeper):

        # Create Local Pointers To These Objects #
        self.Logger = Logger
        self.Zookeeper = Zookeeper


    def StartSystem(self): # Called During Startup #

        # Log Instantiation #
        self.Logger.Log('Starting System Telemetry Subsystem')

        # Instantiate #
        self.SysTelFollower = Follower(Logger=self.Logger, Zookeeper=self.Zookeeper)
        self.SysTelLeader = None

        # Log Finish Message #
        self.Logger.Log('Initalized System Telemetry Subsystem')


    def TransitionLeader(self): # Transitions The System To Follower Mode #

        # Log Instantiation #
        self.Logger.Log('System Telemetry Transition Asserted, Called For Mode "LEADER"')

        # Instantiate #
        self.SysTelLeader = Leader(Logger=self.Logger, Zookeeper=self.Zookeeper)

        # Log Finish Message #
        self.Logger.Log('Initalized System Telemetry Subsystem In Mode [Leader, Follower]')


    def TransitionFollower(self): # Transitions The System To Follower Mode #

        # Log Instantiation #
        self.Logger.Log('System Telemetry Transition Asserted, Called For Mode "FOLLOWER"')

        # Instantiate #
        self.SysTelLeader = None

        # Log Finish Message #
        self.Logger.Log('Initalized System Telemetry Subsystem In Mode [None, Follower]')


    def UpdateLeader(self): # Called To Update Anything That Needs Updating #

        # Skip, Nothing To Do #
        pass


    def UpdateFollower(self): # Called To Update Anything In Follower Mode #

        # Skip, Nothing To Do #
        pass



    ## Include Any mAPI Commands Here ##
    # Command: {"SysName":"NES", "CallStack":"LFTM.SystemTelemetryManager.mAPI_GetClusterSize", "KeywordArgs": {}}
    def mAPI_GetClusterSize(self, APIArgs):

        # Set Help String (NOTE, THE NAMESCHEME IS VERY IMPORTANT! MAKE SURE TO FOLLOW IT! (self.mAPI_[CommandName]_Help = 'HelpString')) #
        self.mAPI_GetClusterSize_Help = 'The GetClusterSize function is responsible for returning the current number of nodes in the cluster. This is returned as an integar.'

        # Set Command #
        return self.Zookeeper.ConcurrentConnectedNodes()


    # Command: {"SysName":"NES", "CallStack":"LFTM.SystemTelemetryManager.mAPI_GetNodeList", "KeywordArgs": {}}
    def mAPI_GetNodeList(self, APIArgs):

        # Set Help String #
        self.mAPI_GetNodeList_Help = 'The Get NodeList function returns a list of the hostnames of all nodes within this cluster'

        # Set Command #
        return self.Zookeeper.ConnectedNodes


    # Command: {"SysName":"NES", "CallStack":"LFTM.SystemTelemetryManager.mAPI_GetNodeStats", "KeywordArgs": {"Node": %Your Node Hostname% }}
    def mAPI_GetNodeStats(self, APIArgs):

        # Set Help String #
        self.mAPI_GetNodeList_Help = 'The Get Node Stats returns a json array of all collected node performance information. You must pass it a JSON request containing {"Node":[NodeName]}.'

        # Set Command #
        NodeName = APIArgs['Node']
        return self.SysTelLeader.Info[NodeName]


    # Command: {"SysName":"NES", "CallStack":"LFTM.SystemTelemetryManager.mAPI_GetAllNodeStats", "KeywordArgs": {}}
    def mAPI_GetAllNodeStats(self, APIArgs):

        # Set Help String #
        self.mAPI_GetNodeList_Help = 'The Get Node Stats returns a json array of all collected node performance information. It does not accept any parameters as input.'

        # Set Command #
        return self.SysTelLeader.Info