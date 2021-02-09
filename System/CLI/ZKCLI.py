###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

import threading
import time
import inspect

from CLI.CommandParser import TraceCall
from CLI.BuiltinCommands import BG

'''
Name: ZKCLI
Description: This file runs the actual CLI used by BG.
Date-Created: 2021-01-19
'''

class ConnectionInstance(): # This class is instantiated every time a user connects to the CLI #

    '''
    This class handles CLI connection instances.
    It's called by ZKCLI class and handles one specific instance of a connection.

    We'll need to add end-to-end encryption support later, but for now everything is done in plain text.
    Also adding ACL so the client can only interact with the zNode for it's own connection would be a good idea.
    '''

    def __init__(self, Logger:object, ZKInstance:object, SysTelLead:object, zNodeLocation:str, LeaderPluginRegistry:dict, ModuleRegistry:dict):

        '''
        This function creates a local copy of pointers needed later on in the class, such as the logger.
        *Please don't use this unless you know what you're doing!*
        '''

        # Create Local Pointers #
        self.Logger = Logger
        self.ZK = ZKInstance
        self.zNodePath = zNodeLocation
        self.LeaderPluginRegistry = LeaderPluginRegistry
        self.ModuleRegistry = ModuleRegistry

        self.BuiltinCommands = BG(ZKInstance, SysTelLead)

        # Start Main Loop #
        self.MainThread()



    def MainThread(self, PollingInterval:float=0.1): # Main Thread That Runs The Connection #

        '''
        This function contains a loop which is used for every connection.
        First, it handles authentication, and will check this with the user database.
        Next, it goes into interactive mode, which is just a loop containing the actual commands.
        If the user fails an authentication check, the connection is dropped.
        *Please don't use this unless you know what you're doing!*
        '''

        # Authenticate #
        while self.ZK.ZookeeperConnection.get(self.zNodePath)[0] == b'':
            pass

        AuthInfo = self.ZK.ZookeeperConnection.get(self.zNodePath)[0]

        try:
            Username, Password = AuthInfo.decode().split('\n')
        except ValueError:
            self.Logger.Log('Invalid Auth Format, Dropping Connection')
            return

        if ((Username == 'root') and (Password == 'turing')):
            self.Logger.Log(f'Authentication Completed For User {Username}')
        else:
            self.Logger.Log(f'Client Failed Authentication With Uname {Username}')
            return

        # Clear zNode #
        self.ZK.ZookeeperConnection.set(self.zNodePath, b'')

        # Initialize Polling Function #
        self.zNodeReference = b''

        while True:

            try:
                # Get The Command State #
                CommandChanged = self.Poll()

                # Check If Connection Terminated #
                if CommandChanged == 'Connection Closed':
                    return
                
                # Execute The Command If the zNode Data Has Changed #
                if CommandChanged:
                    CommandText = self.GetCommand()
                    CommandOutput = self.CommandHandler(CommandText)
                    self.WriteTextBack(CommandOutput)


            except:
                pass

            # Delay For Polling Period #
            time.sleep(PollingInterval)


    def CommandHandler(self, Command:str): # Handles a command #

        '''
        This function Handles a command from the client.
        It's called by the connection handling thread when a new command is detected.
        *Please Don't Call This Unless You Know What You're Doing!*
        '''

        CallStackList, Arguments = TraceCall(Command)

        # Check if it's a help command #
        if CallStackList[0].lower() in ['?', 'help', 'h']:
            
            HelpPage = '''
Welcome to the BrainGenix Command Line Interface (CLI).
This interface is designed to simplify administration and debugging of the system.
The interface is also designed to resemble Pythonic syntax, as the rest of BrainGenix uses Python as a main language, so the syntax should feel similar.

The interface is layed out as follows: [Root Command].[Subcommand] arg1 arg1value arg2 arg2value, etc.
The root command, simply points the command engine to a class where it can fish out the subcommand from.
The subcommand is simply a function name which is called by the command engine.
If the command is part of a plugin, the root command is going to be the plugin's name.

For example, if the plugin's name is "SystemInfo.py", the Root Command is going to be "SystemInfo".
The subcommand under that is going to be the function name that you want to call.
It is worth noting that the plugin/module will only use commands defined in the leader side for obvious reasons.
Additionally, the root and subcommand are not case sensitive, however the arguments are.

To see a list of built-in commands, please type: "BG.help"
            '''
            return HelpPage

        # Check if it's a Builtin Command #
        if CallStackList[0].lower() == 'bg':

            try:

                if 'CLA' in inspect.getfullargspec(getattr(self.BuiltinCommands, CallStackList[1])).args:
                    CommandMethod = getattr(self.BuiltinCommands, CallStackList[1])
                    CommandOutput = str(CommandMethod(CLA=Arguments))
                    return CommandOutput
                else:
                    
                    CommandMethod = getattr(self.BuiltinCommands, CallStackList[1])
                    CommandOutput = str(CommandMethod())
                    return CommandOutput

            except AttributeError:
                return 'BG_COMMAND_ENGINE: ROOT MODULE DOES NOT HAVE THE REQUESTED ATTRIBUTE'


    def Poll(self): # Polls the Node And Returns True If The Node Changed #
    
        '''
        This function reads the connection zNode, and determines if the user has sent over any new commands.
        It returns false if nothing has changed, and true if a new command is detected.
        It also detects if a client has closed a connection, and sets a local var "ConnectionActive" to 'Connection Closed'.
        *Please Don't Call This Unless You Know What You're Doing!*
        '''

        # Check If Node Still Exists #
        self.ConnectionActive = self.ZK.ZookeeperConnection.exists(self.zNodePath)

        if not self.ConnectionActive:
            self.Logger.Log('Client Closed Connection, Terminating Thread')
            return 'Connection Closed'

        # Get The Current Value #
        CurrentValue = self.ZK.ZookeeperConnection.get(self.zNodePath)[0]

        # Compare Values #
        if CurrentValue != self.zNodeReference:
            self.zNodeReference = CurrentValue
            return True
        return False


    def GetCommand(self): # Returns the command from the command zNode #

        '''
        This recieves the data from zk, and returns a string.
        *Please Don't Call This Unless You Know What You're Doing!*
        '''

        # Read zNode Data #
        CommandBytes = self.ZK.ZookeeperConnection.get(self.zNodePath)[0]

        # Decode And Return #
        return CommandBytes.decode()


    def WriteTextBack(self, Text:str): # Writes back the Command Output #

        '''
        Writes back the output from a command to ZK.
        *Please Don't Call This Unless You Know What You're Doing!*
        '''

        # Encode String #
        if Text != None:
            TextBytes = Text.encode()
        else:
            TextBytes = b''

        # Write To ZK #
        self.ZK.ZookeeperConnection.set(self.zNodePath, TextBytes)


class ZKCLI(): # This class handles creating/destroying Connection Instances based on connections #

    '''
    This class handles the creation and destruction of connection instances.
    It's passed the location of the CLI zNode and it checks for new children underneath it.
    These children are connection instances.
    If a new connection is established, this class will create a new thread with a connection instance to handle it.
    If this node is demoted to follower mode from leader mode, it'll stop all the threads which handle connection instances.
    This is to avoid any issues with two systems handling the CLI at the same time.
    '''


    def __init__(self, Logger:object, ZKInstance:object, SysTelLead:object, LeaderPluginRegistry:dict, ModuleRegistry:dict, zNodeRoot:str='/BrainGenix/CLI'):

        '''
        This function initializes the zkcli handler.
        It creates local copies of the pointers needed by the rest of this class.
        *Please don't use this unless you know what you're doing!*
        '''

        # Create Local Pointers #
        self.Logger = Logger
        self.ZK = ZKInstance
        self.zNodeRoot = zNodeRoot
        self.LeaderPluginRegistry = LeaderPluginRegistry
        self.ModuleRegistry = ModuleRegistry    
        self.SystemTelemetryLeader = SysTelLead    

        # Create Local Vars #
        self.HandledConnections = []
        self.ConnectionInstances = []

        self.Logger.Log('ZKCLI Init Completed')


    def StartPollingThread(self): # Starts a thread that checks if new connections have been established #

        '''
        This function starts the polling thread.
        It creates a threading.Thread, and then calls thread.start()
        *Please don't use this unless you know what you're doing!*
        '''

        # Spawn Thread #
        self.PollerThread = threading.Thread(target=self.PollingFunction, name='CLI Polling Thread').start()

        self.Logger.Log('Started ZKCLI Polling Thread')


    def PollingFunction(self, PollingFrequency:int=1): # Polls the ZK CLI Root for any connections, and spawns new instances to handle it #

        '''
        This function contains the polling function, and is the target of the thread starting function.
        This function should not be called because it contains a loop which will never return.
        *Please don't use this unless you know what you're doing!*
        '''

        while True:

            if self.ZK.ZookeeperMode == 'Leader':            

                # Grab A List Of Connection Instances #
                ConnectionList = self.ZK.ZookeeperConnection.get_children(self.zNodeRoot)

                # Check If It's Already Handled #
                for Connection in ConnectionList:
                    
                    if Connection not in self.HandledConnections:
                        self.SpawnConnectionInstance(Connection)
                        self.Logger.Log(f'ZKCLI Started Connection Handler For Session {Connection}')

            # Delay For The Polling Time #
            time.sleep(PollingFrequency)


    def SpawnConnectionInstance(self, ConnectionName): # Creates A New Connection Instance #

        '''
        This function is used by the polling function to instantiate a new thread each time a new connection is detected.
        *Please don't use this unless you know what you're doing!*
        '''

        # Spawn Thread #
        ConnectionLocation = self.zNodeRoot + '/' + ConnectionName
        NewInstance = threading.Thread(target=ConnectionInstance, args=(self.Logger, self.ZK, self.SystemTelemetryLeader, ConnectionLocation, self.LeaderPluginRegistry, self.ModuleRegistry, ), name=f'ZK CLI Connection Handler On Connection {ConnectionName}')
        NewInstance.start()
        self.ConnectionInstances.append(NewInstance)
        self.HandledConnections.append(ConnectionName)