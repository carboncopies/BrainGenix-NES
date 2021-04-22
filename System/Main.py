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


from Core.LoadConfig import LoadLoggerConfig
from Core.LoadConfig import LoadDatabaseConfig
from Core.LoadConfig import LoadZookeeperConfig
from Core.LoadConfig import LoadKafkaConfig

from Core.Instantiator import InstantiateZK
from Core.Instantiator import InstantiateKafka
from Core.Instantiator import InstantiateDB
from Core.Instantiator import InstantiateLogger

from Core.Logger import SysLog
from Core.CheckLibraries import CheckImports

from Zookeeper.ZKManager import SystemTelemetryManager

from API.ZookeeperPoller import PollWatcher

from Telemetry.SystemTelemetry import Follower
from Telemetry.SystemTelemetry import Leader


# Set Version Information
Version = '0.0.6'


# Load Config #
LogPath, PrintLogOutput, SecondsToKeepLogs = LoadLoggerConfig(ConfigFilePath = 'Config/LoggerConfig.yaml')
DBUname, DBPasswd, DBHost, DBName = LoadDatabaseConfig(ConfigFilePath = 'Config/DatabaseConfig.yaml')
ZKHost = LoadZookeeperConfig(ConfigFilePath = 'Config/ZookeeperConfig.yaml')
KafkaHost = LoadKafkaConfig(ConfigFilePath = 'Config/KafkaConfig.yaml')


# Initialize Logger #
Logger = InstantiateLogger(DBUname, DBPasswd, DBHost, DBName, LineRetentionCount, LogPath, PrintLogOutput)


# Purges The Log Buffer On System Exit #
@atexit.register
def CleanLog():
    Logger.CleanExit()


# Connect To DB #
DatabaseInterface = InstantiateDB(Logger, DBUname, DBPasswd, DBHost, DBName)


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


# Connect To Zookeeper Service #
Zookeeper = InstantiateZK(Logger, ZKHost)

# Register Shutdown Function To Automatically Disconnect#
@atexit.register
def ShutdownZK():
    Zookeeper.Exit()


# Connect To Kafka Service #
#Kafka = InstantiateKafka(Logger, KafkaHost)




# Start System Telemetry #
TelemetryFollower = Follower(Logger=Logger, Zookeeper=Zookeeper)
TelemetryLeader = Leader(Logger=Logger, Zookeeper=Zookeeper) #<-- Note: This does NOT start it yet, you need to call start first. The manager handles this.
TelManager = SystemTelemetryManager(Zookeeper, TelemetryLeader)


# Initialize The API ZK Watcher #
ZookeeperAPIWatcher = PollWatcher(Logger, Zookeeper, TelemetryLeader)




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

time.sleep(2)
a = Logger.PullSort(1000)
print(len(a['bg-turing-0']))
Logger.PurgeOldLogs()
print(len(a['bg-turing-0']))

# Main Loop #
while True:

    # Execute System Tasks If In Leader Mode #
    TelManager.UpdateSysTel()


    time.sleep(0.5) # <-- Sleep for a polling interval to avoid excessive CPU usage
