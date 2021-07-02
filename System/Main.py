###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

'''
Name: Main Loop
Description: This is the main file for a BrainGenix instance.
Date-Created: 2020-12-18
'''


from Core.Initialization.LoadConfig import LoadLocalConfig, LoadLoggerConfig
from Core.Initialization.LoadConfig import LoadDatabaseConfig
from Core.Initialization.LoadConfig import LoadZookeeperConfig
from Core.Initialization.LoadConfig import LoadInternodeQueueConfig
from Core.Initialization.LoadConfig import LoadManagementAPIServerConfig
from Core.Initialization.LoadConfig import LoadLocalConfig

from Core.ThreadManager import ThreadManager

from Core.Initialization.Instantiator import InstantiateLogger

from Core.Initialization.CheckLibraries import CheckImports

from Core.Management.Logger.CLAS import CentralizedLoggerAggregationSystem

from Core.Management.API.ManagementAPI import ManagementAPISocketServer

from Core.Internode.Zookeeper.LFTransitionManager import LFTM

from Core.VersionData import VersionNumber
from Core.VersionData import BranchVersion


##############################################################################
## NOTE: A Lowercase "m" Preceeding A Class Means It's a Main System        ##
## NOTE: A Lowercase "s" Preceeding A Class Means It's a Subsystem          ##
##############################################################################



# Set Version Information
Version = VersionNumber
Branch = BranchVersion


# Load Config #
SystemConfiguration = LoadLocalConfig(ConfigFilePath = 'BrainGenix-NES-Config.yaml')
LoggerConfigDict = LoadLoggerConfig(ConfigFilePath = 'Config/LoggerConfig.yaml')
DBConfigDict = LoadDatabaseConfig(ConfigFilePath = 'Config/DatabaseConfig.yaml')
ZKConfigDict = LoadZookeeperConfig(ConfigFilePath = 'Config/ZookeeperConfig.yaml')
KafkaConfigDict = LoadInternodeQueueConfig(ConfigFilePath = 'Config/InternodeQueue.yaml')
ManagementAPIServerConfig = LoadManagementAPIServerConfig(ConfigFilePath = 'Config/ManagementAPIConfig.yaml')


# Initialize Logger #
mLogger = InstantiateLogger(DBConfigDict, LoggerConfigDict)


# Instantiate Thread Manager #
mThreadManagerInstance = ThreadManager(mLogger)


# Initialize CLAS #
sCLAS = CentralizedLoggerAggregationSystem(mLogger)


# Connect To DB #
sDatabaseInterface = mThreadManagerInstance.InstantiateDB(mLogger, DBConfigDict)


# Start API Server #
sSocketAPI = ManagementAPISocketServer(mLogger, ManagementAPIServerConfig, ZKConfigDict, DBConfigDict, mThreadManagerInstance)


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
sZookeeper = mThreadManagerInstance.InstantiateZK(mLogger, ZKConfigDict)


##############################################################################################################
## THIS WILL CONNECT TO THE C++ INTERFACE WITH BRIAN HERE. THE HEAVY PROCESING WORK IS DONE ON THE C++ SIDE ##
##############################################################################################################


# Get NodeCount #
NodeCount = sZookeeper.ConcurrentConnectedNodes()


# Get API Server Count #
APIServerCount = len(sZookeeper.ZookeeperConnection.get_children('/BrainGenix/API/Connections'))

# Instantiate Leader/Follower Transition Manager #
sLFTMInstance = LFTM(mLogger, sZookeeper, sSocketAPI, mThreadManagerInstance, KafkaConfigDict)

# Link LFTM #
sSocketAPI.LinkLFTM(sLFTMInstance)
sSocketAPI.IndexCommands()

sSocketAPI.UpdateCommand()
sSocketAPI.WriteAuthentication()


# MOTD #
mLogger.Log('Starting BrainGenix Instance', 7)
mLogger.Log('', 5)
mLogger.Log('---------------------------------------------------------------------------', 5)
mLogger.Log('\x1b[38;2;0;128;55m██████╗ ██████╗  █████╗ ██╗███╗   ██╗\x1b[38;2;130;68;208m ██████╗ ███████╗███╗   ██╗██╗██╗  ██╗', 5)
mLogger.Log('\x1b[38;2;0;128;55m██╔══██╗██╔══██╗██╔══██╗██║████╗  ██║\x1b[38;2;130;68;208m██╔════╝ ██╔════╝████╗  ██║██║╚██╗██╔╝', 5)
mLogger.Log('\x1b[38;2;0;128;55m██████╔╝██████╔╝███████║██║██╔██╗ ██║\x1b[38;2;130;68;208m██║  ███╗█████╗  ██╔██╗ ██║██║ ╚███╔╝ ', 5)
mLogger.Log('\x1b[38;2;0;128;55m██╔══██╗██╔══██╗██╔══██║██║██║╚██╗██║\x1b[38;2;130;68;208m██║   ██║██╔══╝  ██║╚██╗██║██║ ██╔██╗ ', 5)
mLogger.Log('\x1b[38;2;0;128;55m██████╔╝██║  ██║██║  ██║██║██║ ╚████║\x1b[38;2;130;68;208m╚██████╔╝███████╗██║ ╚████║██║██╔╝ ██╗', 5)
mLogger.Log('\x1b[38;2;0;128;55m╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═╝╚═╝╚═╝  ╚═══╝\x1b[38;2;130;68;208m ╚═════╝ ╚══════╝╚═╝  ╚═══╝╚═╝╚═╝  ╚═╝', 5)
mLogger.Log('---------------------------------------------------------------------------', 5)
mLogger.Log('', 5)
mLogger.Log('    +-----------------------------------------------------------------+', 4)
mLogger.Log('    |     BrainGenix-NES Biological Neuron Simulation System          |', 4)
mLogger.Log(f'    |     Version: {Version}                                              |', 4)
mLogger.Log(f'    |     Branch: {Branch}                                                 |', 4)
mLogger.Log(f'    |     Clustersize: {NodeCount}                                              |', 4)
mLogger.Log('    +-----------------------------------------------------------------+', 4)
mLogger.Log('', 4)


# Start System #
sLFTMInstance.MainLoop()
