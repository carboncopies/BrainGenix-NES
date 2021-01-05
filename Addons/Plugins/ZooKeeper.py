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

class Main(): # Main Class #

    def __init__(self, **kwargs):


        # Extract Logger From kwargs #

        Logger = kwargs['Logger']


        #########################################################################
        # NOTE: The Max zNode Size Is 1MB, So Large Files Will Need To Be Split #
        #########################################################################

        self.ConnectToZookeeper(Logger)

        self.InitializeZNodes(Logger)

        self.ElectLeader(Logger)


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
        

        if Logger != None:
            Logger.Log('Completed zNode Path Check')


    def ElectLeader(self, Logger:object=None): # Elects A Leader From The Pool #


        # Create Zookeeper Election Object #

        if Logger != None:
            Logger.Log('Electing Leader From Zookeeper Ensemble')

        UUIDString = str(uuid.uuid1())

        ZookeeperElection = self.ZookeeperConnection.Election("/BrainGenix/Nodes", UUIDString)

        ZookeeperElection.run(self.leaderf)

    def leaderf(self):

        print('---ELECTED LEADER!!!---')