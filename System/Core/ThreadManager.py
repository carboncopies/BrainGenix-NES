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

    def __init__(self): # Initialization #

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
            self.DBInstanceUpdateControlQueue = queue.Queue()

            # Instantiate DatabaseInstanceManager #
            self.DBInstanceManager = DatabaseInstanceCreator(Logger, DBConfig, self.DBInstanceUpdateControlQueue)


            # Start Pymysql Instance creator #
            self.DBInstanceManagerThread = threading.Thread(target=self.DBInstanceManager.__call__, args=())
            self.DBInstanceManagerThread.name = "DBInstanceUpdate"
            self.DBInstanceManagerThread.start()

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


    def InstantiateLogger(self, DBConfig, LoggerConfigDict): # Instantiates Kafka #

        # Log Message #
        print('Initializing Centralized Logging System')


        # Instantiate Kafka #
        try:

            Logger = SysLog(DBConfig, LoggerConfigDict)

            # Log Success #
            Logger.Log('Centralized Logging Daemon Started')

            # Return Instantiated Kafka Interface Object #
            return Logger

        # If Something Fails During Instantiation #
        except Exception as E:

            # Print Exception Message #
            ErrorMessage = ''
            ErrorMessage += 'Error During Logger Initialization!\n'
            ErrorMessage += f'Fatal Exception: {E}'

            # Save Error Output To Disk#
            with open('README-BRAINGENIX-CLS-ERROR.txt', 'w') as FileObject:
                FileObject.write(ErrorMessage)

            # Print Error Message #
            print(ErrorMessage)

            # Exit #
            exit()


    def ShutdownSystem(self): # Shuts Down Threads #

        # This part needs to have control queues added for the other threads, but for now, we'll just do the ones that support it. #

        # Send Shutdown Message To Control Queues #
        self.DBInstanceUpdateControlQueue.put('stawp!')

        # Merge Threads #
        self.DBInstanceManagerThread.join()
