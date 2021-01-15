###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

'''
Name: InitializeAddons
Description: This system manages leader plugins.
Date-Created: 2020-12-18
'''

class LMPluginManager(): # Manages Leader Plugins, Starts Them When The System #

    def __init__(self, Logger:object, Plugins:dict, Zookeeper:object): # Sets Local Vars #

        # Set Local Vars #
        self.Logger = Logger
        self.UninitializedPlugins = Plugins
        self.Zookeeper = Zookeeper
        self.RegistryLeader = None
        self.ZKMode = 'Follower'


    def TransitionToLeaderMode(self): # Enable Leader Mode #

        self.RegistryLeader = {}

        for PluginName, Plugin in self.UninitializedPlugins.items():

            self.Logger.Log(f'Initializing Leader Mode Plugin: {PluginName}')

            if hasattr(Plugin, 'LeaderMain'):
                ClassInstance = Plugin.LeaderMain(Logger=self.Logger, Zookeeper=self.Zookeeper)
                self.RegistryLeader.update({PluginName : ClassInstance})
            else:
                self.Logger.Log(f'Plugin {PluginName} Does Not Have A LeaderMain Class')


        return self.RegistryLeader


    def TransitionToFollowerMode(self): # Disable Leader Mode #

        self.Logger.Log('Shutting Down Leader Mode Plugins')

        self.RegistryLeader = self.UninitializedPlugins

        self.Logger.Log('Plugins Exited Leader Mode')


    def CheckIfModeChange(self, CurrentZKStatus:str): # Check If ZK Mode Has Changed #

        if CurrentZKStatus != self.ZKMode:

            if CurrentZKStatus == 'Follower':
                self.TransitionToFollowerMode()
            elif CurrentZKStatus == 'Leader':
                self.TransitionToLeaderMode()

            self.ZKMode = CurrentZKStatus

        