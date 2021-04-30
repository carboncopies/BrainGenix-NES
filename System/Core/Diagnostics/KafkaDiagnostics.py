###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

'''
Name: Kafka Diagnostics
Description: This file is used to check if a Kafka service is up and availiable at a given address.
Date-Created: 2021-03-19
'''

import subprocess
import os
import socket

from confluent_kafka import Consumer
from confluent_kafka import Producer
from confluent_kafka.admin import AdminClient


def IsPortOpen(Address,Port): # Checks If A Given Port Is Open #

    # Create Socket #
    S = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    # Check Conditionals #
    try:
        S.connect((Address, int(Port)))
        S.shutdown(2)
        return True
    except:
        return False


def CheckPing(Host): # Returns True Or False If A Given Address Is Reachable #

    # Ping Command #
    Command = ['ping', '-c', '1', Host]

    # Redirect Ping Text To Nothing #
    Devnull = open(os.devnull, 'w')

    # Return Output #
    if not subprocess.call(Command, stderr=Devnull, stdout=Devnull):
        return True
    else:
        return False


def CanAccessKafka(KafkaConfigDict:dict, Logger:dict): # Runs Some Diagnostics About The Kafka Connection #


    # Extract Values From Dictionary #
    Host = str(KafkaConfigDict.get('KafkaHost'))
    Port = str(KafkaConfigDict.get('KafkaPort'))

    Host += f':{Port}'


    # Diagnostic Message #
    Logger.Log('Starting Kafka Connection Diagnostic Tool', 1)

    # Seperate Host Port And Address #
    Address = Host.split(':')[0]
    Port = Host.split(':')[1]

    # Check If Address Valid #
    Logger.Log('Checking If Address Has Correct Number Of Octets', 1)
    Octets = Address.split('.')
    if len(Octets) != 4:
        Logger.Log('Address Invalid Octet Count, Please Use A Valid IPv4 Address! Check Configuration File.', 3)
        return False
    else:
        Logger.Log('Address Valid Octet Count, Advancing To Next Test', 1)


    # Check If Address Valid Numbers #
    Logger.Log('Checking If Address Has Valid Numbers In Octets', 1)
    for Octet in Octets:
        if (int(Octet) > 255) or (int(Octet) < 0):
            Logger.Log('Invalid Number In Kafka Host Address Octet! Check Configuration File.', 3)
            return False
    Logger.Log('Octets Valid, Advancing To Next Test', 1)

    # Check If Device Is Reachable #
    Logger.Log(f'Checking If Kafka Host Server Is Reachable At {Address}', 1)
    PingResult = CheckPing(Address)

    if not PingResult:
        Logger.Log('System Unreachable, Please Check Your Config File Or Kafka Installation!', 3)
        return False
    else:
        Logger.Log('System Reachable, Advancing To Next Test', 1)

    # Check If Port In Allowed Range #
    Logger.Log('Checking If Port In Allowed Range (0-65535)', 1)
    if not ((int(Port) > 0) and (int(Port) < 65536)):
        Logger.Log('Port Outside Allwed Range, Please Check Configuration File')
        return False
    Logger.Log('Port Within Valid Range, Advancing To Next Test')

    # Check If Host Has Port Open #
    Logger.Log('Checking If Remote Host Has Port Open')
    if not IsPortOpen(Address, Port):
        Logger.Log(f'Address {Address} Does Not Have Port {Port} Open, Check Configuration Or Kafka Service!', 3)
        return False
    else:
        Logger.Log('Port Open, Advancing To Next Test', 1)

    # Attempt Kafka Consumer Connection #
    Logger.Log('Attempting Consumer Connection')
    try:
        Configuration = {'bootstrap.servers' : Host}
        Consumer(Configuration)
        Logger.Log('Kafka Consumer Connection Test Passed')
    except ValueError: # NOTE: A value error is expected as we've not sent it a topic name to connect with #
        Logger.Log('Kafka Consumer Connection Test Passed')
    except Exception as E:
        Logger.Log('An Unknown Error Was Detected With Consumer Test! See Line Below.', 3)
        Logger.Log(E, 3)
        return False

    # Attempt Kafka Producer Connection #
    Logger.Log('Attempting Producer Connection')
    try:
        Configuration = {'bootstrap.servers' : Host}
        Producer(Configuration)
        Logger.Log('Kafka Producer Connection Test Passed')
    except Exception as E:
        Logger.Log('An Unknown Error Was Detected With Producer Test! See Line Below.', 3)
        Logger.Log(E, 3)
        return False

    # Attempt Kafka Admin Connection #
    Logger.Log('Attempting Admin Connection')
    try:
        Configuration = {'bootstrap.servers' : Host}
        AdminClient(Configuration)
        Logger.Log('Kafka Admin Connection Test Passed')
    except Exception as E:
        Logger.Log('An Unknown Error Was Detected With Kafka Admin Test! See Line Below.', 3)
        Logger.Log(E, 3)
        return False

    Logger.Log('All Tests Passed, Please Check For Intermittent Problems Such As Networking Issues')

    # Return True, After All Tests Passed #
    return True