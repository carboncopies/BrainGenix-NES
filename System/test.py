
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


def GetRecursiveAttributes(Target):

    # Get Attributes #
    StagingList = dir(Target)
    OutputList = []


    while StagingList != []:

        



TargetInstance = Test()
print(CheckIfBottomLevel(TargetInstance.TestAttribute))
GetRecursiveAttributes(TargetInstance)