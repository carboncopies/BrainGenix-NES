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

    def __init__(self, MAPIConfig:dict, ZookeeperConfigDict:dict): # This function initialializes sockets #

        # Get Config Params #
        self.Port = MAPIConfig['Port'] # Get the port from the port config
        self.IPAddr = ZookeeperConfigDict['ZKAddr'] # Get The IP Addr from the zoomeeper dict

        # Bind To Port #