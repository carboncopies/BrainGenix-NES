###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

import gzip
import datetime
import inspect
import os
import threading
import socket
import queue

from Core.Management.Logger.DBLoggerThread import DatabaseLogTransmissionSystem

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
        SecondsToKeepLogs = int(ConfigFileDictionary.get('SecondsToKeepLogs'))
        ConsoleOutputEnabled = bool(ConfigFileDictionary.get('ConsoleOutputEnabled'))
        self.ConsoleColorEnabled = bool(ConfigFileDictionary.get('ConsoleColorEnabled'))

        # Logger Config Params #
        self.LevelColors = ConfigFileDictionary['LogLevelColors']
        self.LevelNames = ConfigFileDictionary['LogLevelNames']


        # Create Local Log Path Directory #
        DoesLogDirExist = os.path.isdir(LogPath)
        if not DoesLogDirExist:

            print('Log Path Does Not Exist, Creating Directory')
            os.mkdir(LogPath)
            print('Created Directory')


        # Initialize Local Variable Information #
        print('[Level] [               Time] [                Thread] [            Module Name] [           Function] [Message]\n')
        self.PrintEnabled = ConsoleOutputEnabled
        self.CurrentLogLength = 1
        self.LogPath = LogPath
        self.LogFileNumber = 0
        self.SecondsToKeepLogs = SecondsToKeepLogs
        self.DatabaseWorking = False
        self.NodeID = socket.gethostname()

        self.StartTime = str(datetime.datetime.now()).replace(' ', '_')


        # Open Log File #
        self.LogFileName = 'BG.log'
        self.LogFileObject = open(LogPath + self.LogFileName, 'w')


        # Perform Database Connection Validation #
        if DatabaseConfig == None:
            print('Database Configuration Null, Please Check Config File')

        self.LogQueue = queue.Queue()
        self.ControlQueue = queue.Queue() # Causes thread exit when item placed in queue
        self.DBDumper = DatabaseLogTransmissionSystem(self, self.LogQueue, self.ControlQueue, DatabaseConfig)
        self.DumperThread = threading.Thread(target=self.DBDumper)
        self.DumperThread.start()




    # TODO: Either rename, or better rework so that it really only colorizes, without printing
    def ColorizeText(self, Text, Color): # Colorizes And Prints A String Of Text #

        # Get RGB #
        Red = Color[0]
        Green = Color[1]
        Blue = Color[2]


        # Add Colorization Char #
        PrintString = f'\x1b[38;2;{Red};{Green};{Blue}m'

        # Add Text #
        PrintString += Text

        # Add EndColor Char #
        PrintString += '\x1b[0m'

        # Output #
        return PrintString


    # Put Log Item In Queue #
    def EnqueueLogEntry(self, Level, LogTime, CallingModuleName, CallingFunctionName, Message, NodeID):
        self.LogQueue.put({
            'LogLevel': Level,
            'LogDateTime': LogTime,
            'CallingModule': CallingModuleName,
            'FunctionName': CallingFunctionName,
            'LogOutput': Message,
            'Node': NodeID
        })


    def Log(self, Message:str, Level:int=0): # Handles The Log Of An Item #

        '''
        This function is the main logging function for the entire system.
        It accepts both a message and a log level as arguments.
        The log level has a default value built in, so if you are logging something at the system level, the Level argument does not need to be specified.
        If you are logging anything at a different level, you should specifiy it at the different level.
        The levels are as follows:

        +==================================+
        | Level             | Number Value |
        | System            | 0            |
        | Config            | 1            |
        | Audit             | 2            |
        | Info              | 3            |
        | Detail            | 4            |
        | Fine              | 5            |
        | Finer             | 6            |
        | Finest            | 7            |
        | Warning           | 8            |
        | Severe (Non Fatal)| 9            |
        | Fatal             | 10           |
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
        ThreadName = threading.current_thread().name

        LogTime = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
        LogString = f'[{Level.rjust(5, " ")}] [{LogTime}] [{ThreadName.rjust(24, " ")}] [{CallingModuleName.split("/")[-1].split(".")[0].rjust(23, " ")}] [{CallingFunctionName.rjust(19, " ")}] {Message}'


        if self.PrintEnabled:

            # Colorize Text #
            if self.ConsoleColorEnabled == True:

                # Log With Colors #
                Msg = self.ColorizeText(LogString, self.LevelColors[int(Level)])
                print(Msg)

            # Print Text Normally #
            else:
                print(LogString)


        self.EnqueueLogEntry(Level, str(LogTime), CallingModuleName, CallingFunctionName, Message, str(self.NodeID))


    # TODO: Remove or rework (Logger instance no longer has a database cursor)
    # Most probably, just move to the CLAS
    def PullLog(self, NumberOfLines:int): # Pull n most recent entries from the log table #

        # Pull Lines From Database #
        PullStatement= ("SELECT * FROM log LIMIT %d" % int(NumberOfLines))
        self.LoggerCursor.execute(PullStatement)

        Rows = self.LoggerCursor.fetchall()

        # Return Them #
        return Rows


    # TODO: Remove or rework (Logger instance no longer has a database cursor)
    # Most probably, just move to the CLAS
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


    # TODO: Remove or rework (Logger instance no longer has a database cursor)
    # Most probably, just move to the CLAS
    def CheckDelete(self, DeleteDate:str): # Deletes entries from the Log Table prior to a specific date #

        # Delete Old Logs #
        DeleteStatement= ("DELETE FROM log WHERE LogDatetime < %s" % DeleteDate)
        self.LoggerCursor.execute(DeleteStatement)


    # TODO: Remove or rework (Logger instance no longer has a database cursor)
    # Most probably, just move to the CLAS
    def PurgeOldLogs(self): # Automatically Removes Logs As Per The LogFile Retention Policy #

        # Calculate Old Date (Current Date Minus KeepSeconds) #
        DeleteDateRaw = datetime.datetime.now() - datetime.timedelta(seconds=self.SecondsToKeepLogs)
        DeleteDate = DeleteDateRaw.strftime('%Y-%m-%d %H:%M:%S')

        # Execute Deletion Command #
        self.CheckDelete(DeleteDate)


    def CleanExit(self): # Create Logger Shutdown Command #

        # Destroy Connection To Database #
        print('Destroying DB Logger Thread')
        self.ControlQueue.put('stahp!')
        self.DatabasingThread.join()
        print('Destroyed DB Logger Thread')

        # Return Done #
        return

