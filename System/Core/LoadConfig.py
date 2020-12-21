###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

import yaml

def LoadConfig(ConfigFilePath):

    '''
    Name: LoadConfig
    Description: This function loads and parses the system config file which tells it where to look for plugins, etc.
    Date-Created: 2020-12-18
    Date-Modified: 2020-12-20
    '''

    # First Read the contents of the file and parse it #

    with open(ConfigFilePath, 'r') as File:
        ConfigFileContents = File.read()

    ConfigFileDictionary = yaml.load(ConfigFileContents)

    
    # Extract the important values from the dictionary and return them to the main system #

    AddonsPath = ConfigFileDictionary.get('AddonsPath')
    LogPath = ConfigFileDictionary.get('LogPath')

    # Return the values #

    return AddonsPath, LogPath