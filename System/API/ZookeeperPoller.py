###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

import time
import threading
import json

'''
Name: API Poll Watcher
Description: This file is watches for new zookeeper connections via the API and routes them to the appropriate functions.
Date-Created: 2021-03-04
'''

class PollWatcher(): # Watches Zookeeper to check for new API requests #

    def __init__(self, Logger, Zookeeper, SystemTelemetry): # Initializes The System #

        # Initialize Variables #
        self.Zookeeper = Zookeeper
        self.Logger = Logger

        # Create Local Vars #
        self.HasInitialized = False

        self.KnownConnections = []
        self.ConnectionThreads = []

        self.PollingThread = None
        self.LeaderCheckThread = None

        # Ensure Zookeeper Path #
        self.Zookeeper.ZookeeperConnection.ensure_path('/BrainGenix/API/Connections')

        # Create Leader Watcher Thread #
        self.LeaderCheckThread = threading.Thread(target=self.LeaderCheckerThread)
        self.LeaderCheckThread.start()


    def LeaderCheckerThread(self, PollingInterval:float = 0.5): # Checks If This Node Is Running As A Leader #

        # Main Loop #
        while True:

            # Delay For PollingInterval Time In Seconds #
            time.sleep(PollingInterval)

            if (self.Zookeeper.ZookeeperMode == 'Leader') and (not self.HasInitialized):

                # Start Polling Thread #
                self.PollingThread = threading.Thread(target=self.PollingThreadFunction)
                self.PollingThread.start()

                # Log Event #
                self.Logger.Log('Started Polling Thread')

                # Update Has Initialized #
                self.HasInitialized = True

            elif (self.Zookeeper.ZookeeperMode == 'Follower') and (not self.HasInitialized):

                # UnInit Threads #
                self.PollingThread = None
                self.ConnectionThreads = []

                # Log Event #
                self.Logger.Log('Zoookeeper Polling System Now Running As Follower')

                # Update Initialized Var #
                self.HasInitialized = True


    def PollingThreadFunction(self, PollingInterval:float = 0.5): # Polls The API zNode For New Connections #

        # Enter Loop #
        while True:

            # Check For New Connections #
            EstablishedConnections = self.Zookeeper.ZookeeperConnection.get_children('/BrainGenix/API/Connections')

            for ConnectionIndex, ConnectionID in enumerate(EstablishedConnections):
                if ConnectionID not in self.KnownConnections:

                    # Create Connection Thread #
                    Thread = threading.Thread(target=self.ConnectionHandler, args=(ConnectionID, ))
                    Thread.start()

                    self.ConnectionThreads.append(Thread)

                    # Add Connection To Handled List #
                    self.KnownConnections.append(ConnectionID)

            # Delay For Set Interval #
            time.sleep(PollingInterval)


    def ConnectionHandler(self, TargetConnection, PollingInterval:float = 0.025): # Executes Functions For A Given Command #

        # Log Info #
        self.Logger.Log(f'Started Connection Thread For Connection: {TargetConnection}')

        # Enter Loop # 
        while True:

            # Delay For Polling Interval #
            time.sleep(PollingInterval)

            # Check For New Commands #
            try:
                CommandData = self.Zookeeper.ZookeeperConnection.get(f'/BrainGenix/API/Connections/{TargetConnection}')[0]
            except: ## THIS NEEDS TO BE SPECIFIED TO NoNodeError For Kazoo! ##
                self.Logger.Log(f'Connection {TargetConnection} Destroyed')
                return

            if (CommandData != b'') and (CommandData != None):


                # Parse Command #
                RawJSONString = CommandData.decode('utf-8')
                CommandDictionary = json.loads(RawJSONString)


                CommandCallStack = CommandDictionary['CallStack']
                KeywordArguments = CommandDictionary['KeywordArgs']

                # Get Target Function #
                Layers = CommandCallStack.split('.')
                CommandFunction = self

                for LayerIndex, LayerName in enumerate(Layers):
                    CommandFunction = getattr(CommandFunction, LayerName)

                # Send Arguments #
                CommandFunction(KeywordArguments)

