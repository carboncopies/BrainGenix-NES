###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

import datetime
import time
import inspect
import os
import atexit

'''
Name: SysLog
Description: This class handles the main log for this braingenix instance.
Date-Created: 2020-12-19
Date-Modified: 2020-12-21
'''

class SysLog():

    def __init__(self, NodeID:int, LogPath:str, BufferLength:int=10, ConsoleOutputEnabled:bool=True): # Initializes The Log #


        NodeID = str(NodeID)

        DateTimeObject = datetime.datetime.now()
        DateString = DateTimeObject.strftime('%y.%m.%d-%H-%M-%S-Node')

        # Check If Logs Dir Exists #

        print('Checking If Log Path Exists')

        DoesLogDirExist = os.path.isdir(LogPath)

        if not DoesLogDirExist:

            print('Log Path Does Not Exist, Creating Directory')

            os.mkdir(LogPath)
                
            print('Created Directory')
        

        self.LogFileName = DateString + NodeID + '.log.txt'
        self.LogFileObject = open(LogPath + self.LogFileName, 'w')

        self.LogBuffer = '[Level] [     Module Name] [          Function] [Message]\n'
        self.BufferLength = BufferLength
        self.PrintEnabled = ConsoleOutputEnabled

        print(self.LogBuffer[:-1])


    def Log(self, Message:str, Level:int=0): # Handles The Log Of An Item #


        Level = str(Level)

        CallStack = inspect.stack()
        CallingModuleName = CallStack[1][1]
        CallingFunctionName = CallStack[1][3]

        LogString = f'[{Level.rjust(5, " ")}] [{CallingModuleName.split("/")[-1].split(".")[0].rjust(16, " ")}] [{CallingFunctionName.rjust(18, " ")}] {Message}\n'

        self.LogBuffer += LogString

        if self.PrintEnabled:

            print(LogString[:-1])

        if self.LogBuffer.count('\n') >= self.BufferLength:

            self.LogFileObject.write(self.LogBuffer)
            self.LogBuffer = ''


    def PurgeBuffer(self): # Writes The Contents Of The Buffer To Disk #

        self.LogFileObject.write(self.LogBuffer)
        self.LogBuffer = ''


    def CleanExit(self): # Cleanly Closes The File Object #


        DateTimeObject = datetime.datetime.now()
        DateString = DateTimeObject.strftime('%y/%m/%d %H-%M-%S')

        self.LogFileObject.write(f'----Buffer Successfully Flushed On [{DateString}]----')
        self.LogFileObject.close()