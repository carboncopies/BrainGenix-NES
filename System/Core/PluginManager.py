###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

'''
Name: InitializeAddons
Description: This system manages leader plugins.
Date-Created: 2020-12-18
'''

class LMPluginManager(): # Manages Leader Plugins, Starts Them When The System #

    '''
    This class is used for handling the switching between leader and follower mode.
    The class has to be able to dynamically switch all the plugins to and from leader mode.
    This used to be handled by the zookeeper file, however it's been moved to it's own file due to complexity of the system.
    *Please don't use this unless you know what you're doing!*
    '''

    def __init__(self, Logger:object, Plugins:dict, Zookeeper:object): # Sets Local Vars #

        '''
        Runs initialization, creates local pointers to important variables such as the logger object.
        *Please don't use this unless you know what you're doing!*
        '''

        # Set Local Vars #
        self.Logger = Logger
        self.UninitializedPlugins = Plugins
        self.Zookeeper = Zookeeper
        self.RegistryLeader = None
        self.ZKMode = 'Follower'


    def TransitionToLeaderMode(self): # Enable Leader Mode #

        '''
        This function is called when zookeeper transitions the local node from follower to leader mode.
        It loops through the leader plugin registry and instantiates all of the classes, starting them.
        *Please don't use this unless you know what you're doing!*
        '''

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

        '''
        This function is called when zookeeper transitions the system from leader to follower mode.
        This could happen if the leader node looses connection for a while and is reconnected to find a new leader has been elected.
        It essentially does the opposit eof the TransitionToLeaderMode() function.
        *Please don't use this unless you know what you're doing!*
        '''

        self.Logger.Log('Shutting Down Leader Mode Plugins')

        self.RegistryLeader = self.UninitializedPlugins

        self.Logger.Log('Plugins Exited Leader Mode')


    def CheckIfModeChange(self, CurrentZKStatus:str): # Check If ZK Mode Has Changed #

        '''
        This function is used to check if the current mode has changed, and calls the appropriate function if it has.
        *Please don't use this unless you know what you're doing!*
        '''

        if CurrentZKStatus != self.ZKMode:

            if CurrentZKStatus == 'Follower':
                self.TransitionToFollowerMode()
            elif CurrentZKStatus == 'Leader':
                self.TransitionToLeaderMode()

            self.ZKMode = CurrentZKStatus

        