###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

import importlib

'''
Name: CheckCoreImports
Description: This Function Checks For Core Modules Needed For The System, and stop execution before the problem results in a module not found error.
Date-Created: 2020-12-19
Date-Modified: 2020-12-21
'''

def CheckLibrary(ModuleName:str, Logger:object):

    Logger.Log(f'Checking Module: {ModuleName}')


    # Check Module #

    ModuleLoader = importlib.util.find_spec(ModuleName)

    if ModuleLoader == None:

        Logger.Log(f'Module "{ModuleName}" Not Found, Try Installing It With "pip3 install [ModuleName]"')
        exit()

def CheckImports(ModuleList:list, Logger:object):

    # Check List Of Modules #

    Logger.Log('Checking Module Dependencies')

    for Module in ModuleList:

        CheckLibrary(Module, Logger)

    Logger.Log('Module Check Completed, No Issues Found')