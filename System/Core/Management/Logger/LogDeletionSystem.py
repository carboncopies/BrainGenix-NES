###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

import datetime
import pymysql
import threading

'''
Name: LogAutoDeletionSystem
Description: The LogAutodeletionSystem automatically purges logs that are older than the defined retention period (see config.yaml).
Date-Created: 2021-07-15
'''
 
class LogAutoDeletionSystem(): # Create Class To Hold Log Auto Deletion System #

    def __init__(self, Logger:object, ControlQueue: object, SystemConfiguration:dict): # Initialize The System #

        # Create Local Variables #
        self.Logger = Logger
        self.ControlQueue = ControlQueue
        self.SystemConfiguration = SystemConfiguration

        # Log Init Start #
        self.Logger.Log('Initializing Log AutoDeletion Thread', 3)

        # Create Thread Object #
        self.ThreadObject = threading.Thread(
            target=self.DeleteLogThread, 
            args=(), 
            name='Log Auto Deletion System'
        )


    def DeleteLogThread(self): # Thread Target That Deletes Old Logs #

        # Log Initialization #
        self.Logger.Log('Log Deletion Thread Created')

        # Connect To MySQL Server #
        self.Logger.Log('Reading LADS AutoDeletion Configuration Parameters From Local Configuration File', 2)
        DBUsername = str(self.SystemConfiguration.get('DatabaseUsername'))
        DBPassword = str(self.SystemConfiguration.get('DatabasePassword'))
        DBHost = str(self.SystemConfiguration.get('DatabaseHost'))
        DBDatabaseName = str(self.SystemConfiguration.get('DatabaseName'))
        self.Logger.Log('Read LADS Configuration Parameters', 1)


        # Connect To DB Server #
        self.Logger.Log('Creating LADS PymySQL Instance, Connecting To Database Server', 2)
        self.DatabaseConnection = pymysql.connect(
            host = DBHost,
            user = DBUsername,
            password = DBPassword,
            db = DBDatabaseName
        )
        self.Logger.Log('Created LADS PymySQL Instance', 1)

        # Create Database Cursor #
        self.Logger.Log('Creating LADS Daemon Cursor', 2)
        self.LoggerCursor = self.DatabaseConnection.cursor()
        self.Logger.Log('Created LADS Cursor', 1)

        # Enter Polling Loop #
        while self.ControlQueue.empty():
