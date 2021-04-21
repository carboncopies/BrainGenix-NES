###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

'''
Name: Subsystem Instantiator
Description: This file is used to create instances of other subsystems.
Date-Created: 2021-03-24
'''


from Zookeeper.Zookeeper import ZK

from Kafka.KafkaInterface import KafkaInterface

from Database.DatabaseInterface import DBInterface

from Diagnostics.ZKDiagnostics import CanAccessZookeeper
from Diagnostics.KafkaDiagnostics import CanAccessKafka
from Diagnostics.DatabaseDiagnostics import CanAccessDatabase


def InstantiateZK(Logger, ZookeeperHost): # Instantiates Zookeeper #

    # Log Info #
    Logger.Log('Instantiating Zookeeper Interface')

    # Instantiate Zookeeper #
    try:

        # Attempt Normal Connection #
        Zookeeper = ZK(Logger)
        Zookeeper.ConnectToZookeeper(Logger, ZookeeperHost)
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
        CanAccessZookeeper(ZookeeperHost, Logger)
        exit()


def InstantiateKafka(Logger, KafkaHost): # Instantiates Kafka #

    # Log Message #
    Logger.Log('Instantiating Kafka Interface')


    # Instantiate Kafka #
    try:

        KafkaInterfaceInstance = KafkaInterface(Logger, KafkaHost)

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
        CanAccessKafka(KafkaHost, Logger)
        exit()


def InstantiateDB(Logger, Username, Password, Host, Database): # Instantiates Database Interface #

    # Log Message #
    Logger.Log('Instantiating Database Interface')


    # Instantiate DB #
    try:

        DBInterfaceInstance = DBInterface(Logger, Username, Password, Host, Database)

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
        CanAccessDatabase(Host, Username, Password, Database, Logger)
        exit()