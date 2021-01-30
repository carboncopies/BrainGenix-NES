###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

'''
Name: Command Parser
Description: This handles taking new commands and parsing them from the command line into function calls.
Date-Created: 2021-01-29
'''


def TraceCall(Command:str): # Traces A Command To The Right Function Call #

    # Parse Into List Of Calls #
    NameList = Command.split('.')

    # Return The List #
    return NameList