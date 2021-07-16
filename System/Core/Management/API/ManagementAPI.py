###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

import socket
import threading
import queue
import json
import select

import pymysql

from Core.VersionData import VersionNumber
from Core.VersionData import BranchVersion

from Core.Management.API.CommandIndexer import IndexCommands
from Core.Management.API.CommandIndexer import FilterPaths

'''
Name: Management API
Description: This file provides the socket based interface to the management api backend.
Date-Created: 2021-05-17
'''

class ManagementAPISocketServer(): # Creates A Class To Connect To The Management API #

    def __init__(self, Logger, SystemConfiguration:dict, ThreadManager:object): # This function initialializes sockets #

        # Get Config Params #
        self.Logger = Logger
        self.Port = SystemConfiguration['Port'] # Get the port from the port config
        self.IPAddr = SystemConfiguration['ZKHost'] # Get The IP Addr from the zoomeeper dict
        self.ThreadManager = ThreadManager

        # Make Local SysConfig Param #
        self.SystemConfiguration = SystemConfiguration

        # Create Socket Host Variable #
        self.Logger.Log('Creating Host Variable')
        self.SocketHost = (self.IPAddr, self.Port)

        # Load License File #
        self.Logger.Log('Loading License Text For Management API Command', 3)
        with open('../LICENSE', 'r') as FileIO:
            self.LicenseText = FileIO.read()
        self.Logger.Log('Loaded License Text File', 2)

        # Bind To Port #
        self.Logger.Log('Binding To Host')
        self.Socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.Socket.bind(self.SocketHost)

        # Listen For Connections #
        self.Logger.Log('Management API Backend Listening For Incoming Connections')
        self.Socket.listen()

        # Create MAPI Thread #
        self.Logger.Log('Starting Management API Server Thread', 3)

        # Create Control Queue #
        self.Logger.Log('Creating Control Queue For ManagementAPISocketServer Thread', 2)
        self.ManagementAPIThreadControlQueue = queue.Queue()
        self.Logger.Log('Created Control Queue For ManagementAPISocketServer Thread', 1)

        # Create Thread #
        self.Logger.Log('Creating Thread For ManagementAPISocketServer Daemon', 2)
        self.Thread = threading.Thread(target=self.ManagementAPIThread, args=(self.ManagementAPIThreadControlQueue, ), name='mAPI Socket Server')
        self.Logger.Log('Created Control Queue For ManagementAPISocketServer Daemon', 1)

        # Start Thread #
        self.Logger.Log('Starting ManagementAPISocketServer Daemon', 2)
        self.Thread.start()
        self.Logger.Log('Started ManagementAPISocketServer Daemon', 1)

        # Add Thread To Control Manager #
        self.Logger.Log('Adding ManagementAPISocketServer To Thread Manager', 3)

        self.Logger.Log('Appending ManagementAPISocketServerControlQueue To Control Queue List', 2)
        self.ThreadManager.ControlQueues.append(self.ManagementAPIThreadControlQueue)
        self.Logger.Log('Appended ManagementAPISocketServerControlQueue To Control Queue List', 1)

        self.Logger.Log('Appending ManagementAPISocketServer Thread Object To ThreadManager Thread List', 2)
        self.ThreadManager.Threads.append(self.Thread)
        self.Logger.Log('Appended ManagementAPISocketServer Thread To ThreadManager Thread List', 1)

        # Set Management API Help Strings #
        self.mAPI_Help_Help = 'Help command. Accepts a path arguemnt to provide specialized help for a given command.'
        self.mAPI_ls_Help = 'Lists Commands, Accpets a Path argument to provide commands for a given path.'
        self.mAPI_Version_Help = 'Returns a version string.'
        self.mAPI_TestAPI_Help = 'Testing for debugging API problems or benchmarking API latency. Returns "but most of all, samy is my hero".'
        self.mAPI_RegenerateCommandIndex_Help = 'Regenerates Command Index, optional Argument RecursionDepth allows user definable recursion depth.'


    def IndexCommands(self, MaxRecursionDepth=4):

        # Logger Message #
        self.Logger.Log('Beginning Command Indexing Process', 3)

        # Index Commands #
        self.Logger.Log('Beginning Recursive Attribute Search', 2)
        self.Logger.Log(f'Max Recursion Depth: {MaxRecursionDepth}' , 0)
        self.CommandIndex = IndexCommands(self, self.Logger, MaxRecursionDepth)
        self.Logger.Log('Recursive Attribute Search Complete',1)

        self.Logger.Log('Starting Attribute Sorting' ,2)
        self.CommandIndex = FilterPaths(self.CommandIndex, self.Logger)
        self.RecursionCommands= self.CommandIndex
        self.Logger.Log('Finished Attribute Sorting' ,1)

        # Log Completion #
        self.Logger.Log('Command Indexing Complete', 2)


    def LinkLFTM(self, LFTMInstance): # Link LFTM #

        # Log Link Start #
        self.Logger.Log('Linking LFTM Instance To APIServer')

        # Link LFTM #
        self.LFTM = LFTMInstance

        # Log Finish Method #
        self.Logger.Log('Linking Complete')


    def ExecuteCommand(self): # Runs The Command #

        # Extract Values From Command Dict #
        CommandCallStack = self.Command['CallStack']
        ArgumentsDictionary = self.Command['KeywordArgs']

        # Get Target Function #
        Layers = CommandCallStack.split('.')
        CommandFunction = self


        # Iterate Through Layers, Run Command Called #
        for LayerIndex, _ in enumerate(Layers):

            try:
                # Run Command With Prefix Included (mAPI_[Command Name]) #
                if LayerIndex < (len(Layers) - 1):
                    CommandFunction = getattr(CommandFunction, Layers[LayerIndex])
                else:
                    CommandFunction = getattr(CommandFunction, 'mAPI_'+Layers[LayerIndex])

                # Run Function #
                CommandOutput = CommandFunction(ArgumentsDictionary)
                CommandName = CommandCallStack


            except Exception as ErrorString:

                # Format Return Error For Return To Client #
                CommandOutput = str(ErrorString)
                CommandName = 'Error'


        # Return Values #
        return CommandOutput, CommandName

    def ManagementAPIThread(self, ControlQueue): # Create A Thread Function For The Management API #

        # Enter Connection Accept Loop #
        while ControlQueue.empty():

            #try:
                # Log That Server Awaiting Connections #
                self.Logger.Log(f'MAPI Server Awaiting Connections On Port: {self.Port}')

                # Set Socket Timeout #
                self.Logger.Log('Setting Socket Timeout', 2)
                self.Socket.settimeout(0.2)
                self.Logger.Log('Set Socket Timeout', 1)

                # Wait Accept Incoming Connections #
                self.Logger.Log('Entering Connection Accept Loop', 2)
                while ControlQueue.empty():

                    # Await Connection #
                    try:
                        self.Connection, self.ConnectionInformation = self.Socket.accept()
                        self.Logger.Log(f'Management API Recieved Connection From: {self.ConnectionInformation}', 7)

                        # Set Nonblocking #
                        self.Connection.setblocking(0)

                        # Exit Loop #
                        break

                    except socket.timeout:
                        pass


                # Enter Listening Loop To Recieve Commands #
                self.Logger.Log('Management API Server Awaiting Commands', 4)
                while ControlQueue.empty():


                    # Get Command From Client #
                    SocketReady = select.select([self.Connection], [], [], 1)
                    if SocketReady[0]:
                        self.Command = self.Connection.recv(65535)
                        self.Command = self.Command.decode()

                        # Check If Command String Empty #
                        if self.Command == '':
                            self.Logger.Log('Management API Client Disconnected, Restarting Server', 6)

                            self.Logger.Log('Destroying Management API Server Socket Connection', 3)
                            self.Connection.close()
                            self.Logger.Log('Socket Connection Destroyed', 2)

                            self.Logger.Log('Invoking New Socket Server Instance', 2)
                            self.ManagementAPIThread(ControlQueue)

                    else:
                        self.Command = None

                    # Check If Command Ready #
                    if self.Command != None:

                        # Convert To Dict From JSON #
                        try:
                            self.Command = json.loads(self.Command)
                        except json.decoder.JSONDecodeError:
                            print(self.Command)
                            self.Logger.Log('Management API Socket Connection Forcibly Terminated', 6)

                        # Check That Command Syntax Is Correct #
                        if str(type(self.Command)) != "<class 'dict'>":
                            CommandOutput = "INVALID DICTIONARY FORMAT"

                        if 'CallStack' not in self.Command:
                            CommandOutput = "COMMAND DOES NOT INCLUDE 'CallStack' FIELD. If using CLI, run 'scope (NES, ERS, STS)'"


                        # Check If Disconnect (Must be before SysName check to remain client agnostic) #
                        elif self.Command['CallStack'] == 'Disconnect':
                            self.Logger.Log('Client Initiated MAPI Disconnect, Connection Closed')
                            self.Connection.close()
                            break


                        # More Command Syntax Checks #
                        elif 'SysName' not in self.Command:
                            CommandOutput = "COMMAND DOES NOT INCLUDE 'SysName' FIELD"

                        elif 'KeywordArgs' not in self.Command:
                            CommandOutput = "COMMAND DOES NOT INCLUDE 'KeywordArgs' FIELD"

                        elif self.Command['SysName'] != 'NES':
                            CommandOutput = "INVALID VALUE FOR 'SysName' FIELD"

                        # Run System Command #
                        else:
                            CommandOutput, CommandName = self.ExecuteCommand()



                        # Encode JSON Output #
                        Response = {"Name" : CommandName, "Content" : CommandOutput}
                        ResponseString = json.dumps(Response)
                        ResponseByteString = ResponseString.encode()

                        # Send Output #
                        self.Connection.sendall(ResponseByteString)

        # Exit Message #
        self.Logger.Log('Management API Socket Server Shutting Down', 4)

        self.Logger.Log('Closing Management API Server Socket', 2)
        self.Quit()
        self.Logger.Log('mAPI Server Socket Closed', 1)

        self.Logger.Log('Finished Shutting Down Management API Socket Server', 3)


    def Quit(self): # Release The Socket #

        if 'Connection' in dir(self):

            self.Connection.close()

        # Close The Socket #
        self.Socket.close()


    def mAPI_ls(self, ArgumentsDictionary): # Old ls Command #

        # Check Command Validity #
        if 'Path' not in ArgumentsDictionary:
            return 'Invalid Argument, Please Check Your "Path" Variable'


        # Get Attributes From Arguments #
        TargetPath = ArgumentsDictionary['Path']

        # Get Attributes #
        AttrTarget = self.LFTM

        if (TargetPath != '' and TargetPath != '.'):

            for TargetPathName in TargetPath.split('.'):
                AttrTarget = getattr(AttrTarget, TargetPathName)

        elif TargetPath == '.':
            AttrTarget = self.LFTM


        Attributes = dir(AttrTarget)

        # Sort Attributes #
        OutAttr = []
        for Attr in Attributes:
            if '__' not in str(Attr):
                OutAttr.append(Attr)

        # Return Output #
        return str(OutAttr)


    def mAPI_Help(self, ArgumentsDictionary): # Provides Basic About The BGCLI #

        # Set Default Help Message #
        HelpMessage = 'This system provides a functional management interface to the BrainGenix system. Please use "ls Path=[path here]" to find commands, and use help Path=[path here] to find more information about a specific command. Please note that commands are seperated via a ".", so if calling command b nested under a, it would be "a.b".'

        # Check If Path Has Been Provided #
        if 'Path' in ArgumentsDictionary:

             # Get Target Function #
            Layers = ArgumentsDictionary['Path'].split('.')
            CommandFunction = self

            # Iterate Through Layers, Run Command Called #
            for LayerIndex in enumerate(Layers):

                    # Run Command With Prefix Included (mAPI_[Command Name]) #
                    if LayerIndex < (len(Layers) - 1):
                        CommandFunction = getattr(CommandFunction, Layers[LayerIndex])
                    else:
                        HelpMessage = getattr(CommandFunction, 'mAPI_'+Layers[LayerIndex] + '_Help')


        return HelpMessage


    def mAPI_Version(self, ArgumentsDictionary): # Provide Versioning Information #

        # Set Full Branch Version #
        if BranchVersion == 'dev':
            BranchVersionType = 'Development'
        elif BranchVersion == 'rel':
            BranchVersionType = 'Release'
        else:
            BranchVersionType = 'ERR-UNKNOWN'

        # Construct Version Information #
        Version = f'BrainGenix-NES (Neuron Emulation System) [{BranchVersionType} Branch] [{VersionNumber}]'

        # Return Version String #
        return Version


    def mAPI_License(self, ArgumentsDictionary): # Provides Basic License Information As Command

        # Return License Text #
        return self.LicenseText

    def DBUpdate(self, SystemConfiguration:dict, command:str): # Executes SQL queries to update commands into the bgdb.Command table #

        # Get Database Config #
        SystemConfiguration = self.SystemConfiguration

        # Connect To DB #
        DBUsername = str(SystemConfiguration.get('DatabaseUsername'))
        DBPassword = str(SystemConfiguration.get('DatabasePassword'))
        DBHost = str(SystemConfiguration.get('DatabaseHost'))
        DBDatabaseName = str(SystemConfiguration.get('DatabaseName'))

        # Connect To Database #
        self.DatabaseConnection = pymysql.connect(
            host = DBHost,
            user = DBUsername,
            password = DBPassword,
            db = DBDatabaseName
        )

        cur = self.DatabaseConnection.cursor(pymysql.cursors.DictCursor)

        cur.execute("INSERT INTO command (commandName) VALUES (%s)",(command))

        self.DatabaseConnection.close()


    def UpdateCommand(self): # Updates commands to bgdb.Command table to establish usage permission levels #

        # Can we add more comments here explaining this?

        for key, value in self.RecursionCommands.items():
            self.DBUpdate(self.SystemConfiguration, key)
            if isinstance(value, dict):
                if len(value)!=0:
                    self.RecursionCommands= value
                    self.UpdateCommand()

    #Returns list of commands that a user can execute based on his/her permission level
    def WriteAuthentication(self):

        # Get Database Config #
        SystemConfiguration = self.SystemConfiguration

        # Connect To DB #
        DBUsername = str(SystemConfiguration.get('DatabaseUsername'))
        DBPassword = str(SystemConfiguration.get('DatabasePassword'))
        DBHost = str(SystemConfiguration.get('DatabaseHost'))
        DBDatabaseName = str(SystemConfiguration.get('DatabaseName'))

        # Connect To Database #
        self.DatabaseConnection = pymysql.connect(
            host = DBHost,
            user = DBUsername,
            password = DBPassword,
            db = DBDatabaseName
        )

        cur = self.DatabaseConnection.cursor(pymysql.cursors.DictCursor)

        rows = cur.execute("SELECT * FROM user WHERE userName=%s AND passwordHash=%s",(DBUsername,DBPassword))

        if rows!=0:
            for row in rows:
                level = row['permissionLevel']
                rows = cur.execute("SELECT * FROM command WHERE permissionLevel=%d",int(level))

                if rows!=0:
                    print("Executable Commands for current permission level:")
                    for row in rows:
                        print(row['commandName'],"\t",row['commandDescription'])

                else:
                    print("No commands available for current permission level")

        else:
            print("No matching user found in Database.")

        self.DatabaseConnection.close()
        
    def addUser(self, SystemConfiguration:dict, userName:str, passwordHash:str, salt:str, firstName:str, lastName:str, notes:str, permissionLevel:int):
        
        # Get Database Config #
        SystemConfiguration = self.SystemConfiguration

        # Connect To DB #
        DBUsername = str(SystemConfiguration.get('DatabaseUsername'))
        DBPassword = str(SystemConfiguration.get('DatabasePassword'))
        DBHost = str(SystemConfiguration.get('DatabaseHost'))
        DBDatabaseName = str(SystemConfiguration.get('DatabaseName'))

        # Connect To Database #
        self.DatabaseConnection = pymysql.connect(
            host = DBHost,
            user = DBUsername,
            password = DBPassword,
            db = DBDatabaseName
        )
        
        cur = self.DatabaseConnection.cursor(pymysql.cursors.DictCursor)
        cur.execute("INSERT INTO user (userName, passwordHash, salt, firstName, lastName, notes, permissionLevel) VALUES (%s,%s,%s,%s,%s,%s,%d)",(userName, passwordHash, salt, firstName, lastName, notes, permissionLevel))    
        
        self.DatabaseConnection.close()

    def mAPI_TestAPI(self, ArgumentsDictionary): # Returns A Test String #

        # You should get this refrerence... (Look it up) #
        return "but most of all, samy is my hero"


    def mAPI_RegenerateCommandIndex(self, ArgumentsDictionary): # Regenerates The Command Index #

        # Log Command Regeneration #
        self.Logger.Log('Command Index Regeneration Invoked By Management API', 6)

        # Regenerate Index #
        if 'RecursionDepth' in ArgumentsDictionary:
            self.IndexCommands(int(ArgumentsDictionary['RecursionDepth']))
        else:
            self.IndexCommands()

        # Log Completion #
        self.Logger.Log('Command Index Regeneration Complete', 6)

        # Return Success #
        return 'Success'
