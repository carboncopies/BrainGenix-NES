###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

'''
Name: API Server
Description: This is the file used by the BrainGenix API System to get the server to communicate with the leader. This is run standalone due to issues with IP addresses changing based on the currently elected leader zookeeper node.
Date-Created: 2021-03-03
'''

import atexit

from Core.LoadConfig import LoadConfig
from Core.Logger import SysLog

from Zookeeper.Zookeeper import ZK


#https://realpython.com/python-https/
#Client -> Proxy(This File, Hosts API) -> Zookeeper -> Leader -> Follower(s)
# IS THERE A BETTER WAY TO DO THIS THAT DOESN'T NEED ALL THESE STEPS? I'M NOT SURE OF ONE #

# from flask import Flask

# SECRET_MESSAGE = "fluffy tail"
# app = Flask(__name__)

# @app.route("/")
# def get_secret_message():
#     return SECRET_MESSAGE


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
Logger.Log(f'    |               Welcome To BrainGenix Version {Version}               |')
Logger.Log('    +-----------------------------------------------------------------+')
Logger.Log('')