###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

import datetime
import inspect
import os
import gzip

'''
Name: SysLog
Description: This class handles the main log for this braingenix instance.
Date-Created: 2020-12-19
'''

class SysLog():

    '''
    This class contains the logger used by the entire BrainGenix system.
    The logger is an essential part of the BrainGenix system, as it allows an entire cluster's log events to be aggregated into a usable format.
    The logger contains some functions which are described below.
    The most frequently used one will be the Log function, which accepts a message and a level, and puts it into the log.
    This is the only function that should be called by an external function, as calling other functions will cause undefined behavior in the logger.
    '''

    def __init__(self, NodeID:int, LogPath:str, BufferLength:int=10, LogSegmentLength:int=250, ConsoleOutputEnabled:bool=True, EnableGzip:bool=True): # Initializes The Log #
        
        '''
        This function is used when the system is starting up, and should not be called anytime after that.
        If this function is called later, it'll cause the logger to be reinitialized, which may loose the buffer.
        *DO NOT CALL THIS*
        '''

        NodeID = str(NodeID)

        # Check If Logs Dir Exists #
        print('Checking If Log Path Exists')

        DoesLogDirExist = os.path.isdir(LogPath)
        if not DoesLogDirExist:

            print('Log Path Does Not Exist, Creating Directory')
            os.mkdir(LogPath)
            print('Created Directory')

        # Initialize Local Variables #
        self.TryMakeDir(LogPath + 'Current')
        self.TryMakeDir(LogPath + f'Current/Node-{NodeID}')

        self.LogFileName = f'Current/Node-{NodeID}/Current' + '.log.txt'
        self.LogFileObject = open(LogPath + self.LogFileName, 'w')

        self.LogBuffer = '[Level] [               Time] [     Module Name] [           Function] [Message]\n'
        self.BufferLength = BufferLength
        self.PrintEnabled = ConsoleOutputEnabled
        self.LogSegmentLength = LogSegmentLength
        self.CurrentLogLength = 1
        self.LogPath = LogPath
        self.LogFileNumber = 0
        self.EnableGzip = EnableGzip

        self.NodeID = NodeID
        self.StartTime = str(datetime.datetime.now()).replace(' ', '_')

        print(self.LogBuffer[:-1])


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

        # Update Log Buffer #
        Level = str(Level)

        CallStack = inspect.stack()
        CallingModuleName = CallStack[1][1]
        CallingFunctionName = CallStack[1][3]

        LogTime = datetime.datetime.now().strftime('%Y-%m-%d_%H-%M-%S')

        LogString = f'[{Level.rjust(5, " ")}] [{LogTime}] [{CallingModuleName.split("/")[-1].split(".")[0].rjust(16, " ")}] [{CallingFunctionName.rjust(19, " ")}] {Message}\n'

        self.LogBuffer += LogString

        if self.PrintEnabled:

            print(LogString[:-1])

        if self.LogBuffer.count('\n') >= self.BufferLength:

            self.LogFileObject.write(self.LogBuffer)
            self.LogBuffer = ''

        # Truncate Log And Start New Log File If Log Is Over #
        self.CurrentLogLength += 1

        if self.CurrentLogLength > self.LogSegmentLength:

            self.PurgeBuffer()
            LogFileName = self.StartNewFile()

            if self.EnableGzip:
                self.CompressFile(LogFileName)


    def CompressFile(self, FileName):

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


    def StartNewFile(self):

        '''
        This function is called after every file is split, and creates a new file.
        *DO NOT CALL THIS*
        '''

        # Close Current File, and Move To New FileName #
        self.LogFileObject.close()

        NodeID = self.NodeID

        DateTimeObject = datetime.datetime.now()
        DateString = DateTimeObject.strftime('%Y-%m-%d_%H-%M-%S')

        NewLogName = f'{self.LogPath}Current/Node-{NodeID}/LogSegment{self.LogFileNumber}_{DateString}.log.txt'
        self.LogFileNumber += 1


        os.rename(f'{self.LogPath}Current/Node-{NodeID}/Current.log.txt', NewLogName)


        # Start New LogFile #
        
        self.LogFileName = f'Current/Node-{NodeID}/Current' + '.log.txt'
        self.LogFileObject = open(self.LogPath + self.LogFileName, 'w')

        self.LogBuffer = '[Level] [               Time] [     Module Name] [           Function] [Message]\n'
        self.CurrentLogLength = 1


        # Return The Old File Name #

        return NewLogName


    def TryMakeDir(self, path): # Makes a Dir, catches exception if already exists #

        '''
        This function attempts to make a directory, and if it already exists, skips it.
        *DO NOT CALL THIS*
        '''

        if not os.path.exists(path):
            os.mkdir(path)


    def PurgeBuffer(self): # Writes The Contents Of The Buffer To Disk #

        '''
        This function is called at BG exit, and purges the buffer of the log to the disk.
        *DO NOT CALL THIS*
        '''

        self.LogFileObject.write(self.LogBuffer)
        self.LogBuffer = ''


    def CleanExit(self): # Cleanly Closes The File Object #

        '''
        This function is used whenever BG exists, to purge the buffer and close the file stream.
        If BG crashes, in most cases this will flush the log, however if it crashes in a way which results in immediate execution halting, this may not work.
        *DO NOT CALL THIS*
        '''

        DateTimeObject = datetime.datetime.now()
        DateString = DateTimeObject.strftime('%Y-%m-%d_%H-%M-%S')

        self.LogFileObject.write(f'----Buffer Successfully Flushed On [{DateString}]----')
        self.LogFileObject.close()


        #try:
        #    os.rename(f'{self.LogPath}/Current', f'{self.LogPath}/{self.StartTime}')
        #except: # Catch Exception If Other Node(s) Have Open Files #
        #    pass