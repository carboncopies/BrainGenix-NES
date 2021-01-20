###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

import mysql.connector

'''
Name: Database Interface
Description: This file creates an interface from BrainGenix to mySQL.
Date-Created: 2021-01-17
'''

class DBInterface(): # Interface to MySQL database #

    def __init__(self, Logger:object, Username:str, Password:str, Host:str, Database:str):

        # Create Local Pointer #
        self.Logger = Logger

        # Establish DB Connection #
        self.DBConnection = mysql.connector.connect(user=Username, password=Password, host=Host, database=Database)
        self.Logger.Log(f'Connected To DB At {Host}')
        
        self.DBConnection.close()

