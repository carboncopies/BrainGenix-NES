# BrainGenix-NES
# AGPLv3

from .. import Models

class Simulation():

    def __init__(self):
        
        # Create Attributes
        self.Name = None
        self.Description = None
    
    # Add Funcs
    def AddReceptor(self, _ReceptorConfig):
        return 0 # temp stuff
    
    def AddSynapse(self, _SynapseConfig):
        return 0 # temp stuff

    def AddCompartment(self, _CompartmentConfig):
        return 0 # temp stuff

    def AddNeuron(self, _NeuronConfig):
        return 0 # temp stuff
    

    # Run Funcs
    def RunAll(self, _Time="1s", _Step="100us"):
        return True # do something, we'll implement this later