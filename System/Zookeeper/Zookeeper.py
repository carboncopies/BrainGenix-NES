###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

import time
import uuid
import threading
import platform

from kazoo.client import KazooClient

'''
Name: ZooKeeper Interface
Description: This handles interfacing to the zookeeper cluster.
Date-Created: 2021-1-6
'''


class ZK(): # Create Interface Class #


    def __init__(self, Logger):


        #########################################################################
        # NOTE: The Max zNode Size Is 1MB, So Large Files Will Need To Be Split #
        #########################################################################


        # Set Local Variables #
        self.ZookeeperMode = 'Follower'
        self.ZookeeperHaveLeader = False
        self.TransactionTime = 0
        self.Name = LeaderName = platform.uname().node
        self.ConnectedNodes = []
        self.ConnectedNodesLastUpdate = []

        # Create Local Copy Of Logger Pointer #
        self.Logger = Logger


    def ConnectToZookeeper(self, Logger:object=None, ZKHosts:str='127.0.0.1:2181'): # Creates Connection With ZK #


        # Connect To Zookeeper #
        if Logger != None:
            Logger.Log(f'Connecting To Zookeeper Server At Address: {ZKHosts}')

        self.ZookeeperConnection = KazooClient(hosts=ZKHosts)
        self.ZookeeperConnection.start()
 
        if Logger != None:
            Logger.Log('Established Connection To Zookeeper')

        self.ZookeeperConnection.ensure_path('BrainGenix/')
        self.ZookeeperConnection.ensure_path('BrainGenix/System')
        self.ZookeeperConnection.ensure_path('BrainGenix/System/Nodes')
        self.ZookeeperConnection.ensure_path('BrainGenix/CLI')

        self.ConnectedNodesLastUpdate = self.ZookeeperConnection.get_children('/BrainGenix/System/Nodes')
        self.ConnectedNodes = self.ZookeeperConnection.get_children('/BrainGenix/System/Nodes')

        self.TryCreateOverwrite(f'/BrainGenix/System/Nodes/{self.Name}/', zNodeData=b'', ephemeral=True)


    def SpawnCheckerThread(self): # Spawn ZK Leader Check Thread #

        self.LeaderCheckerThread = threading.Thread(target=self.LeaderCheckDaemon, args=(), name='Zookeeper Leader Timeout Checker').start()


    def AutoInitZKLeader(self): # Init ZK Leader #

        self.Logger.Log('Attempting To Locate Zookeeper Leader')

        if not self.CheckIfLeaderExists():

            self.Logger.Log('Failed To Find ZK Leader, Starting Election')

            self.ElectLeader()

            self.ZookeeperHaveLeader = True

        else:

            self.Logger.Log('Leader Located')
            self.Logger.Log('This Node Is Running In Follower Mode')

            self.ZookeeperHaveLeader = True


    def ElectLeader(self): # Elects A Leader From The Pool #


        # Create Zookeeper Election Object #
        self.Logger.Log('Electing Leader From Zookeeper Ensemble')

        UUIDString = str(uuid.uuid1())
        
        ZookeeperElection = self.ZookeeperConnection.Election("/BrainGenix/System/Election", UUIDString)
        ZookeeperElection.run(self.ElectedLeader)

        self.Logger.Log('Election Complete')
        self.Logger.Log(f'This Node Is Running In {self.ZookeeperMode} Mode')


    def CheckIfLeaderExists(self): # Checks If A Leader Has Already Been Elected #

        return self.ZookeeperConnection.exists('/BrainGenix/System/Leader')


    def ElectedLeader(self): # This Function Is Called If We're Elected Leader From The ZK Ensemble #


        # Create LockFile #
        try:
            self.ZookeeperConnection.create('/BrainGenix/System/Leader', self.Name.encode(), ephemeral=True)
            self.ZookeeperMode = 'Leader'
        except:
            self.Logger.Log('Other Node Already Created Lockfile')


    def TryCreate(self, zNodePath:str, ephemeral:bool=False, zNodeData:bytes=None):

        if not self.ZookeeperConnection.exists(zNodePath):
            self.ZookeeperConnection.create(zNodePath, ephemeral=ephemeral, value=zNodeData)
    

    def TryCreateOverwrite(self, zNodePath:str, ephemeral:bool=False, zNodeData:bytes=None):

        if not self.ZookeeperConnection.exists(zNodePath):
            self.ZookeeperConnection.create(zNodePath, ephemeral=ephemeral, value=zNodeData)
        else:
            self.ZookeeperConnection.set(zNodePath, value=zNodeData)


    def LeaderCheckDaemon(self, RefreshInterval=1): # Constantly Checks If Leader Disconnects #

        while True:

            # Check Latency #
            StartTime = time.time()
            LeaderExists = self.ZookeeperConnection.exists('/BrainGenix/System/Leader')
            self.TransactionTime = time.time() - StartTime

            # Check Leader #
            if not LeaderExists:
                self.LeaderTimeout()
            try:
                if (self.ZookeeperConnection.get('/BrainGenix/System/Leader')[0] != self.Name.encode() and (self.ZookeeperMode == 'Leader')):
                    self.Logger.Log('Node Lock File Overwritten, Degrading To Follower!', 1)
                    self.ZookeeperMode = 'Follower'
            except: # Catch exception if node is destroyed during check
                pass

            # Check Conn/Disconn Events #
            self.GetConnectedNodes()
            NewNodes, DelNodes = self.CheckIfNodeChangeEvents()
            self.PrintDifferences(NewNodes, DelNodes)

            # Delay Until Next Update Interval #
            time.sleep(RefreshInterval)


    def GetConnectedNodes(self): # Updates The List Of Connected Nodes In ZK #

        self.ConnectedNodes = self.ZookeeperConnection.get_children('/BrainGenix/System/Nodes')


    def CheckIfNodeChangeEvents(self): # Checks If Any Nodes Have Joined Or Left The Cluster #

        # Check Additions #
        AddedNodes = []
        for NodeName in self.ConnectedNodes:
            if NodeName not in self.ConnectedNodesLastUpdate:
                AddedNodes.append(NodeName)
        
        # Check Subtractions #
        RemovedNodes = []
        for NodeName in self.ConnectedNodesLastUpdate:
            if NodeName not in self.ConnectedNodes:
                RemovedNodes.append(NodeName)

        # Update List Of Nodes Since Last Check #
        self.ConnectedNodesLastUpdate = self.ConnectedNodes

        return AddedNodes, RemovedNodes

    
    def PrintDifferences(self, AddedNodes, SubtractedNodes): # Prints Deltas Between Checks, IE A NODE ADDED OR REMOVED #

        for NewNode in AddedNodes:
            self.Logger.Log(f'Node {NewNode} Connected')

        for RemNode in SubtractedNodes:
            self.Logger.Log(f'Node {RemNode} Disconnected', 1)
            if RemNode == self.Name:
                self.Logger.Log('Did Another Instance Time Out?')
                self.TryCreateOverwrite(f'/BrainGenix/System/Nodes/{self.Name}/', zNodeData=b'', ephemeral=True)


    def LeaderTimeout(self): # Runs if a leader times out #

        self.Logger.Log("Can't find Zookeeper Leader! Attempting To Reconnect", 2)

        self.ZookeeperHaveLeader = False
        self.ZookeeperMode = 'Follower'
        self.AutoInitZKLeader()



   