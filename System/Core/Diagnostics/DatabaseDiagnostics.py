###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

'''
Name: Database Diagnostics
Description: This file is used to check if a Database service is up and availiable at a given address.
Date-Created: 2021-03-19
'''

import subprocess
import os
import socket
import pymysql


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


def CanAccessDatabase(Host, DatabaseConfig:dict): # Runs Some Diagnostics About The Database Connection #

    # Extract Values From Dictionary #
    Username = str(DatabaseConfig.get('DatabaseUsername'))
    Password = str(DatabaseConfig.get('DatabasePassword'))
    Host = str(DatabaseConfig.get('DatabaseHost'))
    Database = str(DatabaseConfig.get('DatabaseName'))


    # Diagnostic Message #
    Logger.Log('Starting Database Connection Diagnostic Tool', 1)


    # Seperate Host Port And Address #
    Address = Host.split(':')[0]
    if len(Host.split(':')) == 2:
        Port = Host.split(':')[1]
    else:
        Port = None


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
            Logger.Log('Invalid Number In Database Host Address Octet! Check Configuration File.', 3)
            return False
    Logger.Log('Octets Valid, Advancing To Next Test', 1)

    # Check If Device Is Reachable #
    Logger.Log(f'Checking If Database Host Server Is Reachable At {Address}', 1)
    PingResult = CheckPing(Address)

    if not PingResult:
        Logger.Log('System Unreachable, Please Check Your Config File Or Database Installation!', 3)
        return False
    else:
        Logger.Log('System Reachable, Advancing To Next Test', 1)

    # Check If Port In Allowed Range #
    if Port != None:
        Logger.Log('Checking If Port In Allowed Range (0-65535)', 1)
        if not ((int(Port) > 0) and (int(Port) < 65536)):
            Logger.Log('Port Outside Allwed Range, Please Check Configuration File')
            return False
        Logger.Log('Port Within Valid Range, Advancing To Next Test')

    # Check If Host Has Port Open #
    if Port != None:
        Logger.Log('Checking If Remote Host Has Port Open')
        if not IsPortOpen(Address, Port):
            Logger.Log(f'Address {Address} Does Not Have Port {Port} Open, Check Configuration Or Database Service!', 3)
            return False
        else:
            Logger.Log('Port Open, Advancing To Next Test', 1)

    # Attempt Database Connection #
    Logger.Log('Attempting Database Connection')
    try:
        pymysql.connect(host=Host, user=Username, password=Password, db=Database)
    except Exception as E:
        Logger.Log('A Fatal Error Has Been Asserted, Please See Line Below For More Information:', 3)
        Logger.Log(E, 3)
        return False

    Logger.Log('All Tests Passed, Please Check For Intermittent Problems Such As Networking Issues, Or DB Configuration Issues')

    # Return True, After All Tests Passed #
    return True