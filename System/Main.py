###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

'''
Name: Main Loop
Description: This is the main file for a BrainGenix instance.
Date-Created: 2020-12-18
Date-Modified: 2020-12-21
'''

import atexit
import time

from Core.LoadConfig import LoadConfig
from Core.LoadAddons import LoadAddons
from Core.Logger import SysLog
from Core.CheckCoreImports import CheckImport, CheckImports

# Load Config #

ConfigPath = '../Config/Config.yaml'

AddonsPath, LogPath = LoadConfig(ConfigPath)


# Initialize Logger #

Logger = SysLog('0', LogPath) # NOTE: THE SYSLOG ID HERE NEEDS TO BE REPLACED WITH THE ZOOKEEPER ID LATER ON! (FIRST ARG)

Logger.Log('-----------------------------------------')
Logger.Log('-- Welcome To BrainGenix Version 0.0.1 --')
Logger.Log('-----------------------------------------')
Logger.Log('')
Logger.Log('')

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
                'threading'
                ]

CheckImports(ModulesNeeded, Logger)

# Load Addons #

Logger.Log('Initializing Addon Loading Process')

Plugins, Modules = LoadAddons(AddonsPath, Logger)

Logger.Log('Addon Loading Process Complete')


# Initialize Plugins #

Logger.Log('Initializing Plugins')

InitializationRegistry = {}

for PluginName, Plugin in Plugins.items():

    InitializationClassInstance = Plugin.Initialize(Logger)
    
    #atexit.register(InitializationClassInstance.AtExit())

    InitializationRegistry.update({PluginName : InitializationClassInstance})


# Start System #

Logger.Log('Starting BrainGenix Instance')


