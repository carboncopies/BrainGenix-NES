###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

import datetime
import time
import inspect
import os
import atexit
import gzip

'''
Name: SysLog
Description: This class handles the main log for this braingenix instance.
Date-Created: 2020-12-19
'''

class SysLog():

    def __init__(self, NodeID:int, LogPath:str, BufferLength:int=10, LogSegmentLength:int=250, ConsoleOutputEnabled:bool=True, EnableGzip:bool=True): # Initializes The Log #


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


        # Truncate Log And Start New Log File If Log Is Over 

        self.CurrentLogLength += 1

        if self.CurrentLogLength > self.LogSegmentLength:

            self.PurgeBuffer()
            LogFileName = self.StartNewFile()

            if self.EnableGzip:
                self.CompressFile(LogFileName)


    def CompressFile(self, FileName):


        # First, Read The File Into Ram, Then Remove The Original #

        with open(FileName, 'rb') as FileObject:

            FileText = FileObject.read()

        os.remove(FileName)


        # Write The Compressed File To The Disk #

        with gzip.open(FileName + '.gz', 'wb') as FileObject:

            FileObject.write(FileText)


    def StartNewFile(self):


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

        try:
            os.mkdir(path)
        except:
            pass


    def PurgeBuffer(self): # Writes The Contents Of The Buffer To Disk #

        self.LogFileObject.write(self.LogBuffer)
        self.LogBuffer = ''


    def CleanExit(self): # Cleanly Closes The File Object #


        DateTimeObject = datetime.datetime.now()
        DateString = DateTimeObject.strftime('%Y-%m-%d_%H-%M-%S')

        self.LogFileObject.write(f'----Buffer Successfully Flushed On [{DateString}]----')
        self.LogFileObject.close()


        try:
            os.rename(f'{self.LogPath}/Current', f'{self.LogPath}/{self.StartTime}')
        except: # Catch Exception If Other Node(s) Have Open Files #
            pass