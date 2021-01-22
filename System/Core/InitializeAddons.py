###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

'''
Name: InitializeAddons
Description: This Set of functions calls the Initialize function and gets any issues back.
Date-Created: 2020-12-18
'''

def InitFollowerPlugins(Plugins:list, Logger:object, Zookeeper:object):

    '''
    This function initializes all of the plugins' follower classes.
    These are run by every braingenix node.
    This function returns a registry which is a dictionary containing the initialized follower plugin classes.
    *Please don't call this unless you know what you're doing!*
    '''


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

    '''
    This function does essentially the same thing as the follower plugin init function, but to leader plugins.
    The function is called when a node transitions from follower to leader.
    *Please don't call this unless you know what you're doing!*
    '''

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

    '''
    This function initializes the modules. 
    Modules are only used by leaders, and so this function is only called when a node transitions to leader mode from follower mode.
    *Please don't call this unless you know what you're doing!*
    '''

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

    '''
    This function initializes the plugin registry.
    It pushes a pointer of the follower plugin registry to all the plugins so that they can communicate with eachother.
    *Please don't call this unless you know what you're doing!*
    '''


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

    '''
    This function initializes the plugin registry.
    It pushes a pointer of the leader plugin registry to all the plugins so that they can communicate with eachother.
    *Please don't call this unless you know what you're doing!*
    '''


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

    '''
    This function initializes the plugin registry.
    It pushes a pointer of the modules' registry to all the other modules so that they can communicate with eachother.
    *Please don't call this unless you know what you're doing!*
    '''

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