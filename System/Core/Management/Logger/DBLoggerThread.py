###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

import threading
import pymysql

'''
Name: DB Logger Thread
Description: This system is responsible for sending logs to the database.
Date-Created: 2021-06-18
''' 


class DatabaseLogTransmissionSystem(): # Transmits Logs From The Logger To The Database #

    def __init__(self, Logger:object, LogBufferQueue:object, DatabaseConfiguration:dict): # Init #

        # Init Logger #
        self.Logger = Logger
        self.Logger.Log('Initializing Database Log Transmission System', 4)

        # Create Thread #
        self.Logger.Log('Creating Database Log Transmission System Thread', 3)
        self.DatabaseLogTransmissionSystemThread = threading.Thread(target=self.__call__, args=(LogBufferQueue, DatabaseConfiguration,))
        self.DatabaseLogTransmissionSystemThread.name = 'DB Log Transmission'
        self.Logger.Log('Starting Thread for Database Log Transmission System', 3)
        self.DatabaseLogTransmissionSystemThread.start()


    def ShouldStop():
        # TODO: implement
        pass


    def __call__(self, LogBufferQueue:object, DatabaseConfig:dict): # Thread To Transmit Logs To DB #

        # Connect To DB #
        DBUsername = str(DatabaseConfig.get('DatabaseUsername'))
        DBPassword = str(DatabaseConfig.get('DatabasePassword'))
        DBHost = str(DatabaseConfig.get('DatabaseHost'))
        DBDatabaseName = str(DatabaseConfig.get('DatabaseName'))


        # Connect To Database #
        self.DatabaseConnection = pymysql.connect(
            host = DBHost,
            user = DBUsername,
            password = DBPassword,
            db = DBDatabaseName
        )

        # Create Database Cursor #
        self.LoggerCursor = self.DatabaseConnection.cursor()


        # Enter Loop #
        while not self.ShouldStop():
            # Read Log Item #
            LogDataDict = LogBufferQueue.get()

            # Decode Dict #
            Level = LogDataDict['LogLevel']
            LogTime = LogDataDict['LogDatetime']
            CallingModuleName = LogDataDict['CallingModule']
            CallingFunctionName = LogDataDict['FunctionName']
            Message = LogDataDict['LogOutput']
            Node = LogDataDict['Node']


            # Transmit Log Line #
            insertStatement= ("INSERT INTO log(LogLevel,LogDatetime,CallingModule,FunctionName,LogOutput,Node) VALUES (%s, %s, \"%s\", \"%s\", \"%s\", \"%s\")")
            val = (Level, str(LogTime), CallingModuleName.split("/")[-1].split(".")[0], CallingFunctionName, Message, Node)
            self.LoggerCursor.execute(insertStatement,val)

