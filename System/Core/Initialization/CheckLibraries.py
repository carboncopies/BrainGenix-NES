###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

import importlib

'''
Name: CheckCoreImports
Description: This Function Checks For Core Modules Needed For The System, and stop execution before the problem results in a module not found error.
Date-Created: 2020-12-19
'''

def CheckLibrary(ModuleName:str, Logger:object):

    '''
    This function checks if a python module/library exists.
    If the library does not exist, then it asks the user to install it, then exits the program.
    *Please don't use this function unless you know what you're doing!*
    '''

    #Logger.Log(f'Checking Module: {ModuleName}')


    # Check Module #

    ModuleLoader = importlib.util.find_spec(ModuleName)

    if ModuleLoader == None:

        Logger.Log(f'Module "{ModuleName}" Not Found, Try Installing It With "pip3 install [ModuleName]"')
        exit()

def CheckImports(ModuleList:list, Logger:object):

    '''
    This function checks a list of imports by calling the checklibrary function.
    *Please don't use this function unless you know what you're doing!*
    '''

    # Check List Of Modules #

    #Logger.Log('Checking Module Dependencies')

    for Module in ModuleList:

        CheckLibrary(Module, Logger)

    Logger.Log('Python Module Check Completed, No Issues Found')