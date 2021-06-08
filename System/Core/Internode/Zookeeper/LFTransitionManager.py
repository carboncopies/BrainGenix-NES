###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

'''
Name: LFTM
Description: The LFTM is used to handle the transitions between leader and follower modes.
Date-Created: 2021-06-03
'''


import os
#import time


from System.Core.LFTM.SystemTelemetryManager import SystemTelemetryManager



class LFTM(): # Define Class Object #

    def __init__(self, Logger, ZookeeperInstance, SocketAPI): # Initialize LFTM #

        # Log Start Message #
        Logger.Log('Initializing Leader-Follower Transition Manager')

        # Create Local Pointers #
        self.Logger = Logger
        self.ZookeeperInstance = ZookeeperInstance
        self.SocketAPI = SocketAPI

        # Set Default State #
        self.CurrentZookeeperState = 'Follower' # <-- Don't change this, it's set to follower for a reason...
        self.HasZookeeperStateChanged = False # <-- Again, don't change this, its set this way for initialization.


        # Create Instances Of Requested Systems #                                   # ADD YOUR FUNCTION CALL BELOW HERE!
        self.SystemTelemetryManager = SystemTelemetryManager(self.Logger, self.ZookeeperInstance)


        ## INCLUDE INITIALIZATION CALLS BELOW TO START SUBSYSTEMS ##
        self.SystemTelemetryManager.StartSystem()


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


                elif ((self.HasZookeeperStateChanged) and (self.CurrentZookeeperState == 'Leader')):

                    # System Upgraded To Leader #
                    self.Logger.Log('Calling Leader Transition Functions')
                    self.HasZookeeperStateChanged = False

                    # Call Transition Functions #                                   # ADD YOUR FUNCTION CALL BELOW HERE!
                    self.SystemTelemetryManager.TransitionLeader()



                # MAIN CONDITIONALS, CALLED WHEN SYSTEM STATE IS CONSISTENT #
                elif ((not self.HasZookeeperStateChanged) and (self.CurrentZookeeperState == 'Follower')):

                    # Execute Follower Main Loop Calls #                            # ADD YOUR FUNCTION CALL BELOW HERE!
                    self.SystemTelemetryManager.UpdateFollower()



                elif ((not self.HasZookeeperStateChanged) and (self.CurrentZookeeperState == 'Leader')):

                    # Execute Leader Main Loop Calls #                              # ADD YOUR FUNCTION CALL BELOW HERE!
                    self.SystemTelemetryManager.UpdateLeader()



        except KeyboardInterrupt:

            # Call Shutdown Function #
            self.Logger.Log('System Shutdown Invoked By KeyboardInterrupt')

            self.ShutdownSystem()



    # Run Shutdown Commands On System Exit #
    def ShutdownSystem(self):

        # Log Shutdown Message #
        self.Logger.Log('Shutting Down System')


        # Call Shutdown Functions #
        self.ZookeeperInstance.Exit()
        self.SocketAPI.Quit()
        self.Logger.CleanExit()


        # Print SQL Style Message #
        print('Bye.')


        # Exit Program #
        os._exit(1)
