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
import json

from Core.Management.Telemetry.TelemetryAPI import TelemetryAPI

'''
Name: System Telemetry
Description: This handles getting system usage statistics from the nodes.
Date-Created: 2021-01-29
'''




class Follower(): # This Class Gets System Information And Puts It Into ZK #

    def __init__(self, **kwargs):


        # Extract Logger From kwargs #
        self.Logger = kwargs['Logger']
        self.ZK = kwargs['Zookeeper']


        # Log Starting Message #
        self.Logger.Log('Collecting System Information')


        # Get Static Stats #
        self.GetStaticStatsDict()


        # Start AutoRefresh Daemon #
        self.Logger.Log('Automatic Refresh Enabled, Starting Thread')
        self.UpdateThread = threading.Thread(target=self.AutoRefresh, args=(1,), name='System Telemetry Follower Thread')
        self.UpdateThread.start()
        self.Logger.Log('Thread Started, Dynamic Usage Stats Will Be Refreshed Automatically')


    def AutoRefresh(self, RefreshInterval:float=1):


        # Start Inf Loop #
        while True:


            # Get Start Time #
            StartTime = time.time()


            # Get Dynamic Stats #
            self.GetDynamicStatsDict()


            # Update Zookeeper #
            self.SendStats()


            # Delay For Requested Refresh Interval #

            EndTime = time.time()
            ExecutionTime = StartTime - EndTime
            DelayTime = RefreshInterval - ExecutionTime

            try:
                time.sleep(DelayTime)
            except ValueError: # Catch Exception if the execution time for finding stats takes longer than the refresh interval, resulting in negative delay
                pass


    def GetStaticStatsDict(self): # Gets Static Stats And Puts Them Into A Dictionary #

        # Create New Dictionary #
        self.SystemHardware = {}


        # Get System Name Info #
        PlatformInfo = platform.uname()

        self.SystemHardware.update({'OperatingSystemName' : PlatformInfo.system})
        self.SystemHardware.update({'NodeName' : PlatformInfo.node})
        self.NodeName = PlatformInfo.node
        self.SystemHardware.update({'OperatingSystemRelease' : PlatformInfo.release})
        self.SystemHardware.update({'OperatingSystemVersion' : PlatformInfo.version})


        # Get Last Boot Info #
        BootTimeInfo = psutil.boot_time()

        BootTimeDateTimeObject = datetime.datetime.fromtimestamp(BootTimeInfo)
        self.SystemHardware.update({'BootTimeDateString' : f'{BootTimeDateTimeObject.year}/{BootTimeDateTimeObject.month}/{BootTimeDateTimeObject.day} {BootTimeDateTimeObject.hour}:{BootTimeDateTimeObject.minute}:{BootTimeDateTimeObject.second}'})


        # Get System CPU Info #
        CPUInfo = cpuinfo.get_cpu_info()

        self.SystemHardware.update({'PythonVersion' : CPUInfo.get('python_version')})
        self.SystemHardware.update({'CPUInfoVersion' : CPUInfo.get('cpuinfo_version_string')})
        self.SystemHardware.update({'CPUArchitecture' : CPUInfo.get('arch')})
        self.SystemHardware.update({'CPUBits' : CPUInfo.get('bits')})
        self.SystemHardware.update({'CPUThreads' : CPUInfo.get('count})')})
        self.SystemHardware.update({'CPUCores' : psutil.cpu_count(logical=False)})
        self.SystemHardware.update({'CPUVendor' : CPUInfo.get('vendor_id_raw')})
        self.SystemHardware.update({'CPUName' : CPUInfo.get('brand_raw')})
        self.SystemHardware.update({'CPUBaseClock' : CPUInfo.get('hz_advertized_friendly')})
        self.SystemHardware.update({'CPUInstructionSet' : CPUInfo.get('flags')})
        self.SystemHardware.update({'CPUL3CacheSize' : CPUInfo.get('l3_cache_size')})
        self.SystemHardware.update({'CPUL2CacheSize' : CPUInfo.get('l2_cache_Size')})
        self.SystemHardware.update({'CPUL1CacheSize' : CPUInfo.get('l1_cache_size')})


        # Get System Ram Info #
        RamInfo = psutil.virtual_memory()

        self.SystemHardware.update({'TotalSystemRAM' : RamInfo.total})

        SwapInfo = psutil.swap_memory()

        self.SystemHardware.update({'TotalSystemSwap' : SwapInfo.total})


        # Get System Disk Info #
        Partitions = psutil.disk_partitions()

        PartitionDevices = []
        PartitionMountPoints = []
        PartitionFileSystemType = []
        PartitionTotal = []
        PartitionUsed = []
        PartitionFree = []
        PartitionUsagePercent = []

        for Partition in Partitions:

            PartitionDevices.append(Partition.device)
            PartitionMountPoints.append(Partition.mountpoint)
            PartitionFileSystemType.append(Partition.fstype)

            try:
                PartitionUsage = psutil.disk_usage(Partition.mountpoint)
            except PermissionError: # Catch Exception Thrown If Partition Is Unreadable #
                continue

            PartitionTotal.append(PartitionUsage.total)
            PartitionUsed.append(PartitionUsage.used)
            PartitionFree.append(PartitionUsage.free)
            PartitionUsagePercent.append(PartitionUsage.percent)

        self.SystemHardware.update({'PartitionDevices' : PartitionDevices})
        self.SystemHardware.update({'PartitionMountPoints' : PartitionMountPoints})
        self.SystemHardware.update({'PartitionFileSystemType' : PartitionFileSystemType})
        self.SystemHardware.update({'PartitionTotal' : PartitionTotal})
        self.SystemHardware.update({'PartitionUsed' : PartitionUsed})
        self.SystemHardware.update({'PartitionFree' : PartitionFree})
        self.SystemHardware.update({'PartitionUsagePercent' : PartitionUsagePercent})


        # Get Network Info #
        NetNames = []
        NetAddresses = []
        NetMasks = []
        NetBroadcasts = []

        IFAddresses = psutil.net_if_addrs()

        for InterfaceName, InterfaceAddresses in IFAddresses.items():
            for Address in InterfaceAddresses:
                NetNames.append(InterfaceName)
                NetAddresses.append(Address.address)
                NetMasks.append(Address.netmask)
                NetBroadcasts.append(Address.broadcast)


        self.SystemHardware.update({'NetNames' : NetNames})
        self.SystemHardware.update({'NetAddresses' : NetAddresses})
        self.SystemHardware.update({'NetMasks' : NetMasks})
        self.SystemHardware.update({'NetBroadcasts' : NetBroadcasts})


        # GPU Info #
        GPUIds = []
        GPUNames = []
        GPUTotalMemory = []

        GPUs = GPUtil.getGPUs()

        for GPU in GPUs:

            GPUIds.append(GPU.id)
            GPUNames.append(GPU.name)
            GPUTotalMemory.append(GPU.memoryTotal)

        self.SystemHardware.update({'GPUIds' : GPUIds})
        self.SystemHardware.update({'GPUNames' : GPUNames})
        self.SystemHardware.update({'GPUTotalMemory' : GPUTotalMemory})


    def GetDynamicStatsDict(self): # Gets Dynamic Stats And Puts Them Into A Dictionary #


        # Get CPU Realtime Statistics #
        CPUInfo = psutil.cpu_freq()

        self.SystemHardware.update({'CPUFrequency' : CPUInfo.current})
        CPUUsage = []

        for _, Percent in enumerate(psutil.cpu_percent(percpu=True, interval=1)):
            CPUUsage.append(Percent)

        self.SystemHardware.update({'CPUUsage' : CPUUsage})


        # Get Memory Info #
        PhysRAM = psutil.virtual_memory()
        Swap = psutil.swap_memory()

        self.SystemHardware.update({'RAMUsage' : PhysRAM.used})
        self.SystemHardware.update({'RAMFree' : PhysRAM.free})
        self.SystemHardware.update({'RAMPercent' : PhysRAM.percent})

        self.SystemHardware.update({'SWAPFree' : Swap.free})
        self.SystemHardware.update({'SWAPUsage' : Swap.used})
        self.SystemHardware.update({'SWAPPercent' : Swap.percent})


        # Get Realtime Network Info #
        NetInfo = psutil.net_io_counters()

        self.SystemHardware.update({'BytesSent' : NetInfo.bytes_sent})
        self.SystemHardware.update({'BytesRecv' : NetInfo.bytes_recv})


        # Get RealTime GPU Info #
        GPUs = GPUtil.getGPUs()

        GPUUsage = []
        GPUMem = []
        GPUTemps = []

        for GPU in GPUs:

            GPUUsage.append(GPU.load*100)
            GPUMem.append(GPU.memoryFree)
            GPUTemps.append(GPU.temperature)

        self.SystemHardware.update({'GPUUsage' : GPUUsage})
        self.SystemHardware.update({'GPUMem' : GPUMem})
        self.SystemHardware.update({'GPUTemps' : GPUTemps})


    def SendStats(self): # Sends JSON Compressed Stats #

        # Dump The JSON Data #

        JSONArray = json.dumps(self.SystemHardware)
        JSONArray = JSONArray.encode('ascii')

        self.ZK.TryCreateOverwrite(f'/BrainGenix/System/Telemetry/{self.NodeName}', zNodeData = JSONArray, ephemeral = True)


class Leader(): # This Class Is Run By The Leader #

    def __init__(self, **kwargs):

        # Extract Logger From kwargs #
        self.Logger = kwargs['Logger']
        self.ZK = kwargs['Zookeeper']
        self.Info = {}

        # Log Starting Message #
        self.Logger.Log('Starting System Telemetry Leader')


        # Ensure ZK Path #
        self.ZK.ZookeeperConnection.ensure_path('/BrainGenix/System/Telemetry')


        # Extract Static Data #
        self.PullStatsFromZK()


        # Start The AutoUpdate Thread #
        self.Logger.Log('Created System Telemetry Leader Polling Thread')
        self.UpdateThread = threading.Thread(target=self.AutoRefresh, args=(3,), name='System Telemetry Leader Thread')

        # Start API Interface Thread #
        self.APIThread = threading.Thread(target=TelemetryAPI, args=(self.ZK, self.Logger, self))
        self.Logger.Log('Created Telemetry-Zookeeper API Interaction Handler')

    def Start(self):

        # Start System Telemetry Leader Thread #
        self.UpdateThread.start()
        self.Logger.Log('Started Leader Refresh Thread')

        # Start ZK-API Interface Thread #
        self.APIThread.start()
        self.Logger.Log('Started Telemetry-Zookeeper Interaction Thread')



    def AutoRefresh(self, RefreshInterval:float=1):


        # Start Inf Loop #
        while True:


            # Get Start Time #
            StartTime = time.time()


            # Get Stats #
            self.PullStatsFromZK()


            # Delay For Requested Refresh Interval #
            EndTime = time.time()
            ExecutionTime = StartTime - EndTime
            DelayTime = RefreshInterval - ExecutionTime

            try:
                time.sleep(DelayTime)
            except ValueError: # Catch Exception if the execution time for finding stats takes longer than the refresh interval, resulting in negative delay #
                pass


    def PullStatsFromZK(self): # Pulls All System Stats From Zookeeper #


        self.Info = {}


        # Get zNodes In Info Section #
        NodeChildren = self.ZK.ZookeeperConnection.get_children('/BrainGenix/System/Telemetry/')


        # Pull Data From zNodes #
        for NodeName in NodeChildren:
            NodeInfoJSON = self.ZK.ZookeeperConnection.get(f'/BrainGenix/System/Telemetry/{NodeName}')[0]
            NodeInfoJSON = NodeInfoJSON.decode('ascii')

            NodeInfoDecoded = json.loads(NodeInfoJSON)

            self.Info.update({NodeName : NodeInfoDecoded})