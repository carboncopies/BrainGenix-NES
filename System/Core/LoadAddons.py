###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

import importlib
import os

from Core.CheckLibraries import CheckImports, CheckLibrary

def LoadAddons(Path:str, Logger:object):

    '''
    Name: LoadAddons
    Description: This function loads Addons from the Network Drive that it's pointed to.
    Date-Created: 2020-12-18
    Date-Modified: 2020-12-26
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


def CheckDependencies(Plugins:list, Modules:list, Logger:object):

    '''
    Name: CheckDependencies
    Description: This function checks plugin dependencies.
    Date-Created: 2020-12-26
    Date-Modified: 2020-12-26
    '''

    # Get Addon Info Classes #

    Logger.Log('Getting Addon Info')

    PluginInfoClasses = []
    PluginNames = []
    ModuleInfoClasses = []
    ModuleNames = []

    for Plugin in list(Plugins.items()):
        
        Logger.Log(f'Getting Plugin Info For Plugin: {Plugin[0]}')

        PluginNames.append(Plugin[0])

        try:
            PluginInfo = Plugin[1].AddonInfo()
            PluginInfoClasses.append(PluginInfo)
            #Logger.Log('Successfully Got Addon Info')
        except AttributeError: # Catch Exception Thrown If Addon Does Not Have An AddonInfo Class #
            Logger.Log(f'Plugin {Plugin[0]} Does Not Have An AddonInfo Class, This May Result In Undefined Behavior, And This Addon Will NOT Satisfy Others Dependencies!', 2)
            PluginInfoClasses.append(None)
    
    
    for Module in list(Modules.items()):

        Logger.Log(f'Getting Plugin Info For Module: {Module[0]}')

        ModuleNames.append(Module[0])

        try:
            ModuleInfo = Module[1].AddonInfo()
            ModuleInfoClasses.append(ModuleInfo)
            #Logger.Log('Successfully Got Addon Info')
        except AttributeError: # Catch Exception Thrown If Addon Does Not Have An AddonInfo Class #
            Logger.Log(f'Module {Module[0]} Does Not Have An AddonInfo Class, This May Result In Undefined Behavior, And This Addon Will NOT Satisfy Others Dependencies!', 2)
            PluginInfoClasses.append(None)

    Logger.Log('Done Getting Addon Metadata')


    # Sort Addon Info Data #

    PluginVersions = []
    PluginAuthors = []
    PluginDependencies = []
    PluginImports = []
    
    ModuleVersions = []
    ModuleAuthors = []
    ModuleDependencies = []
    ModuleImports = []


    for PluginInfoIndex in range(len(PluginInfoClasses)):

        PluginInfo = PluginInfoClasses[PluginInfoIndex]

        try:
            PluginVersions.append(PluginInfo.Version)
        except AttributeError:
            PluginVersions.append('No Data Available')
            Logger.Log(f'Plugin {list(Plugins.items())[PluginInfoIndex][0]} Is Missing "Version" Information', 2)

        try:
            PluginAuthors.append(PluginInfo.Author)
        except AttributeError:
            PluginAuthors.append('No Data Available')
            Logger.Log(f'Plugin {list(Plugins.items())[PluginInfoIndex][0]} Is Missing "Author" Information', 2)
        
        try:
            PluginDependencies.append(PluginInfo.Dependencies)
        except AttributeError:
            PluginDependencies.append('No Data Available')
            Logger.Log(f'Plugin {list(Plugins.items())[PluginInfoIndex][0]} Is Missing "Dependencies" Information', 2)

        try:
            PluginImports.append(PluginInfo.Imports)
        except AttributeError:
            PluginImports.append('No Data Available')
            Logger.Log(f'Plugin {list(Plugins.items())[PluginInfoIndex][0]} Is Missing "Imports" Information', 2)


    for ModuleInfoIndex in range(len(ModuleInfoClasses)):

        ModuleInfo = ModuleInfoClasses[ModuleInfoIndex]

        try:
            ModuleVersions.append(ModuleInfo.Version)
        except AttributeError:
            ModuleVersions.append('No Data Available')
            Logger.Log(f'Module {list(Modules.items())[ModuleInfoIndex][0]} Is Missing "Version" Information', 2)

        try:
            ModuleAuthors.append(ModuleInfo.Author)
        except AttributeError:
            ModuleAuthors.append('No Data Available')
            Logger.Log(f'Module {list(Modules.items())[ModuleInfoIndex][0]} Is Missing "Author" Information', 2)
        
        try:
            ModuleDependencies.append(ModuleInfo.Dependencies)
        except AttributeError:
            ModuleDependencies.append('No Data Available')
            Logger.Log(f'Module {list(Modules.items())[ModuleInfoIndex][0]} Is Missing "Dependencies" Information', 2)

        try:
            ModuleImports.append(ModuleInfo.Imports)
        except AttributeError:
            ModuleImports.append('No Data Available')
            Logger.Log(f'Module {list(Module.items())[ModuleInfoIndex][0]} Is Missing "Imports" Information', 2)


    # Check Add-On Dependencies #

    for RequiredPlugins in PluginDependencies:

        for Plugin in RequiredPlugins:

            WantedName = Plugin[0]
            WantedOperand = Plugin[1]
            WantedVersion = Plugin[2]

            if WantedName in PluginNames:

                NameIndex = PluginNames.index(WantedName)
                ActualVersion = PluginVersions[NameIndex]
                ActualNumericVersion = int(ActualVersion.replace('.', ''))
                WantedNumericVersion = int(WantedVersion.replace('.', ''))

                if WantedOperand == '<':
                    if ActualNumericVersion < WantedNumericVersion:
                        pass # Condition Met! #
                    else:
                        Logger.Log(f'Version Warning on Plugin "{WantedName}", Wants {WantedOperand}{WantedVersion}, Has {ActualVersion}', 1)
                elif WantedOperand == '=':
                    if ActualNumericVersion == WantedNumericVersion:
                        pass # Condition Met! #
                    else:
                        Logger.Log(f'Version Warning on Plugin "{WantedName}", Wants {WantedOperand}{WantedVersion}, Has {ActualVersion}', 1)
                elif WantedOperand == '>':
                    if ActualNumericVersion > WantedNumericVersion:
                        pass # Condition Met! #
                    else:
                        Logger.Log(f'Version Warning on Plugin "{WantedName}", Wants {WantedOperand}{WantedVersion}, Has {ActualVersion}', 1)
            
            else:
                Logger.Log(f'Missing Plugin "{WantedName}" at Version {WantedVersion}, Aborting startup. Please Locate and Install this Plugin.', 3)
                exit()


    for RequiredModules in ModuleDependencies:

        for Module in RequiredModules:

            WantedName = Module[0]
            WantedOperand = Module[1]
            WantedVersion = Module[2]

            if WantedName in ModuleNames:

                NameIndex = ModuleNames.index(WantedName)
                ActualVersion = ModuleVersions[NameIndex]
                ActualNumericVersion = int(ActualVersion.replace('.', ''))
                WantedNumericVersion = int(WantedVersion.replace('.', ''))

                if WantedOperand == '<':
                    if ActualNumericVersion < WantedNumericVersion:
                        pass # Condition Met! #
                    else:
                        Logger.Log(f'Version Warning on Module "{WantedName}", Wants {WantedOperand}{WantedVersion}, Has {ActualVersion}', 1)
                elif WantedOperand == '=':
                    if ActualNumericVersion == WantedNumericVersion:
                        pass # Condition Met! #
                    else:
                        Logger.Log(f'Version Warning on Module "{WantedName}", Wants {WantedOperand}{WantedVersion}, Has {ActualVersion}', 1)
                elif WantedOperand == '>':
                    if ActualNumericVersion > WantedNumericVersion:
                        pass # Condition Met! #
                    else:
                        Logger.Log(f'Version Warning on Module "{WantedName}", Wants {WantedOperand}{WantedVersion}, Has {ActualVersion}', 1)
            
            else:
                Logger.Log(f'Missing Module "{WantedName}" at Version {WantedVersion}, Aborting startup. Please Locate and Install this Module.', 3)
                exit()


    # Check Import Dependencies #

    Logger.Log('Checking Plugin Imports')

    for ImportIndex in range(len(PluginImports)):

        Logger.Log(f'Checking Import(s) From Plugin {list(Plugins.items())[PluginInfoIndex][ImportIndex]}')

        if PluginImports[ImportIndex] != 'No Data Available':
            CheckImports(PluginImports[ImportIndex], Logger)
        else:
            Logger.Log('Plugin has no Import data, skipping.')


    Logger.Log('Checking Module Imports')

    for ImportIndex in range(len(ModuleImports)):

        Logger.Log(f'Checking Import(s) From Module {list(Module.items())[ModuleInfoIndex][ImportIndex]}')

        if PluginImports[ImportIndex] != 'No Data Available':
            CheckImports(ModuleImports[ImportIndex], Logger)
        else:
            Logger.Log('Plugin has no Import data, skipping.')