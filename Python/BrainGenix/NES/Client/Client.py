# BrainGenix-NES
# AGPLv3

from . import Configuration
from . import Modes

class Client:

    def __init__(self, _Configuration:Configuration):

        # Get And Validate Configuration
        self.Configuration = _Configuration
        self.ValidateConfig()
        self.Setup()


    # Helper Functions
    def ValidateConfig(self):

        # Check Mode
        ValidModes =  [Modes.Remote, Modes.Local]
        if self.Configuration.Mode not in ValidModes:
            raise Exception("NES Client Configuration Mode Not Set")

        
        # Remote not yet implemented
        if self.Configuration.Mode == Modes.Remote:
            raise NotImplementedError("Not finished yet, sorry :(")


    def Setup(self): # this should do some logic to make sure the connection is actually good.
        self.IsConnected = True


    # Getter Functions
    def IsConnected(self):
        return self.HasConnection