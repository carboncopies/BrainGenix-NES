###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

'''
Name: Kafka Interface
Description: This is the main interface between BrainGenix and the Kafka server system.
Date-Created: 2021-03-18
'''

import kafka

#from kafka import KafkaConsumer
#from kafka import KafkaProducer


# # Producer = KafkaProducer(bootstrap_servers='localhost:9092')
# # for z in range(100):
# #     Producer.send('topicname', f'{z}'.encode('utf-8'))

# Consumer = KafkaConsumer(bootstrap_servers='localhost:9092')
# #Consumer.subscribe('topicname')

# print(Consumer)
# for message in Consumer:

#     print(message)


from kafka import KafkaConsumer
from kafka import TopicPartition
consumer = KafkaConsumer(bootstrap_servers='localhost:9092')
consumer.assign([TopicPartition('topicname', 0)])
for msg in consumer:
    print(msg)
