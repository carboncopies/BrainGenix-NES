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
Date-Modified: 2020-12-20
'''


class Initalize(): # This Class Gets System Information And Puts It Into The Registry For Later Gathering #

    def __init__(self, Logger:object, AutoRefreshUpdates:bool=True):

        
        # Log Starting Message #

        Logger.Log('Collecting System Information')


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


        # Call Log Output #

        self.LogSystemInfo(Logger)


    #def AtExit(self):

        # Shuts Down The System Greacefully #

        #if self.UpdateThread is not None:

        #    self.UpdateThread.join()

        #del self.UpdateThread


    def AutoRefresh(self, RefreshInterval:float=1):

        # Start Inf Loop #

        while True:


            # Get Start Time #

            StartTime = time.time()


            # Get Stats #

            self.GetDynamicStats()


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
            