###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################



'''
Name: MultiProcessing
Description: This plugin handles multithreading needed to make brian2 use multiple cores on the same system.
Date-Created: 2020-12-19
Date-Modified: 2020-12-21
'''


class AddonInfo(): # All Addons contain this class, and it tells the system what dependencies this module has #

    def __init__(self):

        self.Version = '0.0.1'
        self.Author = 'BrainGenix Team'
        self.Dependencies = [('SystemInfo', '>', '0.0.1')]