###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

#import threading
#import time

'''
Name: API Functions
Description: This file is used to connect the system's API to the leader.
Date-Created: 2021-03-03
'''

class TelemetryAPI(): # Exposes Telemetry Functions To The Leader #


    def __init__(self, Zookeeper, Logger, LeaderAttributes): # Initializes the System #

        # Set Local Variables #
        self.LeaderAttributes = LeaderAttributes
        self.Zookeeper = Zookeeper
        self.Logger = Logger


    def GetClusterSize(self): # Gets the cluster's size #

        # Get The Number Of Currently Connected Nodes In the System #
        return len(self.LeaderAttributes.Info)


    def GetOperatingSystem(self, NodeName): # Gets The Operating System Name #

        # Get The Info #
        NodeJSON = self.LeaderAttributes.Info[NodeName]
        OSName = NodeJSON['OperatingSystemName']

        # Return The Information #
        return OSName


    def GetNodeHostname(self, NodeName): # Gets The Node's Hostname #

        # Get The Info #
        NodeJSON = self.LeaderAttributes.Info[NodeName]
        NodeName = NodeJSON['NodeName']

        # Return The Information #
        return NodeName


    def GetOperatingSystemRelease(self, NodeName): # Gets The Operating System Release Version #

        # Get The Info #
        NodeJSON = self.LeaderAttributes.Info[NodeName]
        Info = NodeJSON['OperatingSystemRelease']

        # Return The Information #
        return Info


    def GetOperatingSystemVersion(self, NodeName): # Gets The Operating System Version #

        # Get The Info #
        NodeJSON = self.LeaderAttributes.Info[NodeName]
        Info = NodeJSON['OperatingSystemVersion']

        # Return The Information #
        return Info


    def GetLastBootDate(self, NodeName): # Gets The Last Boot Date #

        # Get The Info #
        NodeJSON = self.LeaderAttributes.Info[NodeName]
        Info = NodeJSON['BootTimeDateString']

        # Return The Information #
        return Info


    def GetPythonVersion(self, NodeName): # Gets Node Info #

        # Get The Info #
        NodeJSON = self.LeaderAttributes.Info[NodeName]
        Info = NodeJSON['PythonVersion']

        # Return The Information #
        return Info


    def GetCPUInfoVersion(self, NodeName): # Gets Node Info #

        # Get The Info #
        NodeJSON = self.LeaderAttributes.Info[NodeName]
        Info = NodeJSON['CPUInfoVersion']

        # Return The Information #
        return Info


    def GetCPUArchitecture(self, NodeName): # Gets Node Info #

        # Get The Info #
        NodeJSON = self.LeaderAttributes.Info[NodeName]
        Info = NodeJSON['CPUArchitecture']

        # Return The Information #
        return Info


    def GetCPUBits(self, NodeName): # Gets Node Info #

        # Get The Info #
        NodeJSON = self.LeaderAttributes.Info[NodeName]
        Info = NodeJSON['CPUBits']

        # Return The Information #
        return Info


    def GetCPUThreads(self, NodeName): # Gets Node Info #

        # Get The Info #
        NodeJSON = self.LeaderAttributes.Info[NodeName]
        Info = NodeJSON['CPUThreads']

        # Return The Information #
        return Info


    def GetCPUCores(self, NodeName): # Gets Node Info #

        # Get The Info #
        NodeJSON = self.LeaderAttributes.Info[NodeName]
        Info = NodeJSON['CPUCores']

        # Return The Information #
        return Info


    def GetCPUVendor(self, NodeName): # Gets Node Info #

        # Get The Info #
        NodeJSON = self.LeaderAttributes.Info[NodeName]
        Info = NodeJSON['CPUVendor']

        # Return The Information #
        return Info


    def GetCPUName(self, NodeName): # Gets Node Info #

        # Get The Info #
        NodeJSON = self.LeaderAttributes.Info[NodeName]
        Info = NodeJSON['CPUName']

        # Return The Information #
        return Info


    def GetCPUBaseClock(self, NodeName): # Gets Node Info #

        # Get The Info #
        NodeJSON = self.LeaderAttributes.Info[NodeName]
        Info = NodeJSON['CPUBaseClock']

        # Return The Information #
        return Info


    def GetCPUInstructionSet(self, NodeName): # Gets Node Info #

        # Get The Info #
        NodeJSON = self.LeaderAttributes.Info[NodeName]
        Info = NodeJSON['CPUInstructionSet']

        # Return The Information #
        return Info


    def GetCPUL3CacheSize(self, NodeName): # Gets Node Info #

        # Get The Info #
        NodeJSON = self.LeaderAttributes.Info[NodeName]
        Info = NodeJSON['CPUL3CacheSize']

        # Return The Information #
        return Info


    def GetCPUL2CacheSize(self, NodeName): # Gets Node Info #

        # Get The Info #
        NodeJSON = self.LeaderAttributes.Info[NodeName]
        Info = NodeJSON['CPUL2CacheSize']

        # Return The Information #
        return Info


    def GetCPUL1CacheSize(self, NodeName): # Gets Node Info #

        # Get The Info #
        NodeJSON = self.LeaderAttributes.Info[NodeName]
        Info = NodeJSON['CPUL1CacheSize']

        # Return The Information #
        return Info


    def GetTotalSystemRam(self, NodeName): # Gets Node Info #

        # Get The Info #
        NodeJSON = self.LeaderAttributes.Info[NodeName]
        Info = NodeJSON['TotalSystemRAM']

        # Return The Information #
        return Info


    def GetTotalSystemSwap(self, NodeName): # Gets Node Info #

        # Get The Info #
        NodeJSON = self.LeaderAttributes.Info[NodeName]
        Info = NodeJSON['TotalSystemSwap']

        # Return The Information #
        return Info


    def GetPartitionDevices(self, NodeName): # Gets Node Info #

        # Get The Info #
        NodeJSON = self.LeaderAttributes.Info[NodeName]
        Info = NodeJSON['PartitionDevices']

        # Return The Information #
        return Info


    def GetPartitionMountPoints(self, NodeName): # Gets Node Info #

        # Get The Info #
        NodeJSON = self.LeaderAttributes.Info[NodeName]
        Info = NodeJSON['PartitionMountPoints']

        # Return The Information #
        return Info


    def GetPartitonFileSystemType(self, NodeName): # Gets Node Info #

        # Get The Info #
        NodeJSON = self.LeaderAttributes.Info[NodeName]
        Info = NodeJSON['PartitionFileSystemType']

        # Return The Information #
        return Info


    def GetPartitionTotalBytes(self, NodeName): # Gets Node Info #

        # Get The Info #
        NodeJSON = self.LeaderAttributes.Info[NodeName]
        Info = NodeJSON['PartitionTotal']

        # Return The Information #
        return Info


    def GetPartitionUsedBytes(self, NodeName): # Gets Node Info #

        # Get The Info #
        NodeJSON = self.LeaderAttributes.Info[NodeName]
        Info = NodeJSON['PartitionUsed']

        # Return The Information #
        return Info


    def GetPartitionFreeBytes(self, NodeName): # Gets Node Info #

        # Get The Info #
        NodeJSON = self.LeaderAttributes.Info[NodeName]
        Info = NodeJSON['PartitonFree']

        # Return The Information #
        return Info

    def GetPartitionUsedPercent(self, NodeName): # Gets Node Info #

        # Get The Info #
        NodeJSON = self.LeaderAttributes.Info[NodeName]
        Info = NodeJSON['PartitionUsagePercent']

        # Return The Information #
        return Info


    def GetNetworkAdapterNames(self, NodeName): # Gets Node Info #

        # Get The Info #
        NodeJSON = self.LeaderAttributes.Info[NodeName]
        Info = NodeJSON['NetNames']

        # Return The Information #
        return Info


    def GetNetworkAddresses(self, NodeName): # Gets Node Info #

        # Get The Info #
        NodeJSON = self.LeaderAttributes.Info[NodeName]
        Info = NodeJSON['NetAddresses']

        # Return The Information #
        return Info


    def GetNetMasks(self, NodeName): # Gets Node Info #

        # Get The Info #
        NodeJSON = self.LeaderAttributes.Info[NodeName]
        Info = NodeJSON['NetMasks']

        # Return The Information #
        return Info


    def GetNetBroadcasts(self, NodeName): # Gets Node Info #

        # Get The Info #
        NodeJSON = self.LeaderAttributes.Info[NodeName]
        Info = NodeJSON['NetBroadcasts']

        # Return The Information #
        return Info


    def GetGPUIds(self, NodeName): # Gets Node Info #

        # Get The Info #
        NodeJSON = self.LeaderAttributes.Info[NodeName]
        Info = NodeJSON['GPUIds']

        # Return The Information #
        return Info


    def GetGPUNames(self, NodeName): # Gets Node Info #

        # Get The Info #
        NodeJSON = self.LeaderAttributes.Info[NodeName]
        Info = NodeJSON['GPUNames']

        # Return The Information #
        return Info


    def GetGPUTotalMemory(self, NodeName): # Gets Node Info #

        # Get The Info #
        NodeJSON = self.LeaderAttributes.Info[NodeName]
        Info = NodeJSON['GPUTotalMemory']

        # Return The Information #
        return Info


    def GetGPUClockRate(self, NodeName): # Gets Node Info #

        # Get The Info #
        NodeJSON = self.LeaderAttributes.Info[NodeName]
        Info = NodeJSON['GPUFrequency']

        # Return The Information #
        return Info


    def GetCPUUsage(self, NodeName): # Gets Node Info #

        # Get The Info #
        NodeJSON = self.LeaderAttributes.Info[NodeName]
        Info = NodeJSON['CPUUsage']

        # Return The Information #
        return Info


    def GetRamUsage(self, NodeName): # Gets Node Info #

        # Get The Info #
        NodeJSON = self.LeaderAttributes.Info[NodeName]
        Info = NodeJSON['RAMUsage']

        # Return The Information #
        return Info


    def GetRamFree(self, NodeName): # Gets Node Info #

        # Get The Info #
        NodeJSON = self.LeaderAttributes.Info[NodeName]
        Info = NodeJSON['RAMFree']

        # Return The Information #
        return Info


    def GetRamPercent(self, NodeName): # Gets Node Info #

        # Get The Info #
        NodeJSON = self.LeaderAttributes.Info[NodeName]
        Info = NodeJSON['RAMPercent']

        # Return The Information #
        return Info


    def GetSwapFree(self, NodeName): # Gets Node Info #

        # Get The Info #
        NodeJSON = self.LeaderAttributes.Info[NodeName]
        Info = NodeJSON['SWAPFree']

        # Return The Information #
        return Info


    def GetSwapUsage(self, NodeName): # Gets Node Info #

        # Get The Info #
        NodeJSON = self.LeaderAttributes.Info[NodeName]
        Info = NodeJSON['SWAPUsage']

        # Return The Information #
        return Info


    def GetSwapPercent(self, NodeName): # Gets Node Info #

        # Get The Info #
        NodeJSON = self.LeaderAttributes.Info[NodeName]
        Info = NodeJSON['SWAPPercent']

        # Return The Information #
        return Info


    def GetBytesSent(self, NodeName): # Gets Node Info #

        # Get The Info #
        NodeJSON = self.LeaderAttributes.Info[NodeName]
        Info = NodeJSON['BytesSent']

        # Return The Information #
        return Info


    def GetBytesRecieved(self, NodeName): # Gets Node Info #

        # Get The Info #
        NodeJSON = self.LeaderAttributes.Info[NodeName]
        Info = NodeJSON['BytesRecv']

        # Return The Information #
        return Info


    def GetGPUUsage(self, NodeName): # Gets Node Info #

        # Get The Info #
        NodeJSON = self.LeaderAttributes.Info[NodeName]
        Info = NodeJSON['GPUUsage']

        # Return The Information #
        return Info


    def GetGPUMemory(self, NodeName): # Gets Node Info #

        # Get The Info #
        NodeJSON = self.LeaderAttributes.Info[NodeName]
        Info = NodeJSON['GPUMem']

        # Return The Information #
        return Info


    def GetGPUTemps(self, NodeName): # Gets Node Info #

        # Get The Info #
        NodeJSON = self.LeaderAttributes.Info[NodeName]
        Info = NodeJSON['GPUTemps']

        # Return The Information #
        return Info