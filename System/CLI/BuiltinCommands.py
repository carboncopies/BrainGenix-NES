###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

'''
Name: Builtin Commands
Description: This does exactly what it sounds like, which is handle some builtin commands for the system.
Date-Created: 2021-01-29
'''

class BG(): # Standard Commands #

    '''
    This class contains a set of functions used by the ZKCLI as a builtin set of commands for the core systems to BG.
    You can interact with these by calling 'BG.help' in the CLI. 
    That will produce a list of commands and a breif description of what they do.
    *Please Don't Call This Unless You Know What You're Doing!*
    '''

    def __init__(self, Zookeeper, SystemTelemetryLeader): # Initialize System #

        '''
        Initializes the variables in the BG class.
        *Please Don't Call This Unless You Know What You're Doing!*
        '''

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

    def listnodes(self):
        Nodes = self.Zookeeper.ZookeeperConnection.get_children('/BrainGenix/System/Nodes')
        OutString = ''
        for Node in Nodes:
            OutString += f'{Node}\n'
        OutString = OutString[:-1]
        return OutString
    
    def nodecount(self):
        Nodes = self.Zookeeper.ZookeeperConnection.get_children('/BrainGenix/System/Nodes')
        return str(len(Nodes))

    def nodecpu(self):
        info = self.SysTelLeader.Info.get(self.Zookeeper.Name)
        CPUInfo = info.get('CPUUsage')
        avgcpu = sum(CPUInfo)/len(CPUInfo)
        Outstring = f'Average CPU: {avgcpu}\n'
        for CpuCore in range(len(CPUInfo)):
            Outstring += f'    Core {CpuCore}: {CPUInfo[CpuCore]}\n'
        Outstring = Outstring[:-1]
        return Outstring

    def SyntaxError(self):
        return 'BG_COMMAND_ENGINE: COMMAND IS NOT RECOGNIZED AS VALID IN FORMAT'