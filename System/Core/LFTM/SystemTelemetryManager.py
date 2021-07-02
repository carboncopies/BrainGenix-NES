###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################


'''
Name: ZKManager
Description: This manages Zookeeper Modes.
Date-Created: 2021-01-29
'''


from Core.Management.Telemetry.SystemTelemetry import Follower
from Core.Management.Telemetry.SystemTelemetry import Leader



class SystemTelemetryManager(): # Manages the system telemetry leader class #

    def __init__(self, Logger, Zookeeper, ThreadManager, SystemConfiguration):

        # Create Local Pointers To These Objects #
        self.Logger = Logger
        self.Zookeeper = Zookeeper
        self.ThreadManager = ThreadManager
        self.SystemConfiguration = SystemConfiguration


        # Set Help Strings (NOTE, THE NAMESCHEME IS VERY IMPORTANT! MAKE SURE TO FOLLOW IT! (self.mAPI_[CommandName]_Help = 'HelpString')) #
        self.mAPI_GetClusterSize_Help = 'The GetClusterSize function is responsible for returning the current number of nodes in the cluster. This is returned as an integar.'
        self.mAPI_GetNodeList_Help = 'The Get NodeList function returns a list of the hostnames of all nodes within this cluster'
        self.mAPI_GetNodeStats_Help = 'The Get Node Stats returns a json array of all collected node performance information. You must pass it a JSON request containing {"Node":[NodeName]}.'
        self.mAPI_GetAllNodeStats_Help = 'The Get Node Stats returns a json array of all collected node performance information. It does not accept any parameters as input.'



    def StartSystem(self): # Called During Startup #

        # Log Instantiation #
        self.Logger.Log('Starting System Telemetry Subsystem', 4)

        # Instantiate #
        self.SysTelFollower = Follower(self.Logger, self.SystemConfiguration, self.Zookeeper)
        self.SysTelLeader = None


        # Connect Local System Data Thread To Thread Manager #
        self.Logger.Log('Adding Local System Data Collection Daemon To Thread Manager', 3)

        self.Logger.Log('Adding Local System Data Collection Daemon Control Queue To Thread Manager', 2)
        self.ThreadManager.ControlQueues.append(self.SysTelFollower.LocalSystemDataDaemonControlQueue)
        self.Logger.Log('Added Local System Data Collection Daemon To Control Queue List', 1)

        self.Logger.Log('Adding Local System Data Collection Daemon Object To Thread Manager', 2)
        self.ThreadManager.Threads.append(self.SysTelFollower.UpdateThread)
        self.Logger.Log('Added Local System Data Collection Daemon Object To Thread List', 1)


        # Connect Local System Autorefresh To Thread Manager #
        self.Logger.Log('Adding SendStatsThread To Thread Manager', 3)

        self.Logger.Log('Adding SendStatsThread Control Queue To Thread Manager', 2)
        self.ThreadManager.ControlQueues.append(self.SysTelFollower.ControlQueueSendStatsThread)
        self.Logger.Log('Added SendStatsThread To Control Queue List', 1)

        self.Logger.Log('Adding SendStatsThread Object To Thread Manager', 2)
        self.ThreadManager.Threads.append(self.SysTelFollower.KafkaDataSendThread)
        self.Logger.Log('Added SendStatsThread Object To Thread List', 1)



        # Log Finish Message #
        self.Logger.Log('Initalized System Telemetry Subsystem', 3)


    def TransitionLeader(self): # Transitions The System To Follower Mode #

        # Log Instantiation #
        self.Logger.Log('System Telemetry Transition Asserted, Called For Mode "LEADER"', 6)

        # Instantiate #
        self.SysTelLeader = Leader(self.Logger, self.SystemConfiguration)

        # SysTelLeader Thread Creation Process #
        self.Logger.Log('Adding Data Collection Daemon To Thread Manager', 3)

        self.Logger.Log('Adding Data Collection Daemon Control Queue To Thread Manager', 2)
        self.ThreadManager.ControlQueues.append(self.SysTelLeader.ControlQueueDataCollectionDaemon)
        self.Logger.Log('Added Data Collection Daemon To Control Queue List', 1)

        self.Logger.Log('Adding Data Collection Thread Object To Thread Manager', 2)
        self.ThreadManager.Threads.append(self.SysTelLeader.UpdateThread)
        self.Logger.Log('Added Data Collection Thread Object To Thread List', 1)


        # Log Finish Message #
        self.Logger.Log('Initalized System Telemetry Subsystem In Mode [Leader, Follower]', 5)


    def TransitionFollower(self): # Transitions The System To Follower Mode #

        # Log Instantiation #
        self.Logger.Log('System Telemetry Transition Asserted, Called For Mode "FOLLOWER"', 6)


        # Join Threads #
        self.Logger.Log('Joining Data Collection Daemon Threads', 3)

        self.Logger.Log('Sending Data Collection Daemon Control Queue Shutdown Message', 2)
        self.SysTelLeader.ControlQueueDataCollectionDaemon.put('Stawp!')
        self.Logger.Log('Sent Shutdown Message To Data Collection Control Queue', 1)

        self.Logger.Log('Joining Data Collection Daemon To Main Thread', 2)
        self.SysTelLeader.UpdateThread.join()
        self.Logger.Log('Joined Data Collection Daemon Thread', 1)


        # Instantiate #
        self.Logger.Log('Destroying Data Collection Daemon Class', 2)
        self.SysTelLeader = None
        self.Logger.Log('Destroyed Data Collection Daemon Class', 1)

        # Log Finish Message #
        self.Logger.Log('Initalized System Telemetry Subsystem In Mode [None, Follower]', 5)


    def UpdateLeader(self): # Called To Update Anything That Needs Updating #

        # Skip, Nothing To Do #
        pass


    def UpdateFollower(self): # Called To Update Anything In Follower Mode #

        # Skip, Nothing To Do #
        pass


    ## Include Any mAPI Commands Here ##
    # Command: {"SysName":"NES", "CallStack":"LFTM.SystemTelemetryManager.mAPI_GetClusterSize", "KeywordArgs": {}}
    def mAPI_GetClusterSize(self, APIArgs):


        # Set Command #
        return self.Zookeeper.ConcurrentConnectedNodes()


    # Command: {"SysName":"NES", "CallStack":"LFTM.SystemTelemetryManager.mAPI_GetNodeList", "KeywordArgs": {}}
    def mAPI_GetNodeList(self, APIArgs):


        # Set Command #
        return self.Zookeeper.ConnectedNodes


    # Command: {"SysName":"NES", "CallStack":"LFTM.SystemTelemetryManager.mAPI_GetNodeStats", "KeywordArgs": {"Node": %Your Node Hostname% }}
    def mAPI_GetNodeStats(self, APIArgs):


        # Set Command #
        NodeName = APIArgs['Node']
        return self.SysTelLeader.Info[NodeName]


    # Command: {"SysName":"NES", "CallStack":"LFTM.SystemTelemetryManager.mAPI_GetAllNodeStats", "KeywordArgs": {}}
    def mAPI_GetAllNodeStats(self, APIArgs):

        # Return Result #
        return self.SysTelLeader.Info