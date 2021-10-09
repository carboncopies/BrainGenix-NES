###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

import os

'''
Name: CPPLoader
Description: This file is responsible for creating the C++ Code Loader For The Main Simulation Engine.
Date-Created: 2021-10-08
'''

# This Function Loads C++ Code As A String And Feeds It To The BrianInterface
def LoadCPP(Logger:object, CPPRelativePath:str, HeaderRelativePath:str):

    # Log C++ Code Read
    Logger.Log('Reading Brian2Interface C++ Callback Code', 3)


    # Identify C++ Code Path
    CurrentDirectory = os.path.abspath(os.path.dirname(__file__))
    CPPCodeFilePath = os.path.join(CurrentDirectory, CPPRelativePath)

    # Read File
    with open(CPPCodeFilePath, 'r') as FileStreamObject:

        CPPFileString = FileStreamObject.read()

    # Log Completion
    Logger.Log('Successfully Read C++ Brian2Interface Callback Code', 2)




    # Log Header Code Read
    Logger.Log('Reading Brian2Interface Header Callback Code', 3)

    # Identify Header Code Path
    HeaderCodeFilePath = os.path.join(CurrentDirectory, HeaderRelativePath)

    # Read File
    with open(HeaderCodeFilePath, 'r') as FileStreamObject:

        HeaderFileString = FileStreamObject.read()

    # Log Completion
    Logger.Log('Successfully Read Header Brian2Interface Callback Code', 2)



    # Return Filestream Object
    return CPPFileString, HeaderFileString