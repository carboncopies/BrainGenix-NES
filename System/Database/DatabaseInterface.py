###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

#import mysql.connector
import pymysql
import encryption

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
        self.DBConnection = pymysql.connect(host=Host, user=Username, password=Password, db=Database)
        
        #Encrypt the host before logging
        Host= encryption.encrypt(Logger:object, Host:str)
        
        self.Logger.Log(f'Connected To DB At {Host}')
        
        self.DBConnection.close()

