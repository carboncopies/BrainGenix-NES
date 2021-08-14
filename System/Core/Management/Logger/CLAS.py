###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

import pymysql

'''
Name: CLAS
Description: The Centralized Logger Aggregation System is used by the leader to present a unfied system log across the entire BG cluster.
Date-Created: 2021-04-25
'''


class CentralizedLoggerAggregationSystem(): # Define Class for CLAS #

    def __init__(self, Logger:object, SystemConfiguration:dict): # Initialization Function #

        # Create Local Pointers To Logger Object #
        self.Logger = Logger
        self.SystemConfiguration = SystemConfiguration

        # Print "Welcome" Message #
        self.Logger.Log('Initializing Centralized Logger Aggregation System', 4)


        # Read Log Retention Second Number #
        self.SecondsToKeepLogs = int(SystemConfiguration['SecondsToKeepLogs'])


        # Read MYSQL Information From System Configuration Dictionary #
        self.Logger.Log('Reading CLAS Configuration Parameters From Local Configuration File', 2)
        DBUsername = str(SystemConfiguration.get('DatabaseUsername'))
        DBPassword = str(SystemConfiguration.get('DatabasePassword'))
        DBHost = str(SystemConfiguration.get('DatabaseHost'))
        DBDatabaseName = str(SystemConfiguration.get('DatabaseName'))
        self.Logger.Log('Read CLAS Configuration Parameters', 1)


        # Connect To DB Server #
        self.Logger.Log('Creating CLAS PymySQL Instance, Connecting To Database Server', 2)
        self.DatabaseConnection = pymysql.connect(
            host = DBHost,
            user = DBUsername,
            password = DBPassword,
            db = DBDatabaseName
        )
        self.Logger.Log('Created CLAS PymySQL Instance', 1)

        # Create Database Cursor #
        self.Logger.Log('Creating CLAS Daemon Cursor', 2)
        self.LoggerCursor = self.DatabaseConnection.cursor()
        self.Logger.Log('Created CLAS Cursor', 1)

        # Create Help Strings #
        self.Logger.Log('Setting CLAS mAPI Help Strings', 2)
        self.mAPI_ReadLog_Help = ''
        self.Logger.Log('Set CLAS mAPI Help Strings', 1)


        # Finalize Init #
        self.Logger.Log('Finished CLAS Initialization', 3)



    def mAPI_ReadLog(self, APIArgs): # Read Lines From The Log, Default=500Lines #

        # Get Lines Param From APIArgs Dict #
        NumLines = APIArgs['Lines']

        # Get Log Text #
        self.LoggerCursor.execute("SELECT * FROM log LIMIT %d" % int(NumLines))

        LogEntries = self.LoggerCursor.fetchall()
        CommandOutput = []

        # Convert Datetime Objects To Unix Epoch Time (UTC) #
        for LogEntry in LogEntries:

            OutputEntry = list(LogEntry)
            OutputEntry[2] = OutputEntry[2].timestamp()

            CommandOutput.append(OutputEntry)

        # Return Log Text #
        return CommandOutput
