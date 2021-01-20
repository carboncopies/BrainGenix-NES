###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

import threading
import time
import uuid

from kazoo.client import KazooClient

'''
Name: ZKCLI-Client
Description: This file is the BG Cli client.
Date-Created: 2021-01-19
'''

class Client(): # Client For BrainGenix System #

    def __init__(self, Username:str, Password:str, ZKAddress:str):

        # Connect To ZK #
        self.ZookeeperConnection = KazooClient(hosts=ZKAddress)
        self.ZookeeperConnection.start()

        self.UUID = str(uuid.uuid1())

        # Create A Connection zNode #
        self.ConnectionNode = f'/BrainGenix/CLI/{self.UUID}'
        self.ZookeeperConnection.create(self.ConnectionNode, ephemeral=True)

        # Write The Authentication Information To The System #
        AuthInfo = f'{Username}\n{Password}'
        AuthInfo = AuthInfo.encode()
        self.ZookeeperConnection.set(self.ConnectionNode, AuthInfo)

    def Main(self): # Main Loop #

        while True:

            pass

test = Client('tliao', '123456', '127.0.0.1:2181')