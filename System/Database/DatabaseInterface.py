###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

import pymysql

'''
Name: Database Interface
Description: This file creates an interface from BrainGenix to mySQL.
Date-Created: 2021-01-17
'''

class DBInterface(): # Interface to MySQL database #

    '''
    This class is used to provide a layer of abstraction between the BrainGenix system and mysql. 
    It uses pymysql to interact with the database over sockets.
    This class is designed with some specific methods to check user information, and other such information.
    As the database changes structure, this file will also change, to add new database features.
    '''

    def __init__(self, Logger:object, Username:str, Password:str, Host:str, Database:str):

        '''
        Initializes the DB Interface by connecting to mysql.
        The connection information is passed in as arguments.
        These arguments are loaded from the local config YAML file.
        '''

        # Create Local Pointer #
        self.Logger = Logger

        # Establish DB Connection #
        self.DBConnection = pymysql.connect(host=Host, user=Username, password=Password, db=Database)

        self.Logger.Log(f'Connected To DB At {Host}')


    def CreateCursor(self): # Creates A Database Cursor #

        '''
        This method creates a cursor object as an attribute in self.
        '''

        # Create Cursor #
        self.DatabaseCursor = pymysql.cursors.Cursor()

    
    def CheckIfUserExists(self, UserName:str): # Checks If A Given User Exists #

        '''
        Returns a boolean indicating if the target username exists or does not exist.
        This method is part of the DBInterface class. 
        '''

        # Execute Some MYSQL Cursor Code #
        # something here...

        raise NotYetImplementedError


    def GetUserInformation(self, UserName:str): # Returns A List Of User Information #

        '''
        Returns some user information from the specific row of the user.
        This is going to include everything outlined in the user DB block diagram.
        The specific format of the information is a dictionary with key values being the names of the columns.
        '''

        raise NotYetImplementedError


    def ReadNeuronInformation(self, UserName:str): # ... #

        raise NotYetImplementedError