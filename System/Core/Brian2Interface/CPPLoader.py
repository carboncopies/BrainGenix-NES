###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################


# This Function Loads C++ Code As A String And Feeds It To The BrianInterface
def LoadCPP(Logger:object, FileStreamObject:str):

    # Log C++ Code Read
    Logger.Log('Reading C++ Brian2Interface Callback Code', 3)

    # Read File
    with open(CPPCodeFilePath, 'r') as FileStreamObject:

        FileString = FileStreamObject.read()

    # Log Completion
    Logger.Log('Successfully Read C++ Brian2Interface Callback Code', 2)

    # Return Filestream Object
    return FileString