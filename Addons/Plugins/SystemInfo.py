###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

import cpuinfo
import platform
import datetime
import psutil
import GPUtil
import threading
import time

'''
Name: SystemInfo
Description: This plugin handles getting system usage statistics from the nodes.
Date-Created: 2020-12-19
'''

class AddonInfo(): # All Addons contain this class, and it tells the system what dependencies this module has #

    def __init__(self):

        self.Version = '0.0.1'
        self.Author = 'BrainGenix Team'
        self.Dependencies = []
        self.Imports = ['cpuinfo','platform','datetime','psutil','GPUtil','threading','time']
        

class FollowerMain(): # This Class Gets System Information And Puts It Into The Registry For Later Gathering #

    def __init__(self, AutoRefreshUpdates:bool=True, **kwargs):

        # Extract Logger From kwargs #

        Logger = kwargs['Logger']

        
        # Log Starting Message #

        Logger.Log('Collecting System Information')


        # Make Local Pointer To Logger #

        self.Logger = Logger


        # Get Static System Info #

        self.GetStaticStats()


        # Start The AutoUpdate Thread #

        if AutoRefreshUpdates:

            # Start Thread #

            Logger.Log('Automatic Refresh Enabled, Starting Thread')
            self.UpdateThread = threading.Thread(target=self.AutoRefresh, args=(1,))
            self.UpdateThread.start()
            Logger.Log('Thread Started, Dynamic Usage Stats Will Be Refreshed Automatically')


            # Block System Until One Refresh Is Completed #

            while True:
                try:
                    self.CPUUsage
                    break
                except:
                    pass

        #else: # Prevent an undefined error with the AtExit function

        #    self.UpdateThread = None

        # Define Registry #

        self.Registry = None


        # Call Log Output #

        self.LogSystemInfo(Logger)


    def GetPluginRegistry(self, Registry:dict): # Gets The Registry From The Main Process #


        # Get Registry #

        self.Registry = Registry

        self.Logger.Log('Successfully Acquired Registry')


        # Extract ZK Plugin From Registry

        self.ZK = self.Registry.get('Zookeeper')


        # Write Data To ZK #

        self.SendStaticStats()


    def AutoRefresh(self, RefreshInterval:float=1):

        # Start Inf Loop #

        while True:


            # Get Start Time #

            StartTime = time.time()


            # Get Stats #

            self.GetDynamicStats()


            # Update Zookeeper #
            if self.Registry != None: # Wait Until The Registry Is Populated #
                self.SendDynamicStats()


            # Delay For Requested Refresh Interval #

            EndTime = time.time()
            ExecutionTime = StartTime - EndTime
            DelayTime = RefreshInterval - ExecutionTime


            try:
                time.sleep(DelayTime)
            except ValueError: # Catch Exception if the execution time for finding stats takes longer than the refresh interval, resulting in negative delay
                pass


    def GetStaticStats(self):

        # Get System Name Info #

        PlatformInfo = platform.uname()

        self.OperatingSystemName = PlatformInfo.system
        self.NodeName = PlatformInfo.node
        self.OperatingSystemRelease = PlatformInfo.release
        self.OperatingSystemVersion = PlatformInfo.version


        # Get Last Boot Info #

        BootTimeInfo = psutil.boot_time()

        BootTimeDateTimeObject = datetime.datetime.fromtimestamp(BootTimeInfo)
        self.BootTimeDateString = f'{BootTimeDateTimeObject.year}/{BootTimeDateTimeObject.month}/{BootTimeDateTimeObject.day} {BootTimeDateTimeObject.hour}:{BootTimeDateTimeObject.minute}:{BootTimeDateTimeObject.second}'


        # Get System CPU Info #

        CPUInfo = cpuinfo.get_cpu_info()

        self.PythonVersion = CPUInfo.get('python_version')
        self.CPUInfoVersion = CPUInfo.get('cpuinfo_version_string')
        self.CPUArchitecture = CPUInfo.get('arch')
        self.CPUBits = CPUInfo.get('bits')
        self.CPUThreads = CPUInfo.get('count')
        self.CPUCores = psutil.cpu_count(logical=False)
        self.CPUVendor = CPUInfo.get('vendor_id_raw')
        self.CPUName = CPUInfo.get('brand_raw')
        self.CPUBaseClock = CPUInfo.get('hz_advertized_friendly')
        self.CPUInstructionSet = CPUInfo.get('flags')
        self.CPUL3CacheSize = CPUInfo.get('l3_cache_size')
        self.CPUL2CacheSize = CPUInfo.get('l2_cache_Size')
        self.CPUL1CacheSize = CPUInfo.get('l1_cache_size')


        # Get System Ram Info #

        RamInfo = psutil.virtual_memory()

        self.TotalSystemRAM = RamInfo.total

        SwapInfo = psutil.swap_memory()

        self.TotalSystemSwap = SwapInfo.total


        # Get System Disk Info #

        Partitions = psutil.disk_partitions()

        self.PartitionDevices = []
        self.PartitionMountPoints = []
        self.PartitionFileSystemType = []
        self.PartitionTotal = []
        self.PartitionUsed = []
        self.PartitionFree = []
        self.PartitionUsagePercent = []

        for Partition in Partitions:

            self.PartitionDevices.append(Partition.device)
            self.PartitionMountPoints.append(Partition.mountpoint)
            self.PartitionFileSystemType.append(Partition.fstype)

            try:
                PartitionUsage = psutil.disk_usage(Partition.mountpoint)
            except PermissionError: # Catch Exception Thrown If Partition Is Unreadable #
                continue
            
            self.PartitionTotal.append(PartitionUsage.total)
            self.PartitionUsed.append(PartitionUsage.used)
            self.PartitionFree.append(PartitionUsage.free)
            self.PartitionUsagePercent.append(PartitionUsage.percent)


        # Get Network Info #

        self.NetNames = []
        self.NetAddresses = []
        self.NetMasks = []
        self.NetBroadcasts = []
        
        IFAddresses = psutil.net_if_addrs()

        for InterfaceName, InterfaceAddresses in IFAddresses.items():
            for Address in InterfaceAddresses:
                self.NetNames.append(InterfaceName)
                self.NetAddresses.append(Address.address)
                self.NetMasks.append(Address.netmask)
                self.NetBroadcasts.append(Address.broadcast)


        # GPU Info #

        self.GPUIds = []
        self.GPUNames = []
        self.GPUTotalMemory = []
        
        GPUs = GPUtil.getGPUs()

        for GPU in GPUs:

            self.GPUIds.append(GPU.id)
            self.GPUNames.append(GPU.name)
            self.GPUTotalMemory.append(GPU.memoryTotal)


    def GetDynamicStats(self):


        # Get CPU Realtime Statistics #

        CPUInfo = psutil.cpu_freq()

        self.CPUFrequency = CPUInfo.current
        CPUUsage = []

        for Index, Percent in enumerate(psutil.cpu_percent(percpu=True, interval=1)):
            CPUUsage.append(Percent)

        self.CPUUsage = CPUUsage


        # Get Memory Info #

        PhysRAM = psutil.virtual_memory()
        Swap = psutil.swap_memory()

        self.RAMUsage = PhysRAM.used
        self.RAMFree = PhysRAM.free
        self.RAMPercent = PhysRAM.percent

        self.SWAPFree = Swap.free
        self.SWAPUsage = Swap.used
        self.SWAPPercent = Swap.percent


        # Get Realtime Network Info #

        NetInfo = psutil.net_io_counters()

        self.BytesSent = NetInfo.bytes_sent
        self.BytesRecv = NetInfo.bytes_recv
         
        
        # Get RealTime GPU Info #

        GPUs = GPUtil.getGPUs()

        GPUUsage = []
        GPUMem = []
        GPUTemps = []

        for GPU in GPUs:

            GPUUsage.append(GPU.load*100)
            GPUMem.append(GPU.memoryFree)
            GPUTemps.append(GPU.temperature)

        self.GPUUsage = GPUUsage
        self.GPUMem = GPUMem
        self.GPUTemps = GPUTemps

        
    def LogSystemInfo(self, Logger:object):


        # Log Operating System Info #

        Logger.Log('-- Operating System Info --')
        Logger.Log(f'Operating System Type: {self.OperatingSystemName}')
        Logger.Log(f'System Name: {self.NodeName}')
        Logger.Log(f'Operating System Release: {self.OperatingSystemRelease}')
        Logger.Log(f'Operating System Version: {self.OperatingSystemVersion}')


        # Boot Info #

        Logger.Log('-- Boot Info --')
        Logger.Log(f'Last Boot Was On: {self.BootTimeDateString}')


        # CPU Info #

        Logger.Log('-- CPU Info --')
        Logger.Log(f'Python Version: {self.PythonVersion}')
        Logger.Log(f'CPUInfo Version: {self.CPUInfoVersion}')
        Logger.Log(f'CPU Architecture: {self.CPUArchitecture}')
        Logger.Log(f'CPU Bits: {self.CPUBits}')
        Logger.Log(f'CPU Logical Threads: {self.CPUThreads}')
        Logger.Log(f'CPU Physical Cores: {self.CPUCores}')
        Logger.Log(f'CPU Vendor: {self.CPUVendor}')
        Logger.Log(f'CPU Name: {self.CPUName}')
        Logger.Log(f'CPU Base Clock: {self.CPUBaseClock}')
        #Logger.Log(f'CPU Instruction Set: {self.CPUInstructionSet}') /// Commented Out As It makes the Log look ugly, while providing minimal benefit.
        Logger.Log(f'CPU L3 Cache Size: {self.CPUL3CacheSize}')
        Logger.Log(f'CPU L2 Cache Size: {self.CPUL2CacheSize}')
        Logger.Log(f'CPU L1 Cache Size: {self.CPUL1CacheSize}')


        # Disk Info #

        Logger.Log('-- Disk Info --')        
        Logger.Log(f'Found {len(self.PartitionDevices)} Partitions')

        
        for PartitionIndex in range(len(self.PartitionDevices)):

            Logger.Log(f'Partition Device: {self.PartitionDevices[PartitionIndex]}')
            Logger.Log(f'   Mount Point: {self.PartitionMountPoints[PartitionIndex]}')
            Logger.Log(f'   Filesystem Type: {self.PartitionFileSystemType[PartitionIndex]}')
            Logger.Log(f'   Bytes Total: {self.PartitionTotal[PartitionIndex]}')
            Logger.Log(f'   Bytes Used: {self.PartitionUsed[PartitionIndex]}')
            Logger.Log(f'   Bytes Free: {self.PartitionFree[PartitionIndex]}')
            Logger.Log(f'   Percentage Used: {self.PartitionUsagePercent[PartitionIndex]}')


        # Network Info #

        Logger.Log('-- Network Info --')
        Logger.Log(f'Found {len(self.NetNames)} Network Interfaces')

        for DeviceIndex in range(len(self.NetNames)):

            Logger.Log(f'Network Interface: {self.NetNames[DeviceIndex]}')
            Logger.Log(f'   Address: {self.NetAddresses[DeviceIndex]}')
            Logger.Log(f'   Netmask: {self.NetMasks[DeviceIndex]}')
            Logger.Log(f'   Broadcast: {self.NetBroadcasts[DeviceIndex]}')

        # GPU Info #

        Logger.Log('-- GPU Info --')
        Logger.Log(f'Found {len(self.GPUIds)} GPU(s)')

        for GPUIndex in range(len(self.GPUIds)):

            Logger.Log(f'GPU ID: {GPUIndex}')
            Logger.Log(f'   GPU Name: {self.GPUNames[GPUIndex]}')
            Logger.Log(f'   GPU VRAM Total (MB): {self.GPUTotalMemory[GPUIndex]}')
            Logger.Log(f'   GPU UUID: {self.GPUIds[GPUIndex]}')


    def SendStaticStats(self): # Push Static Statistics To Zookeeper #


        # Create Current zNode #

        self.ZK.TryCreate(f'BrainGenix/SystemInfo/Static/{self.NodeName}', ephemeral=False)


        # Write zNodes For Platform Info #

        self.ZK.TryCreate(f'BrainGenix/SystemInfo/Static/{self.NodeName}/OperatingSystemName', zNodeData=self.OperatingSystemName.encode(), ephemeral=True)
        self.ZK.TryCreate(f'BrainGenix/SystemInfo/Static/{self.NodeName}/NodeName', zNodeData=self.NodeName.encode(), ephemeral=True)
        self.ZK.TryCreate(f'BrainGenix/SystemInfo/Static/{self.NodeName}/OperatingSystemRelease', zNodeData=self.OperatingSystemRelease.encode(), ephemeral=True)
        self.ZK.TryCreate(f'BrainGenix/SystemInfo/Static/{self.NodeName}/OperatingSystemVersion', zNodeData=self.OperatingSystemVersion.encode(), ephemeral=True)


        # Write zNodes For CPU Info #

        InstructionSets = b''

        for Item in self.CPUInstructionSet:
            InstructionSets += Item.encode() + b'&'
        InstructionSets = InstructionSets[:-1]

        self.ZK.TryCreate(f'BrainGenix/SystemInfo/Static/{self.NodeName}/PythonVersion', zNodeData=self.PythonVersion.encode(), ephemeral=True)
        self.ZK.TryCreate(f'BrainGenix/SystemInfo/Static/{self.NodeName}/CPUInfoVersion', zNodeData=self.CPUInfoVersion.encode(), ephemeral=True)
        self.ZK.TryCreate(f'BrainGenix/SystemInfo/Static/{self.NodeName}/CPUArchitecture', zNodeData=self.CPUArchitecture.encode(), ephemeral=True)
        self.ZK.TryCreate(f'BrainGenix/SystemInfo/Static/{self.NodeName}/CPUBits', zNodeData=str(self.CPUBits).encode(), ephemeral=True)
        self.ZK.TryCreate(f'BrainGenix/SystemInfo/Static/{self.NodeName}/CPUThreads', zNodeData=str(self.CPUThreads).encode(), ephemeral=True)
        self.ZK.TryCreate(f'BrainGenix/SystemInfo/Static/{self.NodeName}/CPUCores', zNodeData=str(self.CPUCores).encode(), ephemeral=True)
        self.ZK.TryCreate(f'BrainGenix/SystemInfo/Static/{self.NodeName}/CPUVendor', zNodeData=self.CPUVendor.encode(), ephemeral=True)
        self.ZK.TryCreate(f'BrainGenix/SystemInfo/Static/{self.NodeName}/CPUName', zNodeData=self.CPUName.encode(), ephemeral=True)
        self.ZK.TryCreate(f'BrainGenix/SystemInfo/Static/{self.NodeName}/CPUBaseClock', zNodeData=str(self.CPUBaseClock).encode(), ephemeral=True)
        self.ZK.TryCreate(f'BrainGenix/SystemInfo/Static/{self.NodeName}/CPUInstructionSet', zNodeData=InstructionSets, ephemeral=True)
        self.ZK.TryCreate(f'BrainGenix/SystemInfo/Static/{self.NodeName}/CPUL3CacheSize', zNodeData=str(self.CPUL3CacheSize).encode(), ephemeral=True)
        self.ZK.TryCreate(f'BrainGenix/SystemInfo/Static/{self.NodeName}/CPUL2CacheSize', zNodeData=str(self.CPUL2CacheSize).encode(), ephemeral=True)
        self.ZK.TryCreate(f'BrainGenix/SystemInfo/Static/{self.NodeName}/CPUL1CacheSize', zNodeData=str(self.CPUL1CacheSize).encode(), ephemeral=True)


        # Write zNodes For RAM Info #

        self.ZK.TryCreate(f'BrainGenix/SystemInfo/Static/{self.NodeName}/TotalSystemRAM', zNodeData=str(self.TotalSystemRAM).encode(), ephemeral=True)
        self.ZK.TryCreate(f'BrainGenix/SystemInfo/Static/{self.NodeName}/TotalSystemSwap', zNodeData=str(self.TotalSystemSwap).encode(), ephemeral=True)


        # Write zNodes For Disk Partition Info #

        PartitionDeviceSerialized = b''
        PartitionMountPointsSerialized = b''
        PartitionFileSystemSerialized = b''
        PartitionTotalSerialized = b''
        PartitionUsedSerialized = b''
        PartitionFreeSerialized = b''
        PartitionUsagePercentSerialized = b''

        for Item in self.PartitionDevices:
            PartitionDeviceSerialized += Item.encode() + b'&'
        PartitionDeviceSerialized = PartitionDeviceSerialized[:-1]

        for Item in self.PartitionMountPoints:
            PartitionMountPointsSerialized += Item.encode() + b'&'
        PartitionMountPointsSerialized = PartitionMountPointsSerialized[:-1]

        for Item in self.PartitionFileSystemType:
            PartitionFileSystemSerialized += Item.encode() + b'&'
        PartitionFileSystemSerialized = PartitionFileSystemSerialized[:-1]

        for Item in self.PartitionTotal:
            PartitionTotalSerialized += str(Item).encode() + b'&'
        PartitionTotalSerialized = PartitionTotalSerialized[:-1]

        for Item in self.PartitionUsed:
            PartitionUsedSerialized += str(Item).encode() + b'&'
        PartitionUsedSerialized = PartitionUsedSerialized[:-1]

        for Item in self.PartitionFree:
            PartitionFreeSerialized += str(Item).encode() + b'&'
        PartitionFreeSerialized = PartitionFreeSerialized[:-1]

        for Item in self.PartitionUsagePercent:
            PartitionUsagePercentSerialized += str(Item).encode() + b'&'
        PartitionUsagePercentSerialized = PartitionUsagePercentSerialized[:-1]


        self.ZK.TryCreate(f'BrainGenix/SystemInfo/Static/{self.NodeName}/PartitionDevice', zNodeData=PartitionDeviceSerialized, ephemeral=True)
        self.ZK.TryCreate(f'BrainGenix/SystemInfo/Static/{self.NodeName}/PartitionMountPoints', zNodeData=PartitionMountPointsSerialized, ephemeral=True)
        self.ZK.TryCreate(f'BrainGenix/SystemInfo/Static/{self.NodeName}/PartitionFileSystemType', zNodeData=PartitionFileSystemSerialized, ephemeral=True)
        self.ZK.TryCreate(f'BrainGenix/SystemInfo/Static/{self.NodeName}/PartitionTotal', zNodeData=PartitionTotalSerialized, ephemeral=True)
        self.ZK.TryCreate(f'BrainGenix/SystemInfo/Static/{self.NodeName}/PartitionUsed', zNodeData=PartitionUsedSerialized, ephemeral=True)
        self.ZK.TryCreate(f'BrainGenix/SystemInfo/Static/{self.NodeName}/PartitionFree', zNodeData=PartitionFreeSerialized, ephemeral=True)
        self.ZK.TryCreate(f'BrainGenix/SystemInfo/Static/{self.NodeName}/PartitionUsagePercent', zNodeData=PartitionUsagePercentSerialized, ephemeral=True)
    

        # Create zNodes For Network Info #

        NetNamesSerialized = b''
        NetAddressesSerialized = b''
        NetMasksSerialized = b''
        NetBroadcastsSerialized = b''

        for Item in self.NetNames:
            if Item == None:
                NetNamesSerialized += b'None&'
            else:
                NetNamesSerialized += Item.encode() + b'&'
        NetNamesSerialized = NetNamesSerialized[:-1]

        for Item in self.NetAddresses:
            if Item == None:
                NetAddressesSerialized += b'None&'
            else:
                NetAddressesSerialized += Item.encode() + b'&'
        NetAddressesSerialized = NetAddressesSerialized[:-1]

        for Item in self.NetMasks:
            if Item == None:
                NetMasksSerialized += b'None&'
            else:
                NetMasksSerialized += Item.encode() + b'&'
        NetMasksSerialized = NetMasksSerialized[:-1]

        for Item in self.NetBroadcasts:
            if Item == None:
                NetBroadcastsSerialized += b'None&'
            else:
                NetBroadcastsSerialized += Item.encode() + b'&'
        NetBroadcastsSerialized = NetBroadcastsSerialized[:-1]

        
        self.ZK.TryCreate(f'BrainGenix/SystemInfo/Static/{self.NodeName}/NetNames', zNodeData=NetNamesSerialized, ephemeral=True)
        self.ZK.TryCreate(f'BrainGenix/SystemInfo/Static/{self.NodeName}/NetAddresses', zNodeData=NetAddressesSerialized, ephemeral=True)
        self.ZK.TryCreate(f'BrainGenix/SystemInfo/Static/{self.NodeName}/NetMasks', zNodeData=NetMasksSerialized, ephemeral=True)
        self.ZK.TryCreate(f'BrainGenix/SystemInfo/Static/{self.NodeName}/NetBroadcasts', zNodeData=NetBroadcastsSerialized, ephemeral=True)


        # Create zNodes For GPU Info #

        GPUIdsSerialized = b''
        GPUNamesSerialized = b''
        GPUTotalMemorySerialized = b''

        for Item in self.GPUIds:
            if Item == None:
                GPUIdsSerialized += b'None&'
            else:
                GPUIdsSerialized += Item.encode() + b'&'
        GPUIdsSerialized = GPUIdsSerialized[:-1]

        for Item in self.GPUNames:
            if Item == None:
                GPUNamesSerialized += b'None&'
            else:
                GPUNamesSerialized += Item.encode() + b'&'
        GPUNamesSerialized = GPUNamesSerialized[:-1]

        for Item in self.GPUTotalMemory:
            if Item == None:
                GPUTotalMemorySerialized += b'None&'
            else:
                GPUTotalMemorySerialized += Item.encode() + b'&'
        GPUTotalMemorySerialized = GPUTotalMemorySerialized[:-1]

        self.ZK.TryCreate(f'BrainGenix/SystemInfo/Static/{self.NodeName}/GPUIds', zNodeData=GPUIdsSerialized, ephemeral=True)
        self.ZK.TryCreate(f'BrainGenix/SystemInfo/Static/{self.NodeName}/GPUNames', zNodeData=GPUNamesSerialized, ephemeral=True)
        self.ZK.TryCreate(f'BrainGenix/SystemInfo/Static/{self.NodeName}/GPUTotalMemory', zNodeData=GPUTotalMemorySerialized, ephemeral=True)


    def SendDynamicStats(self): # Push Dynamic Statistics To Zookeeper #


        # Create Current zNode #

        self.ZK.TryCreate(f'BrainGenix/SystemInfo/Dynamic/{self.NodeName}', ephemeral=False)


        # Write zNodes For CPU Info #

        CPUUsageSerialized = b''

        for Item in self.CPUUsage:
            CPUUsageSerialized += str(Item).encode() + b'&'
        CPUUsageSerialized = CPUUsageSerialized[:-1]

        self.ZK.TryCreateOverwrite(f'BrainGenix/SystemInfo/Dynamic/{self.NodeName}/CPUFrequency', zNodeData=str(self.CPUFrequency).encode(), ephemeral=True)
        self.ZK.TryCreateOverwrite(f'BrainGenix/SystemInfo/Dynamic/{self.NodeName}/CPUUsage', zNodeData=CPUUsageSerialized, ephemeral=True)


        # Write zNodes For Memory Info #

        self.ZK.TryCreateOverwrite(f'BrainGenix/SystemInfo/Dynamic/{self.NodeName}/RAMUsage', zNodeData=str(self.RAMUsage).encode(), ephemeral=True)
        self.ZK.TryCreateOverwrite(f'BrainGenix/SystemInfo/Dynamic/{self.NodeName}/RAMFree', zNodeData=str(self.RAMFree).encode(), ephemeral=True)
        self.ZK.TryCreateOverwrite(f'BrainGenix/SystemInfo/Dynamic/{self.NodeName}/RAMPercent', zNodeData=str(self.RAMPercent).encode(), ephemeral=True)

        self.ZK.TryCreateOverwrite(f'BrainGenix/SystemInfo/Dynamic/{self.NodeName}/SwapUsage', zNodeData=str(self.SWAPUsage).encode(), ephemeral=True)
        self.ZK.TryCreateOverwrite(f'BrainGenix/SystemInfo/Dynamic/{self.NodeName}/SwapFree', zNodeData=str(self.SWAPFree).encode(), ephemeral=True)
        self.ZK.TryCreateOverwrite(f'BrainGenix/SystemInfo/Dynamic/{self.NodeName}/Swapercent', zNodeData=str(self.SWAPPercent).encode(), ephemeral=True)

        
        # Write zNodes For Network Info #
        
        self.ZK.TryCreateOverwrite(f'BrainGenix/SystemInfo/Dynamic/{self.NodeName}/BytesSent', zNodeData=str(self.BytesSent).encode(), ephemeral=True)
        self.ZK.TryCreateOverwrite(f'BrainGenix/SystemInfo/Dynamic/{self.NodeName}/BytesRecv', zNodeData=str(self.BytesRecv).encode(), ephemeral=True)


        # Write zNodes For GPU Info #
        
        GPUUsageSerialized = b''
        GPUMemSerialized = b''
        GPUTempsSerialized = b''

        for Item in self.GPUUsage:
            GPUUsageSerialized += str(Item).encode() + b'&'
        GPUUsageSerialized = GPUUsageSerialized[:-1]

        for Item in self.GPUMem:
            GPUMemSerialized += str(Item).encode() + b'&'
        GPUMemSerialized = GPUMemSerialized[:-1]
        
        for Item in self.GPUTemps:
            GPUTempsSerialized += str(Item).encode() + b'&'
        GPUTempsSerialized = GPUTempsSerialized[:-1]
        

        self.ZK.TryCreateOverwrite(f'BrainGenix/SystemInfo/Dynamic/{self.NodeName}/GPUUsage', zNodeData=GPUUsageSerialized, ephemeral=True)
        self.ZK.TryCreateOverwrite(f'BrainGenix/SystemInfo/Dynamic/{self.NodeName}/GPUMem', zNodeData=GPUMemSerialized, ephemeral=True)
        self.ZK.TryCreateOverwrite(f'BrainGenix/SystemInfo/Dynamic/{self.NodeName}/GPUTemps', zNodeData=GPUTempsSerialized, ephemeral=True)


class LeaderMain(): # This Class Is Run By The Leader #

    def __init__(self, **kwargs):

        # Extract Logger From kwargs #

        Logger = kwargs['Logger']

        
        # Log Starting Message #

        Logger.Log('Collecting System Information')


        # Make Local Pointer To Logger #

        self.Logger = Logger

    
        # Define Registry #

        self.FollowerRegistry = None
        self.LeaderRegistry = None

        self.DynamicInfo = []
        self.StaticInfo = []

        self.HasRegistry = False


        # Start The AutoUpdate Thread #

        Logger.Log('Automatic Leader ZK Refresh Enabled, Starting Thread')
        self.UpdateThread = threading.Thread(target=self.AutoRefresh, args=(1,))
        self.UpdateThread.start()
        Logger.Log('Leader Thread Started, Dynamic Usage Stats Will Be Refreshed Automatically')


    def AutoRefresh(self, RefreshInterval:float=1):


        # Pull Static Info #

        while True:
            if self.HasRegistry: # Await Registry Population #
                self.PullStaticDataFromZK()
                self.DecodeStaticInfo()
                break


        # Start Inf Loop #

        while True:


            # Get Start Time #

            StartTime = time.time()


            # Get Stats #
            if self.LeaderRegistry != None:
                self.PullDynamicDataFromZK()



            # Delay For Requested Refresh Interval #

            EndTime = time.time()
            ExecutionTime = StartTime - EndTime
            DelayTime = RefreshInterval - ExecutionTime

            try:
                time.sleep(DelayTime)
            except ValueError: # Catch Exception if the execution time for finding stats takes longer than the refresh interval, resulting in negative delay #
                pass


    def GetPluginRegistry(self, FollowerRegistry:dict, LeaderRegistry:dict): # Gets The Registry From The Main Process #


        # Get Registries From Leader And Follower #

        self.FollowerRegistry = FollowerRegistry
        self.LeaderRegistry = LeaderRegistry

        self.Logger.Log('Successfully Acquired Registry')


        # Extract ZK Plugin From Registry

        self.ZK = self.FollowerRegistry.get('Zookeeper')


        # Extract Static Data #

        self.PullStaticDataFromZK()

        
        # Set Has Reg Stat #

        self.HasRegistry = True


    def PullStaticDataFromZK(self): # Pulls Static ZK Data From Zookeeper # 

        self.StaticInfo = []


        # Get zNodes In Static Info Section #

        StaticNodeChildren = self.ZK.ZookeeperConnection.get_children('/BrainGenix/SystemInfo/Static/')

        
        # Pull Data From zNodes #

        for NodeName in StaticNodeChildren:
            NodeStaticInfoList = self.ZK.ZookeeperConnection.get_children(f'/BrainGenix/SystemInfo/Static/{NodeName}')
            
            TempNodeDataDict = {}
            for Attribute in NodeStaticInfoList:
                NodeDataBytes = self.ZK.ZookeeperConnection.get(f'/BrainGenix/SystemInfo/Static/{NodeName}/{Attribute}')
                TempNodeDataDict.update({Attribute : NodeDataBytes})

            self.StaticInfo.append(TempNodeDataDict)


    def PullDynamicDataFromZK(self): # Pulls Dynamic ZK Data From Zookeeper # 

        DynamicCopy = self.DynamicInfo.copy()
        self.DynamicInfo = []


        # Get zNodes In Dynamic Info Section #

        DynamicNodeChildren = self.ZK.ZookeeperConnection.get_children('/BrainGenix/SystemInfo/Dynamic/')

        
        # Pull Data From zNodes #

        for NodeName in DynamicNodeChildren:
            NodeDynamicInfoList = self.ZK.ZookeeperConnection.get_children(f'/BrainGenix/SystemInfo/Dynamic/{NodeName}')
            
            TempNodeDataDict = {}
            for Attribute in NodeDynamicInfoList:
                NodeDataBytes = self.ZK.ZookeeperConnection.get(f'/BrainGenix/SystemInfo/Dynamic/{NodeName}/{Attribute}')
                TempNodeDataDict.update({Attribute : NodeDataBytes})

            self.DynamicInfo.append(TempNodeDataDict)


        # If The Dynamic Info Is Cleared #

        if self.DynamicInfo == [{}]:
            self.DynamicInfo = DynamicCopy


    def DecodeStaticInfo(self): # Decodes Static Stats From Extracted ZK Data #

        
        # Initialize Dictionary #

        self.StaticInfoDecoded = {}


        # Decode All Nodes #

        for Node in self.StaticInfo:

            NodeTemp = {}


            # Decode Platform Info #

            NodeTemp.update({'NodeName' : Node.get('NodeName')[0].decode()})
            NodeTemp.update({'OperatingSystemName' : Node.get('OperatingSystemName')[0].decode()})
            NodeTemp.update({'OperatingSystemRelease' : Node.get('OperatingSystemRelease')[0].decode()})
            NodeTemp.update({'OperatingSystemVersion' : Node.get('OperatingSystemVersion')[0].decode()})


            # Get CPU Info #

            NodeTemp.update({'PythonVersion' : Node.get('PythonVersion')[0].decode()})
            NodeTemp.update({'CPUInfoVersion' : Node.get('CPUInfoVersion')[0].decode()})
            NodeTemp.update({'CPUArchitecture' : Node.get('CPUArchitecture')[0].decode()})
            NodeTemp.update({'CPUBits' : Node.get('CPUBits')[0].decode()})
            NodeTemp.update({'CPUThreads' : Node.get('CPUThreads')[0].decode()})
            NodeTemp.update({'CPUCores' : Node.get('CPUCores')[0].decode()})
            NodeTemp.update({'CPUVendor' : Node.get('CPUVendor')[0].decode()})
            NodeTemp.update({'CPUName' : Node.get('CPUName')[0].decode()})
            NodeTemp.update({'CPUBaseClock' : Node.get('CPUBaseClock')[0].decode()})
            NodeTemp.update({'CPUInstructionSet' : Node.get('CPUInstructionSet')[0].decode()})
            NodeTemp.update({'CPUL3CacheSize' : Node.get('CPUL3CacheSize')[0].decode()})
            NodeTemp.update({'CPUL2CacheSize' : Node.get('CPUL2CacheSize')[0].decode()})
            NodeTemp.update({'CPUL1CacheSize' : Node.get('CPUL1CacheSize')[0].decode()})


            # Get RAM Info #

            NodeTemp.update({'TotalSystemRAM' : Node.get('TotalSystemRAM')[0].decode()})
            NodeTemp.update({'TotalSystemSwap' : Node.get('TotalSystemSwap')[0].decode()})


            # Get Disk Info #

            NodeTemp.update({'PartitionDevice' : Node.get('PartitionDevice')[0].decode()})
            NodeTemp.update({'PartitionMountPoints' : Node.get('PartitionMountPoints')[0].decode()})
            NodeTemp.update({'PartitionFileSystemType' : Node.get('PartitionFileSystemType')[0].decode()})
            NodeTemp.update({'PartitionTotal' : Node.get('PartitionTotal')[0].decode()})
            NodeTemp.update({'PartitionUsed' : Node.get('PartitionUsed')[0].decode()})
            NodeTemp.update({'PartitionFree' : Node.get('PartitionFree')[0].decode()})
            NodeTemp.update({'PartitionUsagePercent' : Node.get('PartitionUsagePercent')[0].decode()})


            # Get Network Info #

            NodeTemp.update({'NetNames' : Node.get('NetNames')[0].decode()})
            NodeTemp.update({'NetAddresses' : Node.get('NetAddresses')[0].decode()})
            NodeTemp.update({'NetMasks' : Node.get('NetMasks')[0].decode()})
            NodeTemp.update({'NetBroadcasts' : Node.get('NetBroadcasts')[0].decode()})


            # Get GPU Info #

            NodeTemp.update({'GPUIds' : Node.get('GPUIds')[0].decode()})
            NodeTemp.update({'GPUNames' : Node.get('GPUNames')[0].decode()})
            NodeTemp.update({'GPUTotalMemory' : Node.get('GPUTotalMemory')[0].decode()})


            # Update Static Info Dictionary #

            self.StaticInfoDecoded.update({NodeTemp.get('NodeName') : NodeTemp})
    

    def DecodeDynamicInfo(self): # Decodes Dynamic Stats From Extracted ZK Data #

        # Initialize Dictionary #

        self.DynamicInfoDecoded = {}


        # Decode All Nodes #

        for NodeIndex in range(len(self.DynamicInfo)):

            Node = self.DynamicInfo[NodeIndex]
            NodeTemp = {}


            # Get CPU Info #

            NodeTemp.update({'CPUFrequency' : Node.get('CPUFrequency')[0].decode()})
            NodeTemp.update({'CPUUsage' : Node.get('CPUUsage')[0].decode()})


            # Get Memory Info #

            NodeTemp.update({'RAMUsage' : Node.get('RAMUsage')[0].decode()})
            NodeTemp.update({'RAMFree' : Node.get('RAMFree')[0].decode()})
            NodeTemp.update({'RAMPercent' : Node.get('RAMPercent')[0].decode()})
            NodeTemp.update({'SwapUsage' : Node.get('SwapUsage')[0].decode()})
            NodeTemp.update({'SwapFree' : Node.get('SwapFree')[0].decode()})
            NodeTemp.update({'SwapPercent' : Node.get('SwapPercent')[0].decode()})


            # Get Network Info #

            NodeTemp.update({'BytesSent' : Node.get('BytesSent')[0].decode()})
            NodeTemp.update({'BytesRecv' : Node.get('BytesRecv')[0].decode()})


            # Get GPU Info #

            NodeTemp.update({'GPUUsage' : Node.get('GPUUsage')[0].decode()})
            NodeTemp.update({'GPUMem' : Node.get('GPUMem')[0].decode()})
            NodeTemp.update({'GPUTemps' : Node.get('GPUTemps')[0].decode()})


            # Update Dynamic Stats Dictionary #

            self.DynamicInfoDecoded.update({self.StaticInfo.get('NodeName') : NodeTemp})



