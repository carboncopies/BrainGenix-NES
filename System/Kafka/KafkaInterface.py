###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

'''
Name: Kafka Interface
Description: This is the main interface between BrainGenix and the Kafka server system.
Date-Created: 2021-03-18
'''

from kafka import KafkaConsumer
from kafka import KafkaProducer
from kafka.admin import KafkaAdminClient
from kafka.admin import NewTopic


class KafkaInterface(): # Defines a class used to communicate with Kafka from BrainGenix #


    def __init__(self, Host, Logger): # Initializes the Kafka Interface to BrainGenix #

        # Store Host For Later Use #
        self.Host = Host
        self.Logger = Logger


        # Initialize Kafka Interfaces #
        self.KafkaProducer = KafkaProducer(bootstrap_servers = Host)
        self.KafkaConsumer = KafkaConsumer(bootstrap_servers = Host)

        # Log Connection #
        self.Logger.Log('Established Connection To Kafka Server')


    def CreateTopics(self, TopicNames): # Creates a topic or Queue

        # Connect To Kafka #
        AdminClient = KafkaAdminClient(bootstrap_servers=self.Host)

        # Create Topic List #
        TopicsList = []
        for TopicName in TopicNames:
            TopicObject = NewTopic(TopicName, num_partitions=1, replication_factor=1)
            TopicsList.append(TopicObject)

        # Create Topics #
        AdminClient.create_topics(new_topics=TopicsList, validate_only=False)


    def CreateProducerObject(self, TopicName): # Creates A Producer Object #

        # Create Object #
        KafkaProducerObject = KafkaProducer(TopicName, bootstrap_servers = self.Host)

        # Return Object #
        return KafkaProducerObject


    def CreateConsumerObject(self, TopicName): # Creates A Consomer Object #

        # Create Object #
        KafkaConsumerObject = KafkaConsumer(TopicName, bootstrap_servers = self.Host)

        # Return Object #
        return KafkaConsumerObject