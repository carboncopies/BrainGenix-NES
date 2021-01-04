###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

'''
Name: InitializeAddons
Description: This Set of functions calls the Initialize function and gets any issues back.
Date-Created: 2020-12-18
'''

def InitializePlugins(Plugins:list, Logger:object):

    Registry = {}

    for PluginName, Plugin in Plugins.items():

        Logger.Log(f'Initializing Plugin: {PluginName}')

        try:
            ClassInstance = Plugin.Main(Logger=Logger)
            Registry.update({PluginName : ClassInstance})
        except AttributeError: # Catch Exception from a plugin not using the init function #
            Logger.Log(f'Plugin {PluginName} Does Not Have A Main Class')
        

    return Registry


def InitializeModules(Modules:list, Logger:object):

    Registry = {}

    for ModuleName, Module in Modules.items():

        Logger.Log(f'Initializing Module: {ModuleName}')

        try:
            ClassInstance = Module.Initialize(Logger=Logger)
            Registry.update({ModuleName : IClassInstance})
        except AttributeError: # Catch Exception from a Module not using the init function #
            Logger.Log(f'Module {ModuleName} Does Not Have A Main Class')
        

    return Registry