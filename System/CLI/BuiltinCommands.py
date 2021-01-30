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
    - BG.WhoAmI: returns the name of the node the server runs on
    - BG.WhoIsLeader: returns name of leader node, should match output of BG.whoami
    - BG.ListCluster: outputs a list of all currently connected nodes
    - BG.NodeCPU: Lists cpu usage percentages for this node
    - BG.ClusterCPU: Lists cpu info for the entire cluster
    - BG.NodeMemory: Displays Stats about this node's memory usages/totals
    - BG.ClusterMemory: Displays stats about the entire cluster's memory usages/totals
    - BG.help: this help message
        '''
        return helpstring


    def WhoAmI(self):
        NodeName = self.Zookeeper.Name
        return NodeName


    def WhoIsLeader(self):
        NodeName = self.Zookeeper.ZookeeperConnection.get('/BrainGenix/System/Leader')[0].decode()
        return NodeName


    def ListNodes(self):
        Nodes = self.Zookeeper.ZookeeperConnection.get_children('/BrainGenix/System/Nodes')
        OutString = ''
        for Node in Nodes:
            OutString += f'{Node}\n'
        OutString = OutString[:-1]
        return OutString
    

    def NodeCount(self):
        Nodes = self.Zookeeper.ZookeeperConnection.get_children('/BrainGenix/System/Nodes')
        return str(len(Nodes))


    def NodeCPU(self):
        info = self.SysTelLeader.Info.get(self.Zookeeper.Name)
        CPUInfo = info.get('CPUUsage')
        avgcpu = sum(CPUInfo)/len(CPUInfo)
        Outstring = f'Average CPU for node {self.Zookeeper.Name}: {round(avgcpu)}%\n'
        for CpuCore in range(len(CPUInfo)):
            Outstring += f'    Core {CpuCore}: {CPUInfo[CpuCore]}%\n'
        Outstring = Outstring[:-1]
        return Outstring


    def ClusterCPU(self):
        CPUInfo = []

        for Node in self.Zookeeper.ZookeeperConnection.get_children('/BrainGenix/System/Nodes'):
            info = self.SysTelLeader.Info.get(Node)
            CPUInfo += info.get('CPUUsage')

        avgcpu = sum(CPUInfo)/len(CPUInfo)
        Outstring = f'Average CPU for cluster: {round(avgcpu)}%\n'

        for CpuCore in range(len(CPUInfo)):
            Outstring += f'    Core {CpuCore}: {CPUInfo[CpuCore]}%\n'
        Outstring = Outstring[:-1]

        return Outstring


    def NodeMemory(self):
        Info = self.SysTelLeader.Info.get(self.Zookeeper.Name)

        Outstring = f'Node {self.Zookeeper.Name} Is Using {Info.get("RAMPercent")}% of Physical Memory\n'
        Outstring += f'    - {Info.get("RAMUsage")} Bytes Used\n'
        Outstring += f'    - {Info.get("RAMFree")} Bytes Free\n'
        Outstring += f'    - {int(Info.get("RAMUsage")) + int(Info.get("RAMFree"))} Bytes Total\n'

        Outstring += f'Node Using {Info.get("SWAPPercent")}% of Swap\n'
        Outstring += f'    - {Info.get("SWAPUsage")} Bytes Used\n'
        Outstring += f'    - {Info.get("SWAPFree")} Bytes Free\n'
        Outstring += f'    - {int(Info.get("SWAPUsage")) + int(Info.get("SWAPFree"))} Bytes Total\n'

        return Outstring
    

    def ClusterMemory(self):

        RAMFreeTotal = 0
        RAMUsedTotal = 0
        RAMPercents = []

        SwapFreeTotal = 0
        SwapUsedTotal = 0
        SwapPercents = []

        for Node in self.Zookeeper.ZookeeperConnection.get_children('/BrainGenix/System/Nodes'):
            Info = self.SysTelLeader.Info.get(Node)
            RAMFreeTotal += Info.get('RAMFree')
            RAMUsedTotal += Info.get('RAMUsage')
            RAMPercents.append(Info.get('RAMPercent'))

            SwapFreeTotal += Info.get('SWAPFree')
            SwapUsedTotal += Info.get('SWAPUsage')
            SwapPercents.append(Info.get('SWAPPercent'))

        AVGRAM = sum(RAMPercents) / len(RAMPercents)
        AVGSWAP = sum(SwapPercents) / len(SwapPercents)

        Outstring = f'Cluster Memory Stats | RAM: {round(AVGRAM)}%, Swap: {round(AVGSWAP)}%\n'

        Outstring += f'    - {int(RAMUsedTotal)} Bytes RAM Used\n'
        Outstring += f'    - {int(RAMFreeTotal)} Bytes RAM Free\n\n'

        Outstring += f'    - {int(SwapUsedTotal)} Bytes Swap Used\n'
        Outstring += f'    - {int(SwapFreeTotal)} Bytes Swap Free\n'

        return Outstring        


    def SyntaxError(self):
        return 'BG_COMMAND_ENGINE: COMMAND IS NOT RECOGNIZED AS VALID IN FORMAT'