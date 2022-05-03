###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

import yaml

'''
Name: LoadConfig
Description: This function loads and parses the system config file which tells it where to look for plugins, etc.
Date-Created: 2020-12-18
'''

def LoadLocalConfig(ConfigFilePath):

    '''
    This function loads the local configuration file from the system folder.
    '''

    # First Read the contents of the file and parse it #
    with open(ConfigFilePath, 'r') as File:
        ConfigFileDictionary = yaml.safe_load(File)

    # Return the values #
    return ConfigFileDictionary


def LoadLoggerConfig(ConfigFilePath):

    '''
    This function loads the config from a file path.
    It's designed to read a specific set of values from the path, so it doesn't just return a dictionary value.
    This loader is not designed to be used except as the initial config loader, as it only reads a few select parameters.
    *Please don't call this function unless you know what you're doing!*
    '''

    # First Read the contents of the file and parse it #
    with open(ConfigFilePath, 'r') as File:
        ConfigFileDictionary = yaml.safe_load(File)

    # Return the values #
    return ConfigFileDictionary


def LoadDatabaseConfig(ConfigFilePath):

    '''
    This function loads the config from a file path.
    It's designed to read a specific set of values from the path, so it doesn't just return a dictionary value.
    This loader is not designed to be used except as the initial config loader, as it only reads a few select parameters.
    *Please don't call this function unless you know what you're doing!*
    '''

    # First Read the contents of the file and parse it #
    with open(ConfigFilePath, 'r') as File:
        ConfigFileContents = File.read()

    ConfigFileDictionary = yaml.safe_load(ConfigFileContents)


    # Return Config Data #
    return ConfigFileDictionary


def LoadZookeeperConfig(ConfigFilePath):

    '''
    This function loads the config from a file path.
    It's designed to read a specific set of values from the path, so it doesn't just return a dictionary value.
    This loader is not designed to be used except as the initial config loader, as it only reads a few select parameters.
    *Please don't call this function unless you know what you're doing!*
    '''

    # First Read the contents of the file and parse it #
    with open(ConfigFilePath, 'r') as File:
        ConfigFileContents = File.read()

    ConfigFileDictionary = yaml.safe_load(ConfigFileContents)


    # Return Output Config File #
    return ConfigFileDictionary


def LoadInternodeQueueConfig(ConfigFilePath):

    '''
    This function loads the config from a file path.
    It's designed to read a specific set of values from the path, so it doesn't just return a dictionary value.
    This loader is not designed to be used except as the initial config loader, as it only reads a few select parameters.
    *Please don't call this function unless you know what you're doing!*
    '''

    # First Read the contents of the file and parse it #
    with open(ConfigFilePath, 'r') as File:
        ConfigFileContents = File.read()

    ConfigFileDictionary = yaml.safe_load(ConfigFileContents)


    # Return Output #
    return ConfigFileDictionary



def LoadManagementAPIServerConfig(ConfigFilePath):

    '''
    This function loads the config from a file path.
    It's designed to read a specific set of values from the path, so it doesn't just return a dictionary value.
    This loader is not designed to be used except as the initial config loader, as it only reads a few select parameters.
    *Please don't call this function unless you know what you're doing!*
    '''

    # First Read the contents of the file and parse it #
    with open(ConfigFilePath, 'r') as File:
        ConfigFileContents = File.read()

    ConfigFileDictionary = yaml.safe_load(ConfigFileContents)


    # Return Output #
    return ConfigFileDictionary
