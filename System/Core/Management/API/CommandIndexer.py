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

class Test3():

    def __init__(self):
        self.InitTest1()

    def Attr1(self):
        pass

    def mAPI_Attr2(self):
        pass

    def InitTest1(self):
        self.Class2 = Test2()

class Test4():

    def __init__(self):
        self.InitTest1()

    def Attr1(self):
        pass

    def mAPI_Attr2(self):
        pass

    def InitTest1(self):
        self.Class3 = Test3()

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


def GeneratePathTraversals(TargetDictionary, Path=None):
    
    # Check If Path Not Set #
    if not Path:
        Path=[]

    # Check IF Not Yet Populated #
    if TargetDictionary == {}:
        yield Path,TargetDictionary

    # If Valid Subdict #
    elif isinstance(TargetDictionary, dict):
        for SubAttributeIndex in TargetDictionary.keys():
            SubPath = Path[:]
            SubPath.append(SubAttributeIndex) 
            for SubAttributeValue in GeneratePathTraversals(TargetDictionary[SubAttributeIndex], SubPath):
                yield SubAttributeValue

    # If Non-Dict Value #
    else: 
        yield Path,TargetDictionary


def GetAttributeFromPath(RootAttribute, Path): # Gets An Attribute From A Path #

    # Get Attribute #
    TargetAttr = RootAttribute

    for PathStep in Path:
        TargetAttr = getattr(TargetAttr, PathStep)

    # Return Target Attribute #
    return TargetAttr


def IndexCommands(Target, RecursionDepth=5): # Creates Dictionary Tree Of All Commands With The Valid mAPI Prefix #

    # Initialize Parameters #
    OutputDictionary = {}
    AttributesToDictionary(OutputDictionary, [], FilterGetAttributes(Target))


    # Recursion Depth Loop #
    for _ in range(RecursionDepth - 1):

        # Iterate Through Traversed Subdicts #
        for Path in GeneratePathTraversals(OutputDictionary):

            # Get Current Attribute #
            CurrentAttribute = GetAttributeFromPath(Target, Path[0])

            # Get Subattributes From Current Attribute #
            SubAttributes = FilterGetAttributes(CurrentAttribute)

            # Update Output Dictionary #
            AttributesToDictionary(OutputDictionary, Path[0], SubAttributes)

        
    # Return Output Dictionay #
    return OutputDictionary


def TryCreateOverwrite(Target, Path):
    
    # Set Up Working Target #
    WorkingTarget = Target

    # Iterate Through Path #
    for PathNameString in Path:

        # Check If Target Does Not Have Attribute #
        if not PathNameString in WorkingTarget:
            WorkingTarget.update({PathNameString : {}})
        
        # Update Working Target #
        WorkingTarget = WorkingTarget[PathNameString]


def Filter2Test(InputDictionary, RequiredString = 'mAPI_'):

    # Init Output Dictionary #
    OutputDictionary = {}

    # Generate Path Lists #
    Paths = []
    for Path in GeneratePathTraversals(InputDictionary):
        Paths.append(Path[0])

    # Sort From Shortest To Longest #
    SortedPaths = sorted(Paths, key=lambda ListLength: (len(ListLength), ListLength))

    # Iterate From Longest To Shortest #
    for InvertedPathIndex in range(len(SortedPaths)):

        # Calculate Non-Inverted Path Index #
        PathIndex = (len(SortedPaths) - InvertedPathIndex) - 1

        # Get Path #
        CurrentAttribute = InputDictionary
        for PathNameString in Paths[PathIndex]:
            CurrentAttribute = CurrentAttribute[PathNameString]

        # Check If Valid Path #
        if ( (CurrentAttribute != {}) or (RequiredString in Paths[PathIndex][-1:][0]) ):

            TryCreateOverwrite(OutputDictionary, Paths[PathIndex])


    #print(Paths)
    print(OutputDictionary)


TestClass = Test4()
DictVar = IndexCommands(TestClass, RecursionDepth=5)
Out = Filter2Test(DictVar)#FilterCommandDictionary(DictVar)

import yaml
print('')
print(yaml.safe_dump(DictVar))