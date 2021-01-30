###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

'''
Name: Command Parser
Description: This handles taking new commands and parsing them from the command line into function calls.
Date-Created: 2021-01-29
'''


def TraceCall(Command:str): # Traces A Command To The Right Function Call #

    '''
    This function is used to parse the command into a list.
    *Please Don't Call This Unless You Know What You're Doing!*
    '''

    # Get Args #
    ArgumentList = Command.split(' ')

    # Parse Into List Of Calls #
    NameList = ArgumentList[0].split('.')
    del ArgumentList[0]

    # Format Args #
    ArgDict = {}
    for ArgumentIndex in range(int(len(ArgumentList)/2)):
        Kwarg = ArgumentList[ArgumentIndex*2]
        Value = ArgumentList[ArgumentIndex*2 + 1]
        ArgDict.update({Kwarg : Value})

    # Return The List #
    return NameList, ArgDict