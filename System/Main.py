###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

'''
Name: Main Loop
Description: This is the main file for a BrainGenix instance.
Date-Created: 2020-12-18
'''


from Core.Initialization.LoadConfig import LoadLocalConfig
from Core.Initialization.Instantiator import InstantiateLogger
from Core.Initialization.CheckLibraries import CheckImports
from Core.Initialization.ThreadManager import ThreadManager

from Core.Management.API.ManagementAPI import ManagementAPISocketServer

from Core.MainLoop.MainLoop import LFTM

from Core.MainSimulationEngine.MainSimulationEngine import MainSimulationEngine

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
SystemConfiguration = LoadLocalConfig(ConfigFilePath = 'Config.yaml')


# Initialize Logger #
mLogger = InstantiateLogger(SystemConfiguration)


# Instantiate Thread Manager #
mThreadManagerInstance = ThreadManager(mLogger)


# Connect To DB #
# = mThreadManagerInstance.InstantiateDB(mLogger, SystemConfiguration)


# Start API Server #
sSocketAPI = ManagementAPISocketServer(mLogger, SystemConfiguration, mThreadManagerInstance)


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
sZookeeper = mThreadManagerInstance.InstantiateZK(mLogger, SystemConfiguration)


##############################################################################################################
## THIS WILL CONNECT TO THE C++ INTERFACE WITH BRIAN HERE. THE HEAVY PROCESING WORK IS DONE ON THE C++ SIDE ##
##############################################################################################################


# Get NodeCount #
NodeCount = sZookeeper.ConcurrentConnectedNodes()


# Instantiate Leader/Follower Transition Manager #
sLFTMInstance = LFTM(mLogger, sZookeeper, sSocketAPI, mThreadManagerInstance, SystemConfiguration)

# Link LFTM #
sSocketAPI.LinkLFTM(sLFTMInstance)
sSocketAPI.IndexCommands()

sSocketAPI.UpdateCommand()
#sSocketAPI.addUser('pray@carboncopies.org','123456','789','Prishita','Ray','', 5)
#sSocketAPI.WriteAuthentication('pray@carboncopies.org','123456')

# Instantiate Main Simulation Engine #
sMainSimulationEngine = MainSimulationEngine(mLogger, SystemConfiguration)


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
