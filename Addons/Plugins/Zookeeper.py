###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

import time
import uuid

from kazoo.client import KazooClient

'''
Name: ZooKeeper Interface
Description: This plugin handles interfacing to the zookeeper cluster.
Date-Created: 2021-1-3
'''


class AddonInfo(): # All Addons contain this class, and it tells the system what dependencies this module has #

    def __init__(self):

        self.Version = '0.0.1'
        self.Author = 'BrainGenix Team'
        self.Dependencies = []
        self.Imports = ['kazoo', 'uuid', 'time']

class FollowerMain(): # Main Class #

    def __init__(self, **kwargs):


        # Extract Logger From kwargs #

        Logger = kwargs['Logger']


        #########################################################################
        # NOTE: The Max zNode Size Is 1MB, So Large Files Will Need To Be Split #
        #########################################################################


        # Initialize ZK #

        self.ConnectToZookeeper(Logger)
        self.InitializeZNodes(Logger)
        self.ElectLeader(Logger)


        # Set Local Variables #

        self.ZookeeperMode = 'Follower'
        self.ZookeeperModeOld = 'Follower'



    def ConnectToZookeeper(self, Logger:object=None): # Creates Connection With ZK #


        # Connect To Zookeeper #

        ZKHosts = '127.0.0.1:2181'

        if Logger != None:
            Logger.Log(f'Connecting To Zookeeper Server At Address: {ZKHosts}')

        self.ZookeeperConnection = KazooClient(hosts=ZKHosts)
        self.ZookeeperConnection.start()

        if Logger != None:
            Logger.Log('Established Connection To Zookeeper')
            Logger.Log('NOTE: The Default Max zNode Size Is 1MB, Ensure No Writes To Any zNodes Exceed This Value', 1)


    def InitializeZNodes(self, Logger:object=None): # Initializes BrainGenix ZK Paths #


        # Create zNode Path(s) In Zookeeper #

        if Logger != None:
            Logger.Log('Ensuring zNode Paths In Zookeeper')

        self.ZookeeperConnection.ensure_path('/BrainGenix/Interconnects')
        self.ZookeeperConnection.ensure_path('/BrainGenix/Logs')
        self.ZookeeperConnection.ensure_path('/BrainGenix/Config')
        self.ZookeeperConnection.ensure_path('/BrainGenix/Nodes')
        self.ZookeeperConnection.ensure_path('/BrainGenix/SystemInfo/Dynamic')
        self.ZookeeperConnection.ensure_path('/BrainGenix/SystemInfo/Static')             
        

        if Logger != None:
            Logger.Log('Completed zNode Path Check')


    def ElectLeader(self, Logger:object=None): # Elects A Leader From The Pool #


        # Create Zookeeper Election Object #

        if Logger != None:
            Logger.Log('Electing Leader From Zookeeper Ensemble')

        UUIDString = str(uuid.uuid1())

        ZookeeperElection = self.ZookeeperConnection.Election("/BrainGenix/Nodes", UUIDString)

        ZookeeperElection.run(self.ElectedLeader)

        if Logger != None:
            Logger.Log('Election Complete')
            Logger.Log(f'This Node Is Running In {self.ZookeeperMode} Mode')


    def ElectedLeader(self): # This Function Is Called If We're Elected Leader From The ZK Ensemble #

        self.ZookeeperMode = 'Leader'



    ########################################################################################
    # HANDLE ZOOKEEPER C.R.U.D (Create Nodes, Reading Data, Updating Data, Deleting Nodes) #
    ########################################################################################


    # def Create(self, zNodePath:str, zNodeData:bytes=b'', ephemeral:bool=False):

    #     self.ZookeeperConnection.create(zNodePath, value=zNodeData, ephemeral=ephemeral)
    
    # def CreateAsync(self, zNodePath:str, ephemeral:bool=False):

    #     self.ZookeeperConnection.create_async(zNodePath, ephemeral=ephemeral)

    # def EnsureExists(self, zNodePath:str):

    #     self.ZookeeperConnection.ensure_path(zNodePath)

    # def Get(self, zNodePath:str):

    #     return self.ZookeeperConnection.get(zNodePath)

    # def GetChildren(self, zNodePath:str):

    #     return self.ZookeeperConnection.get_children(zNodePath)

    # def Exists(self, zNodePath:str):

    #     return self.ZookeeperConnection.exists(zNodePath)

    # def Set(self, zNodePath:str, zNodeData:bytes=b''):

    #     self.ZookeeperConnection.set(zNodePath, value=zNodeData)

    # def SetAsync(self, zNodePath:str, zNodeData:bytes=b''):

    #     self.ZookeeperConnection.set_async(zNodePath, value=zNodeData)

    # def Delete(self, zNodePath:str, recursive:bool=False):

    #     self.ZookeeperConnection.delete(zNodePath, recursive=recursive)
        
    # def DeleteAsync(self, zNodePath:str, recursive:bool=False):

    #     self.ZookeeperConnection.delete_async(zNodePath, recursive=recursive)

    # def TryCreate(self, zNodePath:str, ephemeral:bool=False, zNodeData:bytes=None):

    #     if not self.ZookeeperConnection.exists(zNodePath):
    #         self.ZookeeperConnection.create(zNodePath, ephemeral=ephemeral, value=zNodeData)

    
    def TryCreate(self, zNodePath:str, ephemeral:bool=False, zNodeData:bytes=None):

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