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

from Core.CheckLibraries import CheckImports

from Zookeeper.ZKManager import SystemTelemetryManager

from API.ZookeeperPoller import PollWatcher

from Telemetry.SystemTelemetry import Follower
from Telemetry.SystemTelemetry import Leader

from Logger.CentralizedLogAggregation import CentralizedLoggerAggregationSystem


##############################################################################
## NOTE: A Lowercase "m" Preceeding A Class Means It's a Main System        ##
## NOTE: A Lowercase "s" Preceeding A Class Means It's a Subsystem          ##
##############################################################################



# Set Version Information
Version = '0.0.7'


# Load Config #
LogPath, PrintLogOutput, SecondsToKeepLogs = LoadLoggerConfig(ConfigFilePath = 'Config/LoggerConfig.yaml')
DBUname, DBPasswd, DBHost, DBName = LoadDatabaseConfig(ConfigFilePath = 'Config/DatabaseConfig.yaml')
ZKHost = LoadZookeeperConfig(ConfigFilePath = 'Config/ZookeeperConfig.yaml')
KafkaHost = LoadKafkaConfig(ConfigFilePath = 'Config/KafkaConfig.yaml')


# Initialize Logger #
mLogger = InstantiateLogger(DBUname, DBPasswd, DBHost, DBName, SecondsToKeepLogs, LogPath, PrintLogOutput)


# Initialize CLAS #
sCLAS = CentralizedLoggerAggregationSystem(mLogger)


# Purges The Log Buffer On System Exit #
@atexit.register
def CleanLog():
    mLogger.CleanExit()


# Connect To DB #
sDatabaseInterface = InstantiateDB(mLogger, DBUname, DBPasswd, DBHost, DBName)


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

CheckImports(ModulesNeeded, mLogger)


# Connect To Zookeeper Service #
sZookeeper = InstantiateZK(mLogger, ZKHost)

# Register Shutdown Function To Automatically Disconnect#
@atexit.register
def ShutdownZK():
    Zookeeper.Exit()


# Connect To Kafka Service #
#Kafka = InstantiateKafka(mLogger, KafkaHost)




# Start System Telemetry #
TelemetryFollower = Follower(Logger=mLogger, Zookeeper=sZookeeper)
TelemetryLeader = Leader(Logger=mLogger, Zookeeper=sZookeeper) #<-- Note: This does NOT start it yet, you need to call start first. The manager handles this.
TelManager = SystemTelemetryManager(sZookeeper, TelemetryLeader)


# Initialize The API ZK Watcher #
ZookeeperAPIWatcher = PollWatcher(mLogger, sZookeeper, TelemetryLeader)




# Start System #
mLogger.Log('Starting BrainGenix Instance')
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
mLogger.Log(f'    |               Welcome To BrainGenix Version {Version}               |')
mLogger.Log('    +-----------------------------------------------------------------+')
mLogger.Log('')

time.sleep(2)


# Main Loop #
while True:

    # Execute System Tasks If In Leader Mode #
    TelManager.UpdateSysTel()


    time.sleep(0.5) # <-- Sleep for a polling interval to avoid excessive CPU usage
