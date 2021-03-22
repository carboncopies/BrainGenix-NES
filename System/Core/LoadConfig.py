###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

import yaml

'''
Name: LoadConfig
Description: This function loads and parses the system config file which tells it where to look for plugins, etc.
Date-Created: 2020-12-18
'''


def LoadLoggerConfig(ConfigFilePath):

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


    # Extract the important values from the dictionary and return them to the main system #
    LogPath = str(ConfigFileDictionary.get('LogPath'))
    PrintLogText = bool(ConfigFileDictionary.get('PrintLogText'))
    LogBufferLength = int(ConfigFileDictionary.get('LogBufferLength'))
    LogFileLinesBeforeSplit = int(ConfigFileDictionary.get('LogFileLinesBeforeSplit'))
    EnableGzip = bool(ConfigFileDictionary.get('gzipLogs'))

    # Return the values #
    return LogPath, LogBufferLength, PrintLogText, LogFileLinesBeforeSplit, EnableGzip



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

    # Extract Values From Dictionary #
    DBUname = str(ConfigFileDictionary.get('DatabaseUsername'))
    DBPasswd = str(ConfigFileDictionary.get('DatabasePassword'))
    DBHost = str(ConfigFileDictionary.get('DatabaseHost'))
    DBName = str(ConfigFileDictionary.get('DatabaseName'))

    return DBUname, DBPasswd, DBHost, DBName


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

    # Extract Values From Dictionary #
    ZKHost = str(ConfigFileDictionary.get('ZKHost'))
    ZKPort = str(ConfigFileDictionary.get('ZKPort'))

    ZKHost += f':{ZKPort}'

    return ZKHost


def LoadKafkaConfig(ConfigFilePath):

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

    # Extract Values From Dictionary #
    KafkaHost = str(ConfigFileDictionary.get('KafkaHost'))
    KafkaPort = str(ConfigFileDictionary.get('KafkaPort'))

    KafkaHost += f':{KafkaPort}'

    return KafkaHost