# BrainGenix-NES
# AGPLv3

from . import Configuration
from . import Modes

class Client:

    def __init__(self, _Configuration:Configuration):

        # Get And Validate Configuration
        self.Configuration = _Configuration
        self.ValidateConfig()
    

    

    def ValidateConfig(self):

        # Check Mode
        ValidModes =  ["Local", "Remote"]
        if self.Configuration.Mode not in ValidModes:
            raise Exception("NES Client Configuration Mode Not Set")

        
        # Remote not yet implemented
        if self.Configuration.Mode == Modes.Remote:
            raise NotImplementedError("Not finished yet, sorry :(")

