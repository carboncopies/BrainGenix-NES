###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

'''
Name: InitializeAddons
Description: This Set of functions calls the Initialize function and gets any issues back.
Date-Created: 2020-12-18
'''

def InitializePlugins(Plugins:list, Logger:object):

    InitializationRegistry = {}

    for PluginName, Plugin in Plugins.items():

        Logger.Log(f'Initializing Plugin: {PluginName}')

        try:
            InitializationClassInstance = Plugin.Initialize(Logger)
            InitializationRegistry.update({PluginName : InitializationClassInstance})
        except AttributeError: # Catch Exception from a plugin not using the init function #
            Logger.Log(f'Plugin {PluginName} Does Not Have An Initialization Class')
        

    return InitializationRegistry


def InitializeModules(Modules:list, Logger:object):

    InitializationRegistry = {}

    for ModuleName, Module in Modules.items():

        Logger.Log(f'Initializing Module: {ModuleName}')

        try:
            InitializationClassInstance = Module.Initialize(Logger)
            InitializationRegistry.update({ModuleName : InitializationClassInstance})
        except AttributeError: # Catch Exception from a Module not using the init function #
            Logger.Log(f'Module {ModuleName} Does Not Have An Initialization Class')
        

    return InitializationRegistry