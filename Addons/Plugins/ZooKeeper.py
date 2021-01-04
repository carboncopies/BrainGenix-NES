###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################



'''
Name: ZooKeeper Interface
Description: This plugin handles interfacing to the zookeeper cluster.
Date-Created: 2021-1-3
'''


class AddonInfo(): # All Addons contain this class, and it tells the system what dependencies this module has #

    def __init__(self):

        self.Version = '0.0.1'
        self.Author = 'BrainGenix Team'
        self.Dependencies = []

class Main(): # Main Class #

    def __init__(self, **kwargs):

        pass


