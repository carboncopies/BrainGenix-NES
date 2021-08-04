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
from Core.Management.Logger.CLAS import CentralizedLoggerAggregationSystem

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

    def __init__(self, SystemConfiguration:dict): # Connect To Database #

        '''
        This function is used when the system is starting up, and should not be called anytime after that.
        If this function is called later, it'll cause the logger to be reinitialized, which may loose the buffer.
        *DO NOT CALL THIS*
        '''


        # Perform Database Connection Validation #
        if SystemConfiguration == None:
            print('System Configuration Null, Please Check Config File')
            exit()

        # Extract the important values from the dictionary and return them to the main system #
        SecondsToKeepLogs = int(SystemConfiguration.get('SecondsToKeepLogs'))
        ConsoleOutputEnabled = bool(SystemConfiguration.get('ConsoleOutputEnabled'))
        self.ConsoleColorEnabled = bool(SystemConfiguration.get('ConsoleColorEnabled'))

        self.SystemConfiguration = SystemConfiguration

        # Logger Config Params #
        self.LevelColors = SystemConfiguration['LogLevelColors']
        self.LevelNames = SystemConfiguration['LogLevelNames']


        # Initialize Local Variable Information #
        
        if bool(self.SystemConfiguration['DisplayLogLevelsAsText']) == False:
            print('[Level] [               Time] [                  Thread] [            Module Name] [           Function] [Message]')
        else:
            print('[  Level] [               Time] [                  Thread] [            Module Name] [           Function] [Message]')


        self.PrintEnabled = ConsoleOutputEnabled
        self.CurrentLogLength = 1
        self.LogFileNumber = 0
        self.SecondsToKeepLogs = SecondsToKeepLogs
        self.DatabaseWorking = False
        self.NodeID = socket.gethostname()

        self.StartTime = str(datetime.datetime.now()).replace(' ', '_')


        # Create Queues #
        self.LogQueue = queue.Queue()
        self.ControlQueue = queue.Queue() # Causes thread exit when item placed in queue
        self.QueueDBWorker = DatabaseLogTransmissionSystem(self, self.LogQueue, self.ControlQueue, SystemConfiguration)
        self.QueueDBWorkerThread = threading.Thread(target=self.QueueDBWorker)
        self.QueueDBWorkerThread.start()

        # Initialize CLAS #
        self.CLAS = CentralizedLoggerAggregationSystem(self, self.SystemConfiguration)

    def ColorizeText(self, Text, Color): # Colorizes A String Of Text #

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
    def EnqueueLogEntry(self, Level, LogTime, CallingModuleName, CallingFunctionName, Message, NodeID, Thread):
        self.LogQueue.put({
            'LogLevel': Level,
            'LogDateTime': LogTime,
            'CallingModule': CallingModuleName,
            'FunctionName': CallingFunctionName,
            'LogOutput': Message,
            'Node': NodeID,
            'Thread': Thread
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

        # Get time first thing when called to make it more precise (not sure if that's needed, but it seems to have no drawbacks) #
        LogTime = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')

        # Set Log Level #
        if bool(self.SystemConfiguration['DisplayLogLevelsAsText']) == False:
            LevelString = str(Level)
        else:
            LevelString = dict(self.SystemConfiguration['LogLevelNames'])[int(Level)]

        # Reformat Log For Human Readabillity #
        CallStack = inspect.stack()
        CallingModuleName = CallStack[1][1]
        CallingFunctionName = CallStack[1][3]
        ThreadName = threading.current_thread().name

        if bool(self.SystemConfiguration['DisplayLogLevelsAsText']) == False:
            LogString = f'[{LevelString.rjust(5, " ")}] [{LogTime}] [{ThreadName.rjust(24, " ")}] [{CallingModuleName.split("/")[-1].split(".")[0].rjust(23, " ")}] [{CallingFunctionName.rjust(19, " ")}] {Message}'
        else:
            LogString = f'[{LevelString.rjust(7, " ")}] [{LogTime}] [{ThreadName.rjust(24, " ")}] [{CallingModuleName.split("/")[-1].split(".")[0].rjust(23, " ")}] [{CallingFunctionName.rjust(19, " ")}] {Message}'


        if self.PrintEnabled:

            # Colorize Text #
            if self.ConsoleColorEnabled:

                # Log With Colors #
                Msg = self.ColorizeText(LogString, self.LevelColors[int(Level)])
                print(Msg)

            # Print Text Normally #
            else:
                print(LogString)


        self.EnqueueLogEntry(Level, str(LogTime), CallingModuleName, CallingFunctionName, Message, str(self.NodeID), ThreadName)


    def CleanExit(self): # Create Logger Shutdown Command #

        # Destroy Connection To Database #
        self.ControlQueue.put('stahp!')
        self.QueueDBWorkerThread.join()

        # Return Done #
        return
