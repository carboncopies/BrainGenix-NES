###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

import time
import uuid
import platform
import json
import sys

from kazoo.client import KazooClient

'''
Name: ZooKeeper Interface
Description: This handles interfacing to the zookeeper cluster.
Date-Created: 2021-1-6
'''


class ZK(): # Create Interface Class #

    '''
    This class is Braingenix's interface to zookeeper.
    It contains both the Kazoo connection object, as well as some other attributes such as the current mode.
    When the class is instantiated, it connects to zk and starts a monitoring thread.
    This thread checks if there are any nodes joining or leaving.
    It also contains information about the status of the current leader.
    If the leader node disconnects, once it's ephemeral zNode is removed, it'll automatically elect another node.
    This is to provide failover due to a crash.
    It also allows BG to sustain nearly half of all nodes failing without any issues.
    '''


    def __init__(self, Logger):

        '''
        Init function for the zookeeper class.
        This function sets local pointers to the variable containing the Logger class.
        It also defines some variables that will be needed later on.
        *You shouldn't have to interact with this class unless you're debugging BrainGenix.*
        '''

        #########################################################################
        # NOTE: The Max zNode Size Is 1MB, So Large Files Will Need To Be Split #
        #########################################################################


        # Set Local Variables #
        self.ZookeeperMode = 'Follower'
        self.ZookeeperHaveLeader = False
        self.TransactionTime = 0
        self.Name = platform.uname().node
        self.ConnectedNodes = []
        self.ConnectedNodesLastUpdate = []

        # Create Local Copy Of Logger Pointer #
        self.Logger = Logger



    def ConnectToZookeeper(self, Logger:object, SystemConfiguration:dict): # Creates Connection With ZK #

        '''
        This function is used to connect to Zookeeper, hence the name.
        *It's automatically called by the main file, so you still shouldn't have to interact with this function.*
        The function will also ensure that some essential paths exist for BG-Core.
        These paths are as follows: '/BrainGenix/System/Nodes', '/BrainGenix/CLI'
        Lastly, It creates a node with it's own hostname to tell the leader that this node has joined.
        That will create an event in the log that node {hostname} joined.
        '''


        # Extract Values From Dictionary #
        ZKHost = str(SystemConfiguration.get('ZKHost'))
        ZKPort = str(SystemConfiguration.get('ZKPort'))

        ZKHost += f':{ZKPort}'


        # Save Info About Connection #
        self.ZKIP = ZKHost
        self.ZKPort = ZKPort


        # Connect To Zookeeper #
        if Logger != None:
            Logger.Log(f'Connecting To Zookeeper Server At Address: {ZKHost}')

        self.ZookeeperConnection = KazooClient(hosts=ZKHost)
        self.ZookeeperConnection.start()

        if Logger != None:
            Logger.Log('Established Connection To Zookeeper')

        self.ZookeeperConnection.ensure_path('BrainGenix/')
        self.ZookeeperConnection.ensure_path('BrainGenix/System')
        self.ZookeeperConnection.ensure_path('BrainGenix/System/Nodes')
        self.ZookeeperConnection.ensure_path('BrainGenix/CLI')

        self.ConnectedNodesLastUpdate = self.ZookeeperConnection.get_children('/BrainGenix/System/Nodes')
        self.ConnectedNodes = self.ZookeeperConnection.get_children('/BrainGenix/System/Nodes')

        self.TryCreate(f'/BrainGenix/System/Nodes/{self.Name}/', zNodeData=b'', ephemeral=True)



    def ConcurrentConnectedNodes(self): # Return The Number Of Concurrently Connected Nodes #

        '''
        This function is used to get the number of connected concurrent nodes.
        It's used to get the number of connected nodes on the MOTD splash page.
        It just returns the number of children znodes under the system/nodes path in ZK.
        '''

        # Get Number Of Children zNodes #
        NodeCount = len(self.ZookeeperConnection.get_children('/BrainGenix/System/Nodes'))

        # Return Count #
        return NodeCount


    def AutoInitZKLeader(self): # Init ZK Leader #

        '''
        This function does exactly what the name implies, Initializing the ZK connection.
        It's called by the checkthread in the event that the leader is disconnected.
        *As with many of the functions in this file, you shouldn't have to interact with it at all.*
        '''

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

        '''
        This function is used by the AutoInit function to elect a leader if the current leader is missing.
        *It's also not designed to be used by the end user, so you shouldn't have to use it.*
        '''

        # Create Zookeeper Election Object #
        self.Logger.Log('Electing Leader From Zookeeper Ensemble')

        UUIDString = str(uuid.uuid1())

        ZookeeperElection = self.ZookeeperConnection.Election("/BrainGenix/System/Election", UUIDString)
        ZookeeperElection.run(self.ElectedLeader)

        self.Logger.Log('Election Complete')
        self.Logger.Log(f'This Node Is Running In {self.ZookeeperMode} Mode')



    def CheckIfLeaderExists(self): # Checks If A Leader Has Already Been Elected #

        '''
        This function returns a boolean value indicating if the current leader's ephemeral zNode exists.
        *The end user shouldn't have to interact with this function.*
        '''

        return self.ZookeeperConnection.exists('/BrainGenix/System/Leader')



    def ElectedLeader(self): # This Function Is Called If We're Elected Leader From The ZK Ensemble #

        '''
        This function is called when the current node is elected as a leader.
        If this function is called and the node is not actually the leader, it can cause some very unstable behavior.
        *Please don't use this function unless you're really sure what you're doing.*
        '''


        # Create Leader Dictionary #
        LeaderDictionary = {
            'Hostname': self.Name,
            'IP': self.ZKIP
        }

        LeaderDictionaryString = json.dumps(LeaderDictionary)


        # Create LockFile #
        if not self.ZookeeperConnection.exists('/BrainGenix/System/Leader'):
            self.ZookeeperConnection.create('/BrainGenix/System/Leader', LeaderDictionaryString.encode(), ephemeral=True)
            self.ZookeeperMode = 'Leader'
        else:
            self.Logger.Log('Other Node Already Created Lockfile')



    def TryCreate(self, zNodePath:str, ephemeral:bool=False, zNodeData:bytes=None):

        '''
        This function is designed to be used by plugins/modules to create a node even if it might already exist.
        It's faster than first checking, and then creating a node.
        By default, the nodes it creates are not ephemeral, so if you need ephemeral nodes, pass `ephemeral=False`.
        You can also pass data to it as bytes by passing `zNodeData=b'something'`.
        '''

        if not self.ZookeeperConnection.exists(zNodePath):
            self.ZookeeperConnection.create(zNodePath, ephemeral=ephemeral, value=zNodeData)



    def TryCreateOverwrite(self, zNodePath:str, ephemeral:bool=False, zNodeData:bytes=None):

        '''
        This function is designed to be used by plugins/modules to create a node even if it might already exist.
        It will overwrite the node if it already exists, so please be careful about what you're doing here.
        If you're careless, this function can cause undefined behavior.
        It's faster than first checking, and then creating a node.
        By default, the nodes it creates are not ephemeral, so if you need ephemeral nodes, pass `ephemeral=False`.
        You can also pass data to it as bytes by passing `zNodeData=b'something'`.
        '''

        if not self.ZookeeperConnection.exists(zNodePath):
            self.ZookeeperConnection.create(zNodePath, ephemeral=ephemeral, value=zNodeData)
        else:
            self.ZookeeperConnection.set(zNodePath, value=zNodeData)



    def LeaderCheckDaemon(self, ControlQueue, RefreshInterval=1): # Constantly Checks If Leader Disconnects #

        '''
        This function is the actual thread target used by the leader checking process.
        *Don't call this function unless you know what you're doing.*
        '''

        # Enter Loop #
        while ControlQueue.empty():

            # Catch Errors #
            try:


                # Check Latency #
                StartTime = time.time()
                LeaderExists = self.ZookeeperConnection.exists('/BrainGenix/System/Leader')
                self.TransactionTime = time.time() - StartTime

                # Check Leader #
                if not LeaderExists:
                    self.LeaderTimeout()
                if not self.ZookeeperConnection.exists('/BrainGenix/System/Leader'):
                    if (self.ZookeeperConnection.get('/BrainGenix/System/Leader')[0] != self.Name.encode() and (self.ZookeeperMode == 'Leader')):
                        self.Logger.Log('Node Lock File Overwritten, Degrading To Follower!', 1)
                        self.ZookeeperMode = 'Follower'
                else: # Catch exception if node is destroyed during check
                    pass

                # Check Conn/Disconn Events #
                self.GetConnectedNodes()
                NewNodes, DelNodes = self.CheckIfNodeChangeEvents()
                self.PrintDifferences(NewNodes, DelNodes)

                # Delay Until Next Update Interval #
                time.sleep(RefreshInterval)


            except Exception as E:
                if str(E) == 'Connection has been closed':

                    # Log Connection Destroyed #
                    self.Logger.Log('Zookeeper Connection Destroyed, Shutting Down ZKLeaderCheck Daemon', 8)
                    sys.exit()

                else:

                    # Log Other Errors #
                    self.Logger.Log(E, 9)



    def GetConnectedNodes(self): # Updates The List Of Connected Nodes In ZK #

        '''
        This function returns a list of connected nodes' hostnames into a variable called self.ConnectedNodes.
        You can call this function or just poll the list as it's updated by the polling thread.
        '''

        self.ConnectedNodes = self.ZookeeperConnection.get_children('/BrainGenix/System/Nodes')



    def CheckIfNodeChangeEvents(self): # Checks If Any Nodes Have Joined Or Left The Cluster #

        '''
        This function checks if any new nodes have joined or left.
        *Don't call this function unless you know what you're doing*
        '''

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

        '''
        This function prints the list of new/disconnected nodes since the last time the CheckIfNodeChangeEvents function was called.
        *Don't call this function unless you know what you're doing.*
        '''

        for NewNode in AddedNodes:
            self.Logger.Log(f'Node {NewNode} Connected', 4)


        for RemNode in SubtractedNodes:
            self.Logger.Log(f'Node {RemNode} Disconnected', 5)
            if RemNode == self.Name:
                self.Logger.Log('Did Another Instance Time Out?')
                self.TryCreateOverwrite(f'/BrainGenix/System/Nodes/{self.Name}/', zNodeData=b'', ephemeral=True)



    def LeaderTimeout(self): # Runs if a leader times out #

        '''
        This function is called when the leader times out. It will elect a new leader and handle transitioning to leader mode if needed.
        *Don't call this function unless you know what you're doing.*
        '''

        self.Logger.Log("Can't find Zookeeper Leader! No Quorum?", 2)

        self.ZookeeperHaveLeader = False
        self.ZookeeperMode = 'Follower'
        self.AutoInitZKLeader()



    def Exit(self): # Shutsdown the ZK connection #
        self.ZookeeperConnection.stop()
