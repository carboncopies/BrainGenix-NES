###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

import datetime
import inspect
import os
import pymysql
import socket

'''
Name: SysLog
Description: This class handles the main log for this braingenix instance.
Date-Created: 2020-12-19
'''



def CompressFile(FileName): # Compresses The File #

    '''
    This function is used to compress finished logfiles into a gzip file.
    The function is called whenever a log file is split after a certain number of lines.
    *DO NOT CALL THIS*
    '''

    # First, Read The File Into Ram, Then Remove The Original #
    with open(FileName, 'rb') as FileObject:

        FileText = FileObject.read()

    os.remove(FileName)


    # Write The Compressed File To The Disk #
    with gzip.open(FileName + '.gz', 'wb') as FileObject:

        FileObject.write(FileText)


def TryMakeDir(path): # Makes a Dir, catches exception if already exists #

    '''
    This function attempts to make a directory, and if it already exists, skips it.
    *DO NOT CALL THIS*
    '''

    if not os.path.exists(path):
        os.mkdir(path)



class SysLog(): # Logger Class #

    '''
    This class contains the logger used by the entire BrainGenix system.
    The logger is an essential part of the BrainGenix system, as it allows an entire cluster's log events to be aggregated into a usable format.
    The logger contains some functions which are described below.
    The most frequently used one will be the Log function, which accepts a message and a level, and puts it into the log.
    This is the only function that should be called by an external function, as calling other functions will cause undefined behavior in the logger.
    '''

    def __init__(self, DatabaseConfig:dict, ConfigFileDictionary:dict): # Connect To Database #

        '''
        This function is used when the system is starting up, and should not be called anytime after that.
        If this function is called later, it'll cause the logger to be reinitialized, which may loose the buffer.
        *DO NOT CALL THIS*
        '''


        # Extract the important values from the dictionary and return them to the main system #
        LogPath = str(ConfigFileDictionary.get('LogPath'))
        PrintLogText = bool(ConfigFileDictionary.get('PrintLogText'))
        SecondsToKeepLogs = int(ConfigFileDictionary.get('SecondsToKeepLogs'))
        ConsoleOutputEnabled = bool(ConfigFileDictionary.get('ConsoleOutputEnabled'))


        # Create Local Log Path Directory #
        DoesLogDirExist = os.path.isdir(LogPath)
        if not DoesLogDirExist:

            print('Log Path Does Not Exist, Creating Directory')
            os.mkdir(LogPath)
            print('Created Directory')


        # Initialize Local Variable Information #

        self.LogBuffer = '[Level] [               Time] [         Module Name] [           Function] [Message]\n'
        self.PrintEnabled = ConsoleOutputEnabled
        self.CurrentLogLength = 1
        self.LogPath = LogPath
        self.LogFileNumber = 0
        self.SecondsToKeepLogs = SecondsToKeepLogs
        self.DatabaseWorking = False
        self.NodeID = socket.gethostname()

        self.StartTime = str(datetime.datetime.now()).replace(' ', '_')

        print(self.LogBuffer[:-1])


        # Open Log File #
        self.LogFileName = 'BG.log'
        self.LogFileObject = open(LogPath + self.LogFileName, 'w')


        # Perform Database Connection Validation #
        if DatabaseConfig == None:
            print('Database Configuration Null, Please Check Config File')


        # Extract Values From Dictionary #
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

        self.DatabaseWorking = True


    def Log(self, Message:str, Level:int=0): # Handles The Log Of An Item #

        '''
        This function is the main logging function for the entire system.
        It accepts both a message and a log level as arguments.
        The log level has a default value built in, so if you are logging something at the system level, the Level argument does not need to be specified.
        If you are logging anything at a different level, you should specifiy it at the different level.
        The levels are as follows:

        +==================================+
        | Level             | Number Value |
        | Info              | 0            |
        | Warning           | 1            |
        | Error (Non fatal) | 2            |
        | Error (Fatal)     | 3            |
        +==================================+

        The error codes will be used to sort the log later, if the log is checked.
        If your plugin needs custom error codes, you can specify any number as the error code, however it won't be classified as anything.
        Please note that if you use custom codes, another plugin may conflict with these, so you may want to make it user configurable.
        '''

        # Reformat Log For Human Readabillity #
        Level = str(Level)

        CallStack = inspect.stack()
        CallingModuleName = CallStack[1][1]
        CallingFunctionName = CallStack[1][3]

        LogTime = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
        LogString = f'[{Level.rjust(5, " ")}] [{LogTime}] [{CallingModuleName.split("/")[-1].split(".")[0].rjust(20, " ")}] [{CallingFunctionName.rjust(19, " ")}] {Message}\n'
        self.LogBuffer += LogString

        if self.PrintEnabled:
            print(LogString[:-1])


        if self.DatabaseWorking == False:

            self.LogFileObject.write(self.LogBuffer)
            self.LogBuffer = ''

        else:

            self.LogFileObject.write(self.LogBuffer)
            #
            # Write data *from the logbuffer* into the database here
            #
            
            insertStatement= ("INSERT INTO log(LogLevel,LogDatetime,CallingModule,FunctionName,LogOutput,Node) VALUES (%s, %s, \"%s\", \"%s\", \"%s\", \"%s\")")

            val = (Level, str(LogTime), CallingModuleName.split("/")[-1].split(".")[0], CallingFunctionName, Message, str(self.NodeID))

            self.LoggerCursor.execute(insertStatement,val)

            
            self.LogBuffer = ''
            

    def PullLog(self, NumberOfLines:int): # Pull n most recent entries from the log table #
        
        # Pull Lines From Database #
        PullStatement= ("SELECT * FROM log LIMIT %d" % int(NumberOfLines))
        self.LoggerCursor.execute(PullStatement)
        
        Rows = self.LoggerCursor.fetchall()

        # Return Them #
        return Rows


    def PullSort(self, NumberOfLines:int): # Pull Set Number Of Lines And Return A Sorted Output Dictionary #

        # Pull Lines Here #
        Rows = self.PullLog(NumberOfLines)
        NodesInList = []

        # Sort Lines #
        for LineItem in Rows:
            if LineItem[6] not in NodesInList:
                NodesInList.append(LineItem[6])

        OutDict = {}
        for NodeHostName in NodesInList:
            OutDict.update({NodeHostName : []})

        for LineItem in Rows:
            OutDict[LineItem[6]].append(LineItem)

        # Return Lines #
        return OutDict        


    def CheckDelete(self, DeleteDate:str): # Deletes entries from the Log Table prior to a specific date # 
        
        # Delete Old Logs #
        DeleteStatement= ("DELETE FROM log WHERE LogDatetime < %s" % DeleteDate)
        self.LoggerCursor.execute(DeleteStatement)


    def PurgeOldLogs(self): # Automatically Removes Logs As Per The LogFile Retention Policy #

        # Calculate Old Date (Current Date Minus KeepSeconds) # 
        DeleteDateRaw = datetime.datetime.now() - datetime.timedelta(seconds=self.SecondsToKeepLogs)
        DeleteDate = DeleteDateRaw.strftime('%Y-%m-%d %H:%M:%S')
        
        # Execute Deletion Command # 
        self.CheckDelete(DeleteDate)


    def CleanExit(self): # Create Logger Shutdown Command #

        # Check LogBuffer, Flush #
        if self.LogBuffer != '':
            self.LogFileObject.write(self.LogBuffer)


        # Destroy Connection To Database #
        print('Destroying Database Connector')
        self.DatabaseConnection.close()
        print('Destroyed Database Connector')

        # Return Done #
        return
