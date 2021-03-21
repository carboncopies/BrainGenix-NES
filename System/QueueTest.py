import atexit
import time
import kazoo

from Core.LoadConfig import LoadConfig
from Core.Logger import SysLog

from Zookeeper.Zookeeper import ZK



# Set Version Information
Version = '0.0.4'


# Load Config #
ConfigPath = 'Config/LocalConfig.yaml'
AddonsPath, LogPath, BufferLength, PrintLogOutput, LinesPerFile, EnableGzip, ZKHost, DBUname, DBPasswd, DBHost, DBName = LoadConfig(ConfigPath)


# Initialize Logger #
Logger = SysLog('0', LogPath+'-APIServer', BufferLength=BufferLength, LogSegmentLength=LinesPerFile, ConsoleOutputEnabled=PrintLogOutput, EnableGzip = EnableGzip) # NOTE: THE SYSLOG ID HERE NEEDS TO BE REPLACED WITH THE ZOOKEEPER ID LATER ON! (FIRST ARG)


# Purges The Log Buffer On System Exit #
@atexit.register
def CleanLog():
    Logger.PurgeBuffer()
    Logger.CleanExit()


# Initialize ZK #
Zookeeper = ZK(Logger)
Zookeeper.ConnectToZookeeper(Logger, '127.0.0.1:2181')


# Create Queue For Testing #
Queue1 = kazoo.recipe.queue.Queue(Zookeeper.ZookeeperConnection, '/BrainGenix/TestQueue')


# Test the Queue #
TimePut = []
TimeGet = []

for z in range(100):

    t = time.time()
    Queue1.put(b'Test message')
    TimePut.append(time.time()-t)

    t = time.time()
    Queue1.get()
    TimeGet.append(time.time()-t)


AvgGet = sum(TimeGet)/len(TimeGet)
AvgPut = sum(TimePut)/len(TimePut)

print(f'Average Get: {AvgGet}, Average Put: {AvgPut}')
