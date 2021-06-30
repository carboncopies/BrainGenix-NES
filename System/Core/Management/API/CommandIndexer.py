###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################


'''
Name: Command Indexer
Description: This file provides Management API Command Indexing Support.
Date-Created: 2021-06-30
'''


from typing import Dict


class Test1():

    def __init__(self):
        pass

    def Attr1(self):
        pass

    def mAPI_Attr2(self):
        pass


class Test2():

    def __init__(self):
        self.InitTest1()

    def Attr1(self):
        pass

    def mAPI_Attr2(self):
        pass

    def InitTest1(self):
        self.Class1 = Test1()


def AttributesToDictionary(DictionaryTarget, TargetPath, AttributeList): # Converts AttributeList To Dictionary Format #

    # Get To Target Path #
    TargetDictionaryLocation = DictionaryTarget
    for KeyValue in TargetPath:
        TargetDictionaryLocation = TargetDictionaryLocation[KeyValue]

    # Convert Attribute List To Dict #
    TempDict = {}
    for Attribute in AttributeList:
        TempDict.update({Attribute : {}})

    # Update Original Dict #
    TargetDictionaryLocation.update(TempDict)


def FilterGetAttributes(Target): # Returns A Filtered List Of Commands Excluding The '__' #

    # Get List Of Attributes #
    RawAttributes = dir(Target)

    # Sort List #
    SortedAttributes = []
    for Attribute in RawAttributes:
        if '__' not in Attribute:
            SortedAttributes.append(Attribute)

    # Return Output #
    return SortedAttributes


def GeneratePathTraversals(TargetDictionary, path=None):
    if not path:
        path=[]
    if isinstance(TargetDictionary, dict):
        for x in TargetDictionary.keys():
            local_path = path[:]
            local_path.append(x)
            for b in GeneratePathTraversals(TargetDictionary[x], local_path):
                print(b)
                yield b
    else: 
        yield path,TargetDictionary





def IndexCommands(Target, RecursionDepth=5): # Creates Dictionary Tree Of All Commands With The Valid mAPI Prefix #

    # Initialize Parameters #
    OutputDictionary = {}
    AttributesToDictionary(OutputDictionary, [], FilterGetAttributes(Target))


    # Recursion Depth Loop #
    for CurrentRecursionDepth in range(RecursionDepth - 1):

        # Generate Path Traversal List #
        for Path in GeneratePathTraversals(OutputDictionary):

            print(Path)
        



TestClass = Test2()
IndexCommands(TestClass, RecursionDepth=5)