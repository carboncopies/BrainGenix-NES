###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

'''
Name: Main Loop
Description: This is the main file for a BrainGenix instance.
Date-Created: 2020-12-18
'''

import atexit
import time

from Core.LoadConfig import LoadConfig
from Core.Logger import SysLog
from Core.SystemTelemetry import Follower, Leader
from Core.CheckLibraries import CheckImports#, CheckLibrary

from Zookeeper.Zookeeper import ZK
from Zookeeper.ZKManager import SystemTelemetryManager

from Cryptography.KeyUtils import GenKeys, WriteKeys, ReadKeys, CheckIfKeysExist

from Database.DatabaseInterface import DBInterface


# Set Version Information
Version = '0.0.4'


# Load Config #
ConfigPath = 'Config/LocalConfig.yaml'
AddonsPath, LogPath, BufferLength, PrintLogOutput, LinesPerFile, EnableGzip, ZKHost, DBUname, DBPasswd, DBHost, DBName = LoadConfig(ConfigPath)


# Initialize Logger #
Logger = SysLog('0', LogPath, BufferLength=BufferLength, LogSegmentLength=LinesPerFile, ConsoleOutputEnabled=PrintLogOutput, EnableGzip = EnableGzip) # NOTE: THE SYSLOG ID HERE NEEDS TO BE REPLACED WITH THE ZOOKEEPER ID LATER ON! (FIRST ARG)


# Purges The Log Buffer On System Exit #
@atexit.register
def CleanLog():
    Logger.PurgeBuffer()
    Logger.CleanExit()


# Connect To DB #
DatabaseInterface = DBInterface(Logger, DBUname, DBPasswd, DBHost, DBName)


# Load SSH Keys #
KeysExist = CheckIfKeysExist(Logger)
if not KeysExist:
    PubKey, PrivateKey = GenKeys(Logger)
    WriteKeys(Logger, PubKey, PrivateKey)
PubKey, PrivateKey = ReadKeys(Logger)


# Check Dependencies #
ModulesNeeded = [
                'os',
                'yaml',
                'atexit',
                'inspect',
                'time',
                'datetime',
                'cpuinfo',
                'platform',
                'psutil',
                'GPUtil',
                'threading',
                'kazoo',
                'uuid',
                ]

CheckImports(ModulesNeeded, Logger)


# Initialize ZK #
Zookeeper = ZK(Logger)
Zookeeper.ConnectToZookeeper(Logger, ZKHost)
Zookeeper.AutoInitZKLeader()
Zookeeper.SpawnCheckerThread()

@atexit.register
def ShutdownZK():
    Zookeeper.Exit()


# Start System Telemetry #
TelemetryFollower = Follower(Logger=Logger, Zookeeper=Zookeeper)
TelemetryLeader = Leader(Logger=Logger, Zookeeper=Zookeeper) #<-- Note: This does NOT start it yet, you need to call start first. The manager handles this.
TelManager = SystemTelemetryManager(Zookeeper, TelemetryLeader)


# Start System #
Logger.Log('Starting BrainGenix Instance')
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


# Main Loop #
while True:

    # Execute System Tasks If In Leader Mode #
    TelManager.UpdateSysTel()


    time.sleep(0.5) # <-- Sleep for a polling interval to avoid excessive CPU usage
