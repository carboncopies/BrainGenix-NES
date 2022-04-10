###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################


## @package CommandIndexer
# Name: Command Indexer
# Description: This file provides Management API Command Indexing Support.
# Date-Created: 2021-06-30

## Converts AttributeList To Dictionary Format 
# @param DictionaryTarget Get To Target Path 
# @param TargetPath Target Path Dictionary 
# @param AttributeList Convert Attribute List To Dict
def AttributesToDictionary(DictionaryTarget, TargetPath, AttributeList):

    TargetDictionaryLocation = DictionaryTarget
    for KeyValue in TargetPath:
        TargetDictionaryLocation = TargetDictionaryLocation[KeyValue]

    TempDict = {}
    for Attribute in AttributeList:
        TempDict.update({Attribute : {}})

    ## Update Original Dict 
    TargetDictionaryLocation.update(TempDict)

## Returns A Filtered List Of Commands Excluding The '__' 
# @param Target Target containing list of attributes
def FilterGetAttributes(Target): 

    ## Get List Of Attributes 
    RawAttributes = dir(Target)

    ## Sort List 
    SortedAttributes = []
    for Attribute in RawAttributes:
        if '__' not in Attribute:
            SortedAttributes.append(Attribute)

    ## Return Output 
    return SortedAttributes

## Generates Traversals for specified Path 
# @param TargetDictionary Target Dictionary to be checked for existence
# @param Path Path to be checked for existence
def GeneratePathTraversals(TargetDictionary, Path=None):

    ## Check If Path Not Set 
    if not Path:
        Path=[]

    ## Check IF Not Yet Populated 
    if TargetDictionary == {}:
        yield Path,TargetDictionary

    ## If Valid Subdict 
    elif isinstance(TargetDictionary, dict):
        for SubAttributeIndex in TargetDictionary.keys():
            SubPath = Path[:]
            SubPath.append(SubAttributeIndex)
            for SubAttributeValue in GeneratePathTraversals(TargetDictionary[SubAttributeIndex], SubPath):
                yield SubAttributeValue

    ## If Non-Dict Value 
    else:
        yield Path,TargetDictionary

## Gets An Attribute From A Path 
# @param RootAttribute Starting attribute in path
# @param Path Path to be recursively checked for target attribute
def GetAttributeFromPath(RootAttribute, Path): 

    ## Get Attribute 
    TargetAttr = RootAttribute

    for PathStep in Path:
        TargetAttr = getattr(TargetAttr, PathStep)

    ## Return Target Attribute 
    return TargetAttr

## Creates Dictionary Tree Of All Commands With The Valid mAPI Prefix 
# @param Target Root Attribute of Path
# @param Logger Logger object
# @param RecursionDepth Depth of recursive iteration from root path
def IndexCommands(Target, Logger, RecursionDepth=5): 

    ## Initialize Parameters 
    Logger.Log('Initializing Command Indexing Parameters', 2)
    OutputDictionary = {}
    AttributesToDictionary(OutputDictionary, [], FilterGetAttributes(Target))
    
    ## Recursion Depth Loop 
    for RecursionDepthIndex in range(RecursionDepth - 1):

        ## Log Depth 
        Logger.Log(f'Getting Attributes At Recursion Depth: {RecursionDepthIndex}', 0)

        ## Iterate Through Traversed Subdicts 
        for Path in GeneratePathTraversals(OutputDictionary):

            ## Get Current Attribute 
            CurrentAttribute = GetAttributeFromPath(Target, Path[0])

            ## Get Subattributes From Current Attribute 
            SubAttributes = FilterGetAttributes(CurrentAttribute)

            ## Update Output Dictionary 
            AttributesToDictionary(OutputDictionary, Path[0], SubAttributes)

    ## Log Completion 
    Logger.Log('Attribute Dictionary Generated', 2)

    ## Return Output Dictionary 
    return OutputDictionary

## Creates a path given a root attribute and path
# @param Target Root Attribute of Path
# @param Path Path
def CreatePath(Target, Path):

    ## Set Up Working Target 
    WorkingTarget = Target

    ## Iterate Through Path 
    for PathNameString in Path:

        ## Check If Target Does Not Have Attribute 
        if not PathNameString in WorkingTarget:
            WorkingTarget.update({PathNameString : {}})

        ## Update Working Target 
        WorkingTarget = WorkingTarget[PathNameString]

## Filters Paths beginning with mAPI_ for command indexing
# @param InputDictionary Dictionary of paths
# @param Logger Logger Object
# @param RequiredString String to be checked to filter paths 
def FilterPaths(InputDictionary, Logger, RequiredString = 'mAPI_'):

    ## Init Output Dictionary 
    Logger.Log('Initializing Command Index Sorting System', 3)
    OutputDictionary = {}

    ## Generate Path Lists 
    Logger.Log('Creating Command Index Path Traversal List', 2)
    Paths = []
    for Path in GeneratePathTraversals(InputDictionary):
        Paths.append(Path[0])
    Logger.Log('Path Traversal Complete', 1)

    ## Sort From Shortest To Longest 
    Logger.Log('Sorting Path List', 2)
    SortedPaths = sorted(Paths, key=lambda ListLength: (len(ListLength), ListLength))
    Logger.Log('Sorted Path List', 1)

    ## Iterate From Longest To Shortest 
    Logger.Log('Moving Relevant Elements To Output Dictionary', 2)
    for InvertedPathIndex in range(len(SortedPaths)):

        ## Calculate Non-Inverted Path Index 
        PathIndex = (len(SortedPaths) - InvertedPathIndex) - 1

        ## Get Path 
        CurrentAttribute = InputDictionary
        for PathNameString in Paths[PathIndex]:
            CurrentAttribute = CurrentAttribute[PathNameString]

        ## Check If Valid Path 
        if ( (CurrentAttribute != {}) or (RequiredString in Paths[PathIndex][-1:][0]) ):

            CreatePath(OutputDictionary, Paths[PathIndex])

    ## Log Completion 
    Logger.Log('Command Filtering Complete', 2)

    ## Return Output 
    return OutputDictionary
