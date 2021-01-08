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
from Core.LoadAddons import LoadAddons, CheckDependencies
from Core.Logger import SysLog
from Core.CheckLibraries import CheckLibrary, CheckImports
from Core.InitializeAddons import InitializeFollowerPlugins, InitializeLeaderPlugins, InitializeModules, InitPluginRegistry, InitModuleRegistry, InitLeadPluginReg

from Zookeeper.Zookeeper import ZK

# Load Config #

ConfigPath = 'Config/LocalConfig.yaml'

AddonsPath, LogPath, BufferLength, PrintLogOutput, LinesPerFile, EnableGzip, ZKHost = LoadConfig(ConfigPath)


# Initialize Logger #

Logger = SysLog('0', LogPath, BufferLength=BufferLength, LogSegmentLength=LinesPerFile, ConsoleOutputEnabled=PrintLogOutput, EnableGzip = EnableGzip) # NOTE: THE SYSLOG ID HERE NEEDS TO BE REPLACED WITH THE ZOOKEEPER ID LATER ON! (FIRST ARG)


# Purges The Log Buffer On System Exit #
@atexit.register
def CleanLog(): 

    Logger.PurgeBuffer()
    Logger.CleanExit()


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


# Load Addons #
Plugins, Modules = LoadAddons(AddonsPath, Logger)
CheckDependencies(Plugins, Modules, Logger)


# Initialize Plugins #
FollowerRegistry = InitializeFollowerPlugins(Plugins, Logger, Zookeeper)
LeaderRegistry = InitializeLeaderPlugins(Plugins, Logger, Zookeeper)
InitPluginRegistry(FollowerRegistry, Logger)
#InitLeadPluginReg(FollowerRegistry, LeaderRegistry, Logger)


# Start System #
Logger.Log('Starting BrainGenix Instance')
Logger.Log('-----------------------------------------')
Logger.Log('-- Welcome To BrainGenix Version 0.0.2 --')
Logger.Log('-----------------------------------------')


