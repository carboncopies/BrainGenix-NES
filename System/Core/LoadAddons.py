###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

import importlib
import os

def LoadAddons(Path:str, Logger):

    '''
    Name: LoadAddons
    Description: This function loads Addons from the Network Drive that it's pointed to.
    Date-Created: 2020-12-18
    Date-Modified: 2020-12-20
    '''


    # First, Load Plugins #

    Logger.Log('Getting List Of Plugins')

    PluginFiles = os.listdir(Path + 'Plugins')

    try:
        PluginFiles.remove('__init__.py')
        PluginFiles.remove('__pycache__')
    except ValueError: # Catch Exception Caused By Files Not Existing #
        pass


    PluginObjects = {}

    Logger.Log(f'Found {len(PluginFiles)} Plugin(s)')

    for PluginName in PluginFiles:

        Spec = importlib.util.spec_from_file_location(PluginName, Path + f'Plugins/{PluginName}')
        PluginObject = importlib.util.module_from_spec(Spec)
        Spec.loader.exec_module(PluginObject)

        PluginObjects.update({PluginName.split(".")[0] : PluginObject})

        Logger.Log(f'Loaded Plugin [{PluginName.split(".")[0]}]')


    # Next, Load The Modules #

    Logger.Log('Getting List Of Modules')

    ModuleFiles = os.listdir(Path + 'Modules')
    
    try:
        ModuleFiles.remove('__init__.py')
        ModuleFiles.remove('__pycache__')
    except ValueError: # Catch Exception Caused By Files Not Existing #
        pass


    ModuleObjects = {}

    Logger.Log(f'Found {len(ModuleFiles)} Module(s)')

    for ModuleName in ModuleFiles:

        Spec = importlib.util.spec_from_file_location(ModuleName, Path + f'Modules/{ModuleName}')
        ModuleObject = importlib.util.module_from_spec(Spec)
        Spec.loader.exec_module(ModuleObject)

        ModuleObjects.update({ModuleName.split(".")[0] : ModuleObject})

        Logger.Log(f'Loaded Module [{ModuleName.split(".")[0]}]')


    # Return The Loaded Objects #

    return PluginObjects, ModuleObjects