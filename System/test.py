
from queue import Empty


class Test():

    def __init__(self):
        self.mAPI_Attribute1()

    def Attribute1(self):
        pass

    def mAPI_Attribute1(self):
        self.TestAttribute = Test2()

    def mAPI_Attribute2(self):
        pass

class Test2():

    def __init__(self):
        pass

    def Attribute1(self):
        pass

    def mAPI_Attribute1(self):
        pass

    def mAPI_Attribute2(self):
        pass


def CheckIfBottomLevel(TargetAttribute):

    # Get All Attributes #
    Attributes = dir(TargetAttribute)

    # Check If All Are Internal Values #
    BottomLevel = True

    for Attribute in Attributes:
        
        # Check If Attribute Has '__' #
        if '__' not in str(Attribute):
            BottomLevel = False
            break

    # Return Value #
    return BottomLevel


def GetRecursiveAttributes(Target, SearchDepth=5):

    # Get Attributes #
    StageDict = {'.' : Target}
    CurrentPathIndexes = []


    for CurrentSearchDepth in range(SearchDepth):

        # Add Item To Current Path #
        CurrentPathIndexes.append(0)

        # Get String Name For Current Path #
        PathString = ''
        for PathLevelIndex in CurrentPathIndexes:
            AttributeName = dir(getattr(Target, PathString))
            PathString += AttributeName + '.'
        PathString = PathString[:-1]

        print(PathString)
        



TargetInstance = Test()
GetRecursiveAttributes(TargetInstance)