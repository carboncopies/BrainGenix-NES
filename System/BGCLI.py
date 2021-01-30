###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

import threading
import time
import uuid
import readline
import atexit

from kazoo.client import KazooClient

'''
Name: ZKCLI-Client
Description: This file is the BG Cli client.
Date-Created: 2021-01-19
'''

class Client(): # Client For BrainGenix System #

    '''
    This class contains the ZKCLI client setup.
    It connects to ZK via the arguments passed to __init__ and authenticates via the given username and password.
    The client can then be run via calling the Main function, which drops the user into the actual command line environment.
    '''


    def __init__(self, Username:str, Password:str, ZKAddress:str):

        '''
        Initializes the Client, Authenticates, and waits until connection accepted.
        '''

        # Connect To ZK #
        self.ZookeeperConnection = KazooClient(hosts=ZKAddress)
        self.ZookeeperConnection.start()
        self.Username = Username
        self.Host = ZKAddress
        print('Initializing Connection')

        self.UUID = str(uuid.uuid1())

        # Create A Connection zNode #
        self.ConnectionNode = f'/BrainGenix/CLI/{self.UUID}'
        print('Connected to Zookeeper')
        self.ZookeeperConnection.create(self.ConnectionNode, ephemeral=True)
        print('Created Epehemeral zNode')

        # Write The Authentication Information To The System #
        AuthInfo = f'{Username}\n{Password}'
        AuthInfo = AuthInfo.encode()
        self.ZookeeperConnection.set(self.ConnectionNode, AuthInfo)
        print('Wrote Authentication Information')

        # Block Until Auth Passes #
        while self.ZookeeperConnection.get(self.ConnectionNode)[0] == AuthInfo:
            time.sleep(0.05)
        print('Connection Picked up by Leader')



    def Main(self): # Main Loop #

        '''
        This is the actual interactive ZKCLI aspect of the client.
        This contains the loop that gets the user's input and passes it on to the server.
        '''

        # Loop For Connection #
        while True:

            # Get Command Input From User #
            Command = input(f'[BrainGenix: {self.Host}(Connected:{self.Username})]: ')

            if Command == '':
                pass
            elif Command == 'exit':
                exit()
            else:
                self.ZookeeperConnection.set(self.ConnectionNode, Command.encode())

                # Block Until Command Finished #
                while self.ZookeeperConnection.get(self.ConnectionNode)[0] == Command.encode():
                    time.sleep(0.05)
                
                # Handle Output #
                CommandOutput = self.ZookeeperConnection.get(self.ConnectionNode)[0].decode()

                if CommandOutput != '':
                    print(CommandOutput)
                else:
                    pass

# Set Exit Params #
@atexit.register
def Disconnect():
    CLI.ZookeeperConnection.delete(CLI.ConnectionNode)
    print('BGCLI Connection Destroyed Successfully')

# Instantiate The Client #
CLI = Client('root', 'turing', '10.0.4.3:2181')
CLI.Main()