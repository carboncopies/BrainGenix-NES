###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

'''
Name: LFTM
Description: The LFTM is used to handle the transitions between leader and follower modes.
Date-Created: 2021-06-03
'''

import sys

from Core.LFTM.SystemTelemetryManager import SystemTelemetryManager
from Core.LFTM.LADSManager import LADSManager

class LFTM(): # Define Class Object #

    def __init__(self, Logger, ZookeeperInstance, SocketAPI, ThreadManager, KafkaConfig): # Initialize LFTM #

        # Log Start Message #
        Logger.Log('Initializing Leader-Follower Transition Manager')

        # Create Local Pointers #
        self.Logger = Logger
        self.ZookeeperInstance = ZookeeperInstance
        self.SocketAPI = SocketAPI
        self.ThreadManager = ThreadManager
        self.KafkaConfig = KafkaConfig


        # Set Default State #
        self.CurrentZookeeperState = 'Follower' # <-- Don't change this, it's set to follower for a reason...
        self.HasZookeeperStateChanged = False # <-- Again, don't change this, its set this way for initialization.


        # Create Instances Of Requested Systems #                                   # ADD YOUR FUNCTION CALL BELOW HERE!
        self.SystemTelemetryManager = SystemTelemetryManager(self.Logger, self.ZookeeperInstance, self.ThreadManager, self.KafkaConfig)
        self.LADSManager = LADSManager(self.Logger, self.ZookeeperInstance, self.ThreadManager, self.KafkaConfig)

        ## INCLUDE INITIALIZATION CALLS BELOW TO START SUBSYSTEMS ##
        self.SystemTelemetryManager.StartSystem()
        self.LADSManager.StartSystem()


        # Log End Message #
        Logger.Log('Initialization Complete')


    def MainLoop(self): # Main Loop #

        # Log Start Message #
        self.Logger.Log('Starting Main LFTM Loop')


        # Try Except Statement To Catch [Control]+[C] #
        try:

            # Enter Main Loop #
            while True:


                # Fetch Current ZK State #
                ZookeeperState = self.ZookeeperInstance.ZookeeperMode

                # Check If State Updated #
                if (ZookeeperState != self.CurrentZookeeperState):

                    # Log Change Message #
                    self.Logger.Log('Zookeeper State Change Asserted')
                    self.Logger.Log(f'Zookeeper State Is Now {ZookeeperState}')

                    # Update State, Set Has Changed To True #
                    self.HasZookeeperStateChanged = True
                    self.CurrentZookeeperState = ZookeeperState



                # TRANSITION CONDIITONALS BELOW, CALLED WHEN SYSTEM TRANSITIONS TO/FROM LEADER/FOLLOWER #
                if ((self.HasZookeeperStateChanged) and (self.CurrentZookeeperState == 'Follower')):

                    # System Degraded To Follower, or Starting Up #
                    self.Logger.Log('Calling Follower Transition Functions')
                    self.HasZookeeperStateChanged = False

                    # Call Transition Functions #                                   # ADD YOUR FUNCTION CALL BELOW HERE!
                    self.SystemTelemetryManager.TransitionFollower()
                    self.LADSManager.TransitionFollower()


                elif ((self.HasZookeeperStateChanged) and (self.CurrentZookeeperState == 'Leader')):

                    # System Upgraded To Leader #
                    self.Logger.Log('Calling Leader Transition Functions')
                    self.HasZookeeperStateChanged = False

                    # Call Transition Functions #                                   # ADD YOUR FUNCTION CALL BELOW HERE!
                    self.SystemTelemetryManager.TransitionLeader()
                    self.LADSManager.TransitionLeader()



                # MAIN CONDITIONALS, CALLED WHEN SYSTEM STATE IS CONSISTENT #
                elif ((not self.HasZookeeperStateChanged) and (self.CurrentZookeeperState == 'Follower')):

                    # Execute Follower Main Loop Calls #                            # ADD YOUR FUNCTION CALL BELOW HERE!
                    self.SystemTelemetryManager.UpdateFollower()
                    self.LADSManager.UpdateFollower()



                elif ((not self.HasZookeeperStateChanged) and (self.CurrentZookeeperState == 'Leader')):

                    # Execute Leader Main Loop Calls #                              # ADD YOUR FUNCTION CALL BELOW HERE!
                    self.SystemTelemetryManager.UpdateLeader()
                    self.LADSManager.UpdateLeader()



        except KeyboardInterrupt:

            # Call Shutdown Function #
            self.Logger.Log('System Shutdown Invoked By KeyboardInterrupt')

            self.ShutdownSystem()



    # Run Shutdown Commands On System Exit #
    def ShutdownSystem(self):

        # Log Shutdown Message #
        self.Logger.Log('Shutting Down System')


        # Shut Down System #
        self.ThreadManager.ShutdownSystem()


        # Print SQL Style Message #
        print('\x1b[38;2;256;0;0m' + 'Bye.' + '\x1b[0m')


        # Exit Program #
        sys.exit()

