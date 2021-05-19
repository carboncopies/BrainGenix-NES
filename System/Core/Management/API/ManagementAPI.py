###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

import socket
import threading
import json

'''
Name: Management API
Description: This file provides the socket based interface to the management api backend.
Date-Created: 2021-05-17
'''


class ManagementAPISocketServer(): # Creates A Class To Connect To The Management API #

    def __init__(self, Logger, MAPIConfig:dict, ZookeeperConfigDict:dict): # This function initialializes sockets #

        # Get Config Params #
        self.Logger = Logger
        self.Port = MAPIConfig['Port'] # Get the port from the port config
        self.IPAddr = ZookeeperConfigDict['ZKHost'] # Get The IP Addr from the zoomeeper dict

        # Create Socket Host Variable #
        self.Logger.Log('Creating Host Variable')
        self.SocketHost = (self.IPAddr, self.Port)

        # Bind To Port #
        self.Logger.Log('Binding To Host')
        self.Socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.Socket.bind(self.SocketHost)

        # Listen For Connections #
        self.Logger.Log('Management API Backend Listening For Incoming Connections')
        self.Socket.listen()

        # Create MAPI Thread #
        self.Logger.Log('Starting Management API Server Thread')
        self.Thread = threading.Thread(target=self.ManagementAPIThread, args=())
        self.Thread.start()


    def ManagementAPIThread(self): # Create A Thread Function For The Management API #

        # Enter Connection Accept Loop #
        while True:

            # Log That Server Awaiting Connections #
            self.Logger.Log(f'MAPI Server Awaiting Connections On Port: {self.Port}')


            # Wait Accept Incoming Connections #
            self.Connection, self.ConnectionInformation = self.Socket.accept()
            self.Logger.Log(f'Management API Recieved Connection From: {self.ConnectionInformation}')

            # Enter Listening Loop To Recieve Commands #
            while True:

                # Get Command From Client #
                self.Command = self.Connection.recv(65535)
                self.Command = self.Command.decode()

                # Convert To Dict From JSON #
                self.Command = json.loads(self.Command)


                # Check That Command Syntax Is Correct #
                if str(type(self.Command)) != "<class 'dict'>":
                    self.Connection.send(json.dumps({"Response": "INVALID DICTIONARY FORMAT"}).encode())


                if 'CallStack' not in self.Command:
                    self.Connection.send(json.dumps({"Response": "COMMAND DOES NOT INCLUDE 'CallStack' FIELD"}).encode())

                # Check If Disconnect (Must be before SysName check to remain client agnostic) #
                if self.Command['CallStack'] == 'Disconnect':
                    self.Logger.Log('Client Initiated MAPI Disconnect, Connection Closed')
                    self.Connection.close()
                    break


                if 'SysName' not in self.Command:
                    self.Connection.send(json.dumps({"Response": "COMMAND DOES NOT INCLUDE 'SysName' FIELD"}).encode())

                if 'KeywordArgs' not in self.Command:
                    self.Connection.send(json.dumps({"Response": "COMMAND DOES NOT INCLUDE 'KeywordArgs' FIELD"}).encode())


                if self.Command['SysName'] != 'NES':
                    self.Connection.send(json.dumps({"Response": "INVALID VALUE FOR 'SysName' FIELD"}).encode())

                
                # Built in Commands #
                if self.Command['CallStack'] == 'TestAPI':
                    self.Connection.send(json.dumps({"Response": "but most of all, samy is my hero"}).encode())



    def Quit(self): # Release The Socket #

        # Close The Socket #
        self.Socket.close()