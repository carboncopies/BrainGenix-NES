###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

'''
Name: Subsystem Instantiator
Description: This file is used to create instances of other subsystems.
Date-Created: 2021-03-24
'''


from Core.Internode.Zookeeper.Zookeeper import ZK

from Core.Internode.Kafka.KafkaInterface import KafkaInterface

from Core.Internode.Database.DatabaseInterface import DBInterface

from Core.Diagnostics.ZKDiagnostics import CanAccessZookeeper
from Core.Diagnostics.KafkaDiagnostics import CanAccessKafka
from Core.Diagnostics.DatabaseDiagnostics import CanAccessDatabase

from Core.Management.Logger.Logger import SysLog


def InstantiateZK(Logger, ZookeeperConfig): # Instantiates Zookeeper #

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
        CanAccessZookeeper(ZookeeperHost, Logger) ###############################################################################
        exit()


def InstantiateInternodeQueue(Logger, InternodeConfigDict): # Instantiates Kafka #

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

        pass


    else:
        
        # If It's An Unsupported Backend #
        Logger.Log('Unknown Backend For Queue Subsystem! Please Check Your Configuration File.', 3)

        # Terminate System #
        exit()


def InstantiateDB(Logger, DBConfig): # Instantiates Database Interface #

    # Log Message #
    Logger.Log('Instantiating Database Interface')


    # Instantiate DB #
    try:

        DBInterfaceInstance = DBInterface(Logger, DBConfig)

        # Log Success #
        Logger.Log('Database Connector Created Successfully')

        # Return Instantiated DB Interface Object #
        return DBInterfaceInstance

    # If Something Fails During Instantiation #
    except Exception as E:

        # Print Exception Message #
        Logger.Log('Error During DB Instantiation', 3)
        Logger.Log(f'Exception: {E}; Running Database Diagnostics!', 3)

        # Run Diagnostics #
        CanAccessDatabase(Host, Username, Password, Database, Logger) ###############################################################################################
        exit()


def InstantiateLogger(DBConfig, LoggerConfigDict): # Instantiates Kafka #

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