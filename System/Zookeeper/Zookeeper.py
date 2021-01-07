###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

import time
import uuid

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
        self.ZookeeperModeOld = 'Follower'


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
            #Logger.Log('NOTE: The Default Max zNode Size Is 1MB, Ensure No Writes To Any zNodes Exceed This Value', 1)


        self.ZookeeperConnection.ensure_path('BrainGenix/')
        self.ZookeeperConnection.ensure_path('BrainGenix/Nodes')
        

    def AutoInitZKLeader(self): # Init ZK Leader #

        self.Logger.Log('Checking If ZK Leader Already Exists')

        if not self.CheckIfLeaderExists():

            self.Logger.Log('Failed To Find ZK Leader, Starting Election')

            self.ElectLeader()

        else:

            self.Logger.Log('Leader Found, Skipping Election')


    def ElectLeader(self): # Elects A Leader From The Pool #


        # Create Zookeeper Election Object #
        self.Logger.Log('Electing Leader From Zookeeper Ensemble')

        UUIDString = str(uuid.uuid1())

        ZookeeperElection = self.ZookeeperConnection.Election("/BrainGenix/Nodes", UUIDString)

        ZookeeperElection.run(self.ElectedLeader)

        self.Logger.Log('Election Complete')
        self.Logger.Log(f'This Node Is Running In {self.ZookeeperMode} Mode')


    def CheckIfLeaderExists(self): # Checks If A Leader Has Already Been Elected #

        return self.ZookeeperConnection.exists('/BrainGenix/Leader')


    def ElectedLeader(self): # This Function Is Called If We're Elected Leader From The ZK Ensemble #


        # Set Local Mode #
        self.ZookeeperMode = 'Leader'


        # Create LockFile #
        self.ZookeeperConnection.create('/BrainGenix/Leader', ephemeral=True)


    def TryCreate(self, zNodePath:str, ephemeral:bool=False, zNodeData:bytes=None):

        if not self.ZookeeperConnection.exists(zNodePath):
            self.ZookeeperConnection.create(zNodePath, ephemeral=ephemeral, value=zNodeData)
    

    def TryCreateOverwrite(self, zNodePath:str, ephemeral:bool=False, zNodeData:bytes=None):

        if not self.ZookeeperConnection.exists(zNodePath):
            self.ZookeeperConnection.create(zNodePath, ephemeral=ephemeral, value=zNodeData)
        else:
            self.ZookeeperConnection.set(zNodePath, value=zNodeData)


    def HasModeChanged(self): # Tells The User If The System Has Changed Modes #

        if self.ZookeeperMode != self.ZookeeperModeOld:
            self.ZookeeperModeOld = self.ZookeeperMode
            return True
        else:
            return False        