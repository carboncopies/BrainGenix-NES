###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

'''
Name: Builtin Commands
Description: This does exactly what it sounds like, which is handle some builtin commands for the system.
Date-Created: 2021-01-29
'''

class BG(): # Standard Commands #

    def __init__(self, Zookeeper, SystemTelemetryLeader): # Initialize System #

        # Create Local Pointers #
        self.Zookeeper = Zookeeper
        self.SysTelLeader = SystemTelemetryLeader



    #########################
    # Define Commands Below #
    #########################

    def help(self):
        helpstring = '''Built-in Commands:
    - BG.whoami: returns the name of the node the server runs on
    - BG.whoisleader: returns name of leader node, should match output of BG.whoami
    - BG.listcluster: outputs a list of all currently connected nodes
    - BG.help: this help message
        '''
        return helpstring

    def whoami(self):
        NodeName = self.Zookeeper.Name
        return NodeName

    def whoisleader(self):
        NodeName = self.Zookeeper.ZookeeperConnection.get('/BrainGenix/System/Leader')[0].decode()
        return NodeName

    def listcluster(self):
        Nodes = self.Zookeeper.ZookeeperConnection.get_children('/BrainGenix/System/Nodes')
        OutString = ''
        for Node in Nodes:
            OutString += f'{Node}\n'
        OutString = OutString[:-1]
        return OutString
        
    def SyntaxError(self):
        return 'BG_COMMAND_ENGINE: COMMAND IS NOT RECOGNIZED AS VALID IN FORMAT'