###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

'''
Name: User Management API
Description: This subsystem exposes management API functionality to the user auth system.
Date-Created: 2021-08-13
''' 

import secrets

class UserManagementSystem(): # User Management Class #

    def __init__(self, SocketAPI): # Initialization Function #
        
        # Create Local Pointers #
        self.SocketAPI = SocketAPI

    def mAPI_CreateUser(self, APIArgs): # Create User Statemenet #
        
        # Get User Info #
        UserName = APIArgs['Username']
        Password = APIArgs['Password']
        FirstName = APIArgs['FirstName']
        LastName = APIArgs['LastName']
        Notes = APIArgs['Notes']
        PermissionLevel = APIArgs['PermissionLevel']

        # Create Salt Token #
        Salt = secrets.token_urlsafe(65535)

        # Add User To DB #
        self.SocketAPI.addUser(UserName, Password, Salt, FirstName, LastName, Notes, PermissionLevel)
