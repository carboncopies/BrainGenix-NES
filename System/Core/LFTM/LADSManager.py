 
###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

import queue

'''
Name: LADS Manager
Description: Manages The Logger AutoDeletion System.
Date-Created: 2021-07-15
'''


from Core.Management.Logger.LogDeletionSystem import LogAutoDeletionSystem



class LADSManager(): # Manages the system telemetry leader class #

    def __init__(self, Logger, Zookeeper, ThreadManager, SystemConfiguration):

        # Create Local Pointers To These Objects #
        self.Logger = Logger
        self.Zookeeper = Zookeeper
        self.ThreadManager = ThreadManager
        self.SystemConfiguration = SystemConfiguration


    def StartSystem(self): # Called During Startup #

        # Log Initialization #
        self.Logger.Log('Initializing LADS Daemon Threads', 4)

        # Create Control Queue #
        self.Logger.Log('Creating LADS Daemon Control Queue', 2)
        self.ControlQueue = queue.Queue()
        self.Logger.Log('Created LADS Daemon Control Queue', 1)




    def TransitionLeader(self): # Transitions The System To Follower Mode #

        # Log Instantiation #
        self.Logger.Log('System Transition To Leader Asserted, Starting LADS', 6)

        # Instantiate #
        self.LADSDaemon = LogAutoDeletionSystem(self.Logger, self.ControlQueue, self.SystemConfiguration)

        # LADSDaemon Thread Creation Process #
        self.Logger.Log('Adding LADS Daemon To Thread Manager', 3)

        self.Logger.Log('Adding LADS Daemon Control Queue To Thread Manager', 2)
        self.ThreadManager.ControlQueues.append(self.LADSDaemon.ControlQueue)
        self.Logger.Log('Added LADS Daemon To Control Queue List', 1)

        self.Logger.Log('Adding LADS Thread Object To Thread Manager', 2)
        self.ThreadManager.Threads.append(self.LADSDaemon.UpdateThread)
        self.Logger.Log('Added LADS Thread Object To Thread List', 1)


        # Log Finish Message #
        self.Logger.Log('Initalized System Telemetry Subsystem In Mode [Leader, Follower]', 5)


    def TransitionFollower(self): # Transitions The System To Follower Mode #

        # Log Instantiation #
        self.Logger.Log('System Telemetry Transition Asserted, Called For Mode "FOLLOWER"', 6)


        # Join Threads #
        self.Logger.Log('Joining LADS Daemon Threads', 3)

        self.Logger.Log('Sending LADS Daemon Control Queue Shutdown Message', 2)
        self.LADSDaemon.ControlQueueDataCollectionDaemon.put('Stawp!')
        self.Logger.Log('Sent Shutdown Message To LADS Control Queue', 1)

        self.Logger.Log('Joining LADS Daemon To Main Thread', 2)
        self.LADSDaemon.UpdateThread.join()
        self.Logger.Log('Joined LADS Daemon Thread', 1)


        # Instantiate #
        self.Logger.Log('Destroying LADS Daemon Class', 2)
        self.LADSDaemon = None
        self.Logger.Log('Destroyed LADS Daemon Class', 1)

        # Log Finish Message #
        self.Logger.Log('Initalized System Telemetry Subsystem In Mode [None, Follower]', 5)


    def UpdateLeader(self): # Called To Update Anything That Needs Updating #

        # Skip, Nothing To Do #
        pass


    def UpdateFollower(self): # Called To Update Anything In Follower Mode #

        # Skip, Nothing To Do #
        pass