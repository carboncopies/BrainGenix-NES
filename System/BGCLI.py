###########################################################
# NOTE: DO NOT USE THIS IN PRODUCTION, IT IS *NOT* SECURE #
###########################################################

###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

import time
import uuid
import atexit
import json

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


    def __init__(self, ZKAddress:str):

        '''
        Initializes the Client, Authenticates, and waits until connection accepted.
        '''

        # Connect To ZK #
        self.ZookeeperConnection = KazooClient(hosts=ZKAddress)
        self.ZookeeperConnection.start()
        self.Host = ZKAddress


        self.UUID = str(uuid.uuid1())

        # Create A Connection zNode #
        self.ConnectionNode = f'/BrainGenix/API/Connections/{self.UUID}'

        self.ZookeeperConnection.create(self.ConnectionNode, ephemeral=True)

        # Print Welcome Message #
        print('Type "Help", "License", "Authors" or "Copyright" for more information.')


    def Main(self): # Main Loop #

        '''
        This is the actual interactive ZKCLI aspect of the client.
        This contains the loop that gets the user's input and passes it on to the server.
        '''

        # Loop For Connection #
        while True:

            # Get Command Input From User #
            Command = input('root@BrainGenix:/# ')

            if Command == '':
                pass
            elif Command == 'exit':
                exit()
            else:

                # Format The Connection #
                CommandParsed = Command.split(' ')

                CommandPrefix = CommandParsed[0]

                Arguments = {}
                for Argument in CommandParsed[1:]:
                    Value = Argument.split('=')[0]
                    Value2 = Argument.split('=')[1]

                    Arguments.update({Value : Value2})

                # Compile Command Into Dictionary #
                CommandDictionary = {}

                CommandDictionary.update({'CallStack' : CommandPrefix})
                CommandDictionary.update({'KeywordArgs' : Arguments})

                # Encode As JSON #
                JSONString = json.dumps(CommandDictionary)


                self.ZookeeperConnection.set(self.ConnectionNode, JSONString.encode())


                # Block Until Command Finished #
                while self.ZookeeperConnection.get(self.ConnectionNode)[0] == JSONString.encode():
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
CLI = Client('10.0.4.3:2181')
CLI.Main()