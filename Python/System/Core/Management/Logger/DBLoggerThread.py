###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

import pymysql

## @package DBLoggerThread
# Name: DB Logger Thread
# Description: This system is responsible for sending logs to the database.
# Date-Created: 2021-06-18

## Transmits Logs From The Logger To The Database
class DatabaseLogTransmissionSystem(): 

    ## Init
    # @param Logger Logger object
    # @param LogBufferQueue Log Buffer Queue object
    # @param ControlQueue Control Queue object 
    # @param SystemConfiguration System Configuration Dictionary
    def __init__(self, Logger:object, LogBufferQueue:object, ControlQueue:object, SystemConfiguration:dict): 

        ## Count of DB operations to be executed before committing 
        self.OpsBeforeCommit = 64 

        ## Init Logger
        self.Logger = Logger
        self.LogBufferQueue = LogBufferQueue
        self.ControlQueue = ControlQueue
        self.Logger.Log('Initializing Database Log Transmission System', 4)

        ## Connect To DB
        DBUsername = str(SystemConfiguration.get('DatabaseUsername'))
        DBPassword = str(SystemConfiguration.get('DatabasePassword'))
        DBHost = str(SystemConfiguration.get('DatabaseHost'))
        DBDatabaseName = str(SystemConfiguration.get('DatabaseName'))

        ## Connect To Database
        self.DatabaseConnection = pymysql.connect(
            host = DBHost,
            user = DBUsername,
            password = DBPassword,
            db = DBDatabaseName
        )

        ## Create Database Cursor
        self.LoggerCursor = self.DatabaseConnection.cursor()
        

    ## Check If Thread Should Exit
    def ShouldStop(self):
        return not self.ControlQueue.empty()

    ## Thread To Transmit Logs To DB
    def __call__(self):  #

        OpsSinceLastCommit = 0

        while not self.ShouldStop():

            ## Read Log Item
            LogDataDict = self.LogBufferQueue.get()

            ## Decode Dict
            Level = LogDataDict['LogLevel']
            LogTime = LogDataDict['LogDateTime']
            CallingModuleName = LogDataDict['CallingModule']
            CallingFunctionName = LogDataDict['FunctionName']
            Message = LogDataDict['LogOutput']
            Node = LogDataDict['Node']
            Thread = LogDataDict['Thread']

            ## Transmit Log Line
            insertStatement= ("INSERT INTO log(LogLevel,LogDatetime,CallingModule,FunctionName,LogOutput,Node,Thread) VALUES (%s, %s, \"%s\", \"%s\", \"%s\", \"%s\",%s)")
            val = (Level, str(LogTime), CallingModuleName.split("/")[-1].split(".")[0], CallingFunctionName, Message, Node, Thread)
            self.LoggerCursor.execute(insertStatement, val)

            ## Commit if enough operations were done
            OpsSinceLastCommit += 1
            if OpsSinceLastCommit == self.OpsBeforeCommit:
                self.DatabaseConnection.commit()
                OpsSinceLastCommit = 0


        ## Log System Shutdown
        self.Logger.Log('Shutting Down Database Log Transmission Daemon', 5)

        self.Logger.Log('Commiting Pending pymysql Transactions', 3)
        self.DatabaseConnection.commit()
        self.Logger.Log('Committed Pending pymysql Transactions', 2)

        self.Logger.Log('Closing pymysql Connection', 3)
        self.DatabaseConnection.close()
        self.Logger.Log('Closed pymysql Connection', 2)

        self.Logger.Log('Database Log Transmission System Shutdown Complete', 4)
