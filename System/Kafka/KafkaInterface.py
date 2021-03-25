###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

'''
Name: Kafka Interface
Description: This is the main interface between BrainGenix and the Kafka server system.
Date-Created: 2021-03-18
'''

import multiprocessing
import atexit

from confluent_kafka import Producer
from confluent_kafka import Consumer
from confluent_kafka import TopicPartition


# import random






# import threading, time

# def t():
#     print('Thread Started')
#     p = Producer({'bootstrap.servers':'localhost:9092'})
#     msg = random.randint(1000000000000,57849327543802754839257438905743829057438957432908476829564397256437825647328956437285643728564378256437895432969275689786875430898534288534287342578245073248705870942349786253498)
#     for _ in range(10):
#         msg += msg
#     msg = str(msg).encode('utf-8')
#     time.sleep(0.1)
#     for z in range(20000):
#         t = time.time()
#         p.produce('Test2', msg)
#         print(time.time()-t)

#     p.flush()

# t2 = threading.Thread(target=t, args=())



class ConsumerQueue():

    def __init__(self, Logger:object, Host:str, TopicName:str, Queue:object, GroupID:str='0', Timeout:int = 6000, Offset:str='None'):

        # Create Local Vars #
        self.TopicName = TopicName
        self.TopicPartition = TopicPartition
        self.Host = Host
        self.GroupID = GroupID
        self.Offset = Offset
        self.Timeout = Timeout

        self.Logger = Logger

        # Create Queue #
        self.Queue = Queue

        # Create Consumer #
        Configuration = {'bootstrap.servers': self.Host,
                        'group.id': self.GroupID,
                        'session.timeout.ms': self.Timeout,
                        }

        if Offset != 'None':
            Configuration.update({'auto.offset.reset': Offset})


        self.ConsumerInstance = Consumer(Configuration)

        self.ConsumerInstance.subscribe([TopicName])


        # Register Exit Function #
        atexit.register(self.Shutdown, self)

        # Start Main Loop #
        self.Main()


    def Main(self): # Main Loop For Process #

        # Enter Main Loop #
        while True:

            self.ConsumerInstance.subscribe([self.TopicName])
            Message = self.ConsumerInstance.poll(timeout=1.0)

            if Message is None:
                continue
        
            if Message.error():
                self.Logger.Log(f'Exception In KafkaConsumerObject: {Message.error()}', 1)

            else:
                
                self.Queue.put(Message.value())


    def Shutdown(self): # Destroys Connection Object #

        # Shutdown Connection #
        self.ConsumerInstance.close()
        self.Logger.Log(f'Consumer Instance Exited At Address {str(hex(id(self)))}')


class ProducerQueue(): # Provides A Queue Based Interface To Send Messages To The Kafka Broker At The Specified Topic #

    def __init__(self, Logger:object, Host:str, TopicName:str, Queue:object, Partition:int): # Initialization #

        # Create Local Vars #
        self.Logger = Logger
        self.Host = Host
        self.TopicName = TopicName
        self.Queue = Queue
        self.Partition = Partition

        # Create Producer Object # 
        Configuration = {'bootstrap.servers' : Host}
        self.ProducerObject = Producer(Configuration)

        # Register Exit Function #
        atexit.register(self.Shutdown, self)

        # Launch Queue Handler #
        self.QueueHandler()


    def QueueHandler(self): # Queue Handler Function #

        # Enter Main Loop # 
        while True:

            # Produce Message #
            Message = Queue.get()
            self.ProducerObject.Produce(self.TopicName, Message)


    def Shutdown(self): # Destroys Connection Object #

        # Shutdown Connection #
        self.ProducerObject.close()
        self.Logger.Log(f'Producer Instance Exited At Address {str(hex(id(self)))}')


class KafkaInterface(): # Provides An Interface To Kafka Via The Confluent-Kafka Module #

    def __init__(self, Logger:object, Host:object): # Initialize Iface #

        # Create Local Vars # 
        self.Logger = Logger
        self.Host = Host

        # Create List For Processes and Queues #
        self.ConsumerProcesses = []
        self.ProducerProcesses = []

        self.ConsumerQueues = []
        self.ProducerQueues = []

        # Register Shutdown Function #
        atexit.register(self.Shutdown, self)


    def CreateConsumer(self, TopicName:str, GroupID:str = '0', TimeoutMilliseconds:int = 6000, Offset='None'): # Creates Kafka Consumer #

        # Create Queue #
        QueueInstance = multiprocessing.Queue()

        # Create And Instantiate Process/Interface #
        ConsumerProcess = multiprocessing.Process(target=ConsumerQueue, args=(self.Logger, self.Host, TopicName, GroupID, TimeoutMilliseconds, Offset, ))
        ConsumerProcess.start()

        # Append Instances #
        self.ConsumerProcesses.append(ConsumerProcess)
        self.ConsumerQueues.append(QueueInstance)

        # Return Queue #
        return QueueInstance


    def CreateProducer(self, TopicName:str, Partition:int = 0): # Creates Kafka Producer #

        # Create Queue #
        QueueInstance = multiprocessing.Queue()

        # Create And Instantiate Process/Interface #
        ProducerProcess = multiprocessing.Process(target=ProducerQueue, args=(self.Logger, self.Host, TopicName, QueueInstance, Partition, ))
        ProducerProcess.start()

        # Append Instances #
        self.ProducerProcesses.append(ProducerProcess)
        self.ProducerQueues.append(QueueInstance)

        # Return Functions # 
        return QueueInstance


    def Shutdown(self): # Destroys Connection Objects # 

        # Shutdown Processes # 
        self.Logger.Log(f'Shutting Down {len(self.ConsumerProcess) + len(self.ProducerProcesses)} Processes')
        
        for Process in self.ConsumerProcesses:
            self.Logger.Log(f'Terminating Process {Process}')
            Process.terminate()

        
