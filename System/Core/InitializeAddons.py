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

        if hasattr(Plugin, 'Main'):
            ClassInstance = Plugin.Main(Logger=Logger)
            Registry.update({PluginName : ClassInstance})
        else:
            Logger.Log(f'Plugin {PluginName} Does Not Have A Main Class')
        

    return Registry


def InitializeModules(Modules:list, Logger:object):

    Registry = {}

    for ModuleName, Module in Modules.items():

        Logger.Log(f'Initializing Module: {ModuleName}')

        if hasattr(Module, 'Main'):
            ClassInstance = Module.Initialize(Logger=Logger)
            Registry.update({ModuleName : IClassInstance})
        else:
            Logger.Log(f'Module {ModuleName} Does Not Have A Main Class')
        

    return Registry


def InitializePluginRegistry(Registry:dict, Logger:object): # Passes The Registry To All The Plugins #


    # Convert Dict Into List #

    Logger.Log('Passing Registry To Plugins')

    PluginList = list(Registry.items())

    Logger.Log(f'Found {len(PluginList)} Plugins In Registry')


    # Pass Registry #

    for Plugin in PluginList:

        Logger.Log(f'Sending Registry To Plugin {Plugin[0]}')


        if hasattr(Plugin[1], 'GetPluginRegistry'):
            Plugin[1].GetPluginRegistry(Registry)
        else:
            Logger.Log(f'Plugin {Plugin[0]} Has No Attribute GetPluginRegistry', 1)

    Logger.Log('Finished Initializing Plugin Registry')
        