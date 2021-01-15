###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

'''
Name: InitializeAddons
Description: This Set of functions calls the Initialize function and gets any issues back.
Date-Created: 2020-12-18
'''

def InitFollowerPlugins(Plugins:list, Logger:object, Zookeeper:object):

    RegistryFollower = {}

    for PluginName, Plugin in Plugins.items():

        Logger.Log(f'Initializing Follower Mode Plugin: {PluginName}')

        if hasattr(Plugin, 'FollowerMain'):
            ClassInstance = Plugin.FollowerMain(Logger=Logger, Zookeeper=Zookeeper)
            RegistryFollower.update({PluginName : ClassInstance})
        else:
            Logger.Log(f'Plugin {PluginName} Does Not Have A FollowerMain Class')


    return RegistryFollower


def InitLeaderPlugins(Plugins:list, Logger:object, Zookeeper:object):


    RegistryLeader = {}

    for PluginName, Plugin in Plugins.items():

        Logger.Log(f'Initializing Leader Mode Plugin: {PluginName}')

        if hasattr(Plugin, 'LeaderMain'):
            ClassInstance = Plugin.LeaderMain(Logger=Logger, Zookeeper=Zookeeper)
            RegistryLeader.update({PluginName : ClassInstance})
        else:
            Logger.Log(f'Plugin {PluginName} Does Not Have A LeaderMain Class')


    return RegistryLeader


def InitializeModules(Modules:list, Logger:object, Zookeeper:object):

    Registry = {}

    for ModuleName, Module in Modules.items():

        Logger.Log(f'Initializing Module: {ModuleName}')

        if hasattr(Module, 'Main'):
            ClassInstance = Module.LeaderMain(Logger=Logger, Zookeeper=Zookeeper)
            Registry.update({ModuleName : IClassInstance})
        else:
            Logger.Log(f'Module {ModuleName} Does Not Have A Main Class')
        

    return Registry


def InitPluginRegistry(Registry:dict, Logger:object): # Passes The Registry To All The Plugins #


    # Convert Dict Into List #
    PluginList = list(Registry.items())

    Logger.Log(f'Found {len(PluginList)} Plugins In Follower Registry')


    # Pass Registry #
    for Plugin in PluginList:

        Logger.Log(f'Sending Registry To Plugin {Plugin[0]}')


        if hasattr(Plugin[1], 'GetPluginRegistry'):
            Plugin[1].GetPluginRegistry(Registry)
        else:
            Logger.Log(f'Plugin {Plugin[0]} Has No Attribute GetPluginRegistry', 1)


def InitLeadPluginReg(FollowerRegistry:dict, LeaderRegistry:dict, Logger:object): # Passes The Registry To All The Plugins #


    # Convert Dict Into List #
    PluginList = list(LeaderRegistry.items())

    Logger.Log(f'Found {len(PluginList)} Plugins In Leader Registry')


    # Pass Registry #
    for Plugin in PluginList:

        Logger.Log(f'Sending Registry To Plugin {Plugin[0]}')


        if hasattr(Plugin[1], 'GetPluginRegistry'):
            Plugin[1].GetPluginRegistry(FollowerRegistry, LeaderRegistry)
        else:
            Logger.Log(f'Plugin {Plugin[0]} Has No Attribute GetPluginRegistry', 1)
        

def InitModuleRegistry(Registry:dict, Logger:object): # Passes The Registry To All The Modules #


    # Convert Dict Into List #

    Logger.Log('Passing Registry To Modules')

    ModuleList = list(Registry.items())

    Logger.Log(f'Found {len(ModuleList)} Modules In Registry')


    # Pass Registry #

    for Module in ModuleList:

        Logger.Log(f'Sending Registry To Module {Module[0]}')


        if hasattr(Module[1], 'GetModuleRegistry'):
            Module[1].GetModuleRegistry(Registry)
        else:
            Logger.Log(f'Module {Module[0]} Has No Attribute GetModuleRegistry', 1)

    Logger.Log('Finished Initializing Module Registry')