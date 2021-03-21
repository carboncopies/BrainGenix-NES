###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

'''
Name: API Server
Description: This is the file used by the BrainGenix API System to get the server to communicate with the leader. This is run standalone due to issues with IP addresses changing based on the currently elected leader zookeeper node.
Date-Created: 2021-03-03
'''

import atexit
import random
import json

from fastapi import FastAPI

from Core.LoadConfig import LoadConfig
from Core.Logger import SysLog

from Zookeeper.Zookeeper import ZK


#https://realpython.com/python-https/
#Client -HTTP> Proxy(This File, Hosts API) -zNode> Zookeeper -zNode> Leader -zNode(s)> Follower(s)



# Set Version Information
Version = '0.0.4'


# Load Config #
ConfigPath = 'Config/LocalConfig.yaml'
AddonsPath, LogPath, BufferLength, PrintLogOutput, LinesPerFile, EnableGzip, ZKHost, DBUname, DBPasswd, DBHost, DBName = LoadConfig(ConfigPath)


# Initialize Logger #
Logger = SysLog('0', LogPath+'-APIServer', BufferLength=BufferLength, LogSegmentLength=LinesPerFile, ConsoleOutputEnabled=PrintLogOutput, EnableGzip = EnableGzip) # NOTE: THE SYSLOG ID HERE NEEDS TO BE REPLACED WITH THE ZOOKEEPER ID LATER ON! (FIRST ARG)


# Purges The Log Buffer On System Exit #
@atexit.register
def CleanLog():
    Logger.PurgeBuffer()
    Logger.CleanExit()


# Initialize ZK #
Zookeeper = ZK(Logger)
Zookeeper.ConnectToZookeeper(Logger, ZKHost)


# Define API #
app = FastAPI()

# Create A Connection zNode #
ConnectionNode = f'/BrainGenix/API/Connections/{random.randint(0,38564328964397256432564372)}'
Zookeeper.ZookeeperConnection.create(ConnectionNode, ephemeral=True)


# Define Methods In API #
@app.get('/')
async def root():
    return {'message': 'test'}

@app.get('/test')
async def test():

    Zookeeper.ZookeeperConnection.set(ConnectionNode, b'{"CallStack": "Version", "KeywordArgs": {}}')

    # value = b'{"CallStack": "Version", "KeywordArgs": {}}'

    while Zookeeper.ZookeeperConnection.get(ConnectionNode)[0] == b'{"CallStack": "Version", "KeywordArgs": {}}':
        pass


    return json.loads(Zookeeper.ZookeeperConnection.get(ConnectionNode)[0].decode())


# Start System #
Logger.Log('Starting API Server')
Logger.Log('')
Logger.Log('---------------------------------------------------------------------------')
Logger.Log('██████╗ ██████╗  █████╗ ██╗███╗   ██╗ ██████╗ ███████╗███╗   ██╗██╗██╗  ██╗')
Logger.Log('██╔══██╗██╔══██╗██╔══██╗██║████╗  ██║██╔════╝ ██╔════╝████╗  ██║██║╚██╗██╔╝')
Logger.Log('██████╔╝██████╔╝███████║██║██╔██╗ ██║██║  ███╗█████╗  ██╔██╗ ██║██║ ╚███╔╝ ')
Logger.Log('██╔══██╗██╔══██╗██╔══██║██║██║╚██╗██║██║   ██║██╔══╝  ██║╚██╗██║██║ ██╔██╗ ')
Logger.Log('██████╔╝██║  ██║██║  ██║██║██║ ╚████║╚██████╔╝███████╗██║ ╚████║██║██╔╝ ██╗')
Logger.Log('╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═╝╚═╝╚═╝  ╚═══╝ ╚═════╝ ╚══════╝╚═╝  ╚═══╝╚═╝╚═╝  ╚═╝')
Logger.Log('---------------------------------------------------------------------------')
Logger.Log('')
Logger.Log('    +-----------------------------------------------------------------+')
Logger.Log(f'    |          Welcome To BrainGenix-APIServer Version {Version}          |')
Logger.Log('    +-----------------------------------------------------------------+')
Logger.Log('')


