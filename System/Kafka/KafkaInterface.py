###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

'''
Name: Kafka Interface
Description: This is the main interface between BrainGenix and the Kafka server system.
Date-Created: 2021-03-18
'''

import kafka
import time
import random

from kafka import KafkaConsumer
from kafka import KafkaProducer
from kafka.admin import KafkaAdminClient
from kafka.admin import NewTopic


class KafkaInterface(): # Defines a class used to communicate with Kafka from BrainGenix #


    def __init__(self, Host):

        # Store Host For Later Use #
        self.Host = Host

        # Initialize Kafka Interfaces #
        self.KafkaProducer = KafkaProducer(bootstrap_servers = Host)
        self.KafkaConsumer = KafkaConsumer(bootstrap_servers = Host)


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





K = KafkaInterface('127.0.0.1:9092')
K.CreateTopics(['test1','test2'])