###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

## @package UserManagement
# Name: User Management API
# Description: This subsystem exposes management API functionality to the user auth system.
# Date-Created: 2021-08-13

import secrets

## User Management Class
class UserManagementSystem():

    ## Initialization Function
    # @param SocketAPI Socket Pointer
    def __init__(self, SocketAPI):

        ## Create Local Pointers
        self.SocketAPI = SocketAPI

    ## Create User Statement
    # @param APIArgs New User Arguments passed from UI to mAPI
    def mAPI_CreateUser(self, APIArgs):

        ## Get User Info
        UserName = APIArgs['Username']
        Password = APIArgs['Password']
        FirstName = APIArgs['FirstName']
        LastName = APIArgs['LastName']
        Notes = APIArgs['Notes']
        PermissionLevel = APIArgs['PermissionLevel']

        ## Create Salt Token
        Salt = secrets.token_urlsafe(65535)

        ## Add User To DB
        self.SocketAPI.addUser(UserName, Password, Salt, FirstName, LastName, Notes, PermissionLevel)
