###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

import socket

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
        self.IPAddr = ZookeeperConfigDict['ZKAddr'] # Get The IP Addr from the zoomeeper dict

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
