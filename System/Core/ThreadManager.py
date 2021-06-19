###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################


import queue
import threading
import atexit

'''
Name: Thread Manager
Description: This system destroys threads upon system exit.
Date-Created: 2021-06-18
'''

from Core.Internode.Zookeeper.Zookeeper import ZK

from Core.Internode.Kafka.KafkaInterface import KafkaInterface

from Core.Internode.Database.ModuleInstanceManager import DatabaseInstanceCreator

from Core.Diagnostics.ZKDiagnostics import CanAccessZookeeper
from Core.Diagnostics.KafkaDiagnostics import CanAccessKafka
from Core.Diagnostics.DatabaseDiagnostics import CanAccessDatabase

from Core.Management.Logger.Logger import SysLog



class ThreadManager(): # This Class Manages Threads #

    def __init__(self, Logger): # Initialization #

        # Create Local Pointers #
        self.Logger = Logger


        # Create Control Queue List #
        self.ControlQueues = []

        # Create Thread List #
        self.Threads = []

        # Create Local Var #
        self.ThreadsDestroyed = False

        # Register Exit Function #
        atexit.register(self.ShutdownSystem)



    def InstantiateZK(self, Logger, ZookeeperConfig): # Instantiates Zookeeper #

        # Log Info #
        Logger.Log('Instantiating Zookeeper Interface')

        # Instantiate Zookeeper #
        try:

            # Attempt Normal Connection #
            Zookeeper = ZK(Logger)
            Zookeeper.ConnectToZookeeper(Logger, ZookeeperConfig)
            Zookeeper.AutoInitZKLeader()
            Zookeeper.SpawnCheckerThread()

            # Log Success #
            Logger.Log('Zookeeper Interface Instantiation Successful')

            # Return ZK Instance #
            return Zookeeper

        # If Something Fails During Instantiation #
        except Exception as E:

            # Print Exception Message #
            Logger.Log('Error During Zookeeper Instantation', 3)
            Logger.Log(f'Exception: {E}; Running Zookeeper Diagnostics!', 3)

            # Run Diagnostics #
            CanAccessZookeeper(ZookeeperConfig, Logger) ###############################################################################
            exit()


    def InstantiateInternodeQueue(self, Logger, InternodeConfigDict): # Instantiates Kafka #

        # Log Message #
        Logger.Log('Instantiating Internode Queue Subsystem')

        # Read Mode From File #
        Logger.Log('Reading Configuration File')
        QueueType = InternodeConfigDict['Type']


        # Check Queueing System Type #
        if QueueType == 'Kafka':

            Logger.Log('Internode Queue Subsystem Backend Has Been Set To Kafka')

            # Instantiate Kafka #
            try:

                KafkaInterfaceInstance = KafkaInterface(Logger, InternodeConfigDict)

                # Log Success #
                Logger.Log('Kafka Interface Instantiation Successful')

                # Return Instantiated Kafka Interface Object #
                return KafkaInterfaceInstance

            # If Something Fails During Instantiation #
            except Exception as E:

                # Print Exception Message #
                Logger.Log('Error During Kafka Instantiation', 3)
                Logger.Log(f'Exception: {E}; Running Kafka Diagnostics!', 3)

                # Run Diagnostics #
                CanAccessKafka(InternodeConfigDict, Logger)
                exit()

        elif QueueType == 'Socket':

            Logger.Log('Internode Queue Subsystem Backend Has Been Set To Sockets')

            # Instantiate Socket System #


        else:

            # If It's An Unsupported Backend #
            Logger.Log('Unknown Backend For Queue Subsystem! Please Check Your Configuration File.', 3)

            # Terminate System #
            exit()


    def InstantiateDB(self, Logger, DBConfig): # Instantiates Database Interface #

        # Log Message #
        Logger.Log('Instantiating Database Interface')


        # Instantiate DB #
        try:


            # Create Queues for DBInstance Manager #
            ControlQueue = queue.Queue()

            # Instantiate DatabaseInstanceManager #
            self.DBInstanceManager = DatabaseInstanceCreator(Logger, DBConfig, ControlQueue)

            # Append Control Queue To Control Queues #
            self.ControlQueues.append(ControlQueue)


            # Start Pymysql Instance creator #
            self.DBInstanceManagerThread = threading.Thread(target=self.DBInstanceManager.__call__, args=())
            self.DBInstanceManagerThread.name = "DBInstanceUpdate"
            self.DBInstanceManagerThread.start()

            
            # Append Thread To Stoplist #
            self.Threads.append(self.DBInstanceManagerThread)


            # Log Success #
            Logger.Log('Database Connector Created Successfully')

            # Return Instantiated DB Interface Object #
            return self.DBInstanceManagerThread

        # If Something Fails During Instantiation #
        except Exception as E:

            # Print Exception Message #
            Logger.Log('Error During DB Instantiation', 3)
            Logger.Log(f'Exception: {E}; Running Database Diagnostics!', 3)

            # Run Diagnostics #
            CanAccessDatabase(DBConfig, Logger)
            exit()



    def ShutdownSystem(self): # Shuts Down Threads #

        # Check If Threads Already Destroyed #
        if not self.ThreadsDestroyed:

            # Log Shutdown Initiated #
            self.Logger.Log('System Shutdown Initiated', 3)

            # Send Shutdown Message To Control Queues #
            self.Logger.Log('Sending Shutdown Signal To Threads', 2)
            for ControlQueueIndex in range(len(self.ControlQueues)):

                # Log Sending "Stawp!" #
                self.Logger.Log(f'Sending Term Signal To Thread', 0)

                # Send Thread Term Signal #
                self.ControlQueues[ControlQueueIndex].put('stawp!')

                # Log "Stawp!" Sent #
                self.Logger.Log(f'Sent Stop Signal To Queue {ControlQueueIndex + 1}/{len(self.ControlQueues)}', 0)

            # Log Task Completion #
            self.Logger.Log(f'Stop Signals Sent To Thread {ControlQueueIndex + 1}', 2)


            # Merge Threads #
            self.Logger.Log('Joining Threads', 2)
            for ThreadIndex in range(len(self.Threads)):

                # Log Join Init #
                self.Logger.Log(f'Joining Thread {ThreadIndex + 1}/{len(self.Threads)}', 0)

                # Join Thread #
                self.Threads[ThreadIndex].join()

                # Log Join Completion #
                self.Logger.Log(f'Joined Thread {ThreadIndex + 1}', 0)
            
            # Log Task Completion #
            self.Logger.Log('Thread Joining Complete', 2)

            # Delete Thread Dict #
            self.Logger.Log('Deleting Thread List', 2)
            del self.Threads
            self.Logger.Log('Thread List Deleted', 1)

            # Delete Queues #
            self.Logger.Log('Deleting Control Queue List', 2)
            del self.ControlQueues
            self.Logger.Log('Control Queue List Deleted', 1)

            # Set Threads Destroyed To True #
            self.ThreadsDestroyed = True