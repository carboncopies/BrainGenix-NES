###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

'''
Name: API Server
Description: This is the file used by the BrainGenix API System to get the server to communicate with the leader. This is run standalone due to issues with IP addresses changing based on the currently elected leader zookeeper node.
Date-Created: 2021-03-03
'''

import atexit
from random import SystemRandom
import json

from fastapi import FastAPI

from Core.LoadConfig import LoadLoggerConfig
from Core.LoadConfig import LoadDatabaseConfig
from Core.LoadConfig import LoadZookeeperConfig
from Core.LoadConfig import LoadKafkaConfig

from Core.Instantiator import InstantiateZK
from Core.Instantiator import InstantiateKafka
from Core.Instantiator import InstantiateDB
from Core.Instantiator import InstantiateLogger

from Zookeeper.Zookeeper import ZK


#https://realpython.com/python-https/
#Client -HTTP> Proxy(This File, Hosts API) -zNode> Zookeeper -zNode> Leader -zNode(s)> Follower(s)



##############################################################################
## NOTE: A Lowercase "m" Preceeding A Class Means It's a Main System        ##
## NOTE: A Lowercase "s" Preceeding A Class Means It's a Subsystem          ##
##############################################################################



# Set Version Information
Version = '0.0.7'
Branch = 'dev' # 'dev' or 'rel'


# Load Config #
LogPath, PrintLogOutput, SecondsToKeepLogs = LoadLoggerConfig(ConfigFilePath = 'Config/LoggerConfig.yaml')
DBUname, DBPasswd, DBHost, DBName = LoadDatabaseConfig(ConfigFilePath = 'Config/DatabaseConfig.yaml')
ZKHost = LoadZookeeperConfig(ConfigFilePath = 'Config/ZookeeperConfig.yaml')
KafkaHost = LoadKafkaConfig(ConfigFilePath = 'Config/KafkaConfig.yaml')


# Initialize Logger #
mLogger = InstantiateLogger(DBUname, DBPasswd, DBHost, DBName, SecondsToKeepLogs, LogPath, PrintLogOutput)



# Purges The Log Buffer On System Exit #
@atexit.register
def CleanLog():
    mLogger.CleanExit()


# Connect To Zookeeper Service #
sZookeeper = InstantiateZK(mLogger, ZKHost)


# Define API #
app = FastAPI()


# Create A Connection zNode #
cryptogen = SystemRandom()
ConnectionNode = f'/BrainGenix/API/Connections/{cryptogen.randrange(38564328964397256432564372)}'
sZookeeper.ZookeeperConnection.create(ConnectionNode, ephemeral=True)


# Define Methods In API #
@app.get('/')
async def root():
    return {'message': 'test'}

@app.get('/test')
async def test():

    sZookeeper.ZookeeperConnection.set(ConnectionNode, b'{"CallStack": "Version", "KeywordArgs": {}}')

    # value = b'{"CallStack": "Version", "KeywordArgs": {}}'

    while sZookeeper.ZookeeperConnection.get(ConnectionNode)[0] == b'{"CallStack": "Version", "KeywordArgs": {}}':
        pass


    return json.loads(sZookeeper.ZookeeperConnection.get(ConnectionNode)[0].decode())


# Start System #
mLogger.Log('Starting API Server')
mLogger.Log('')
mLogger.Log('---------------------------------------------------------------------------')
mLogger.Log('██████╗ ██████╗  █████╗ ██╗███╗   ██╗ ██████╗ ███████╗███╗   ██╗██╗██╗  ██╗')
mLogger.Log('██╔══██╗██╔══██╗██╔══██╗██║████╗  ██║██╔════╝ ██╔════╝████╗  ██║██║╚██╗██╔╝')
mLogger.Log('██████╔╝██████╔╝███████║██║██╔██╗ ██║██║  ███╗█████╗  ██╔██╗ ██║██║ ╚███╔╝ ')
mLogger.Log('██╔══██╗██╔══██╗██╔══██║██║██║╚██╗██║██║   ██║██╔══╝  ██║╚██╗██║██║ ██╔██╗ ')
mLogger.Log('██████╔╝██║  ██║██║  ██║██║██║ ╚████║╚██████╔╝███████╗██║ ╚████║██║██╔╝ ██╗')
mLogger.Log('╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═╝╚═╝╚═╝  ╚═══╝ ╚═════╝ ╚══════╝╚═╝  ╚═══╝╚═╝╚═╝  ╚═╝')
mLogger.Log('---------------------------------------------------------------------------')
mLogger.Log('')
mLogger.Log('    +-----------------------------------------------------------------+')
mLogger.Log(f'    |          Welcome To BrainGenix-APIServer Version {Version}          |')
mLogger.Log('    +-----------------------------------------------------------------+')
mLogger.Log('')


