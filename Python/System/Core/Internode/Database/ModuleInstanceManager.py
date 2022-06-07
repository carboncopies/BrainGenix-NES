###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

import pymysql
import threading

'''
Name: Module Instance Manager
Description: This file contains a system that creates pymysql instances for all threads.
Date-Created: 2021-01-17
'''




class DatabaseInstanceCreator: # Pymysql Instance Creation System #


    def __init__(self, Logger:object, DatabaseConfig:dict, ControlQueue:object): # Initialization #

        # Create Local Pointers #
        self.Logger = Logger
        self.DatabaseConfig = DatabaseConfig
        self.ControlQueue = ControlQueue

        # Connect To DB #
        self.DBUsername = str(DatabaseConfig.get('DatabaseUsername'))
        self.DBPassword = str(DatabaseConfig.get('DatabasePassword'))
        self.DBHost = str(DatabaseConfig.get('DatabaseHost'))
        self.DBDatabaseName = str(DatabaseConfig.get('DatabaseName'))


    def ShouldStop(self): # Check If Thread Should Exit #
        return self.ControlQueue.empty()


    def __call__(self): # Database Instance Manager Thread #

        # start loop #
        MainThread = threading.Thread.getName (threading.main_thread())
        ThisThread = threading.Thread.getName (threading.current_thread())
        Threads = [MainThread, ThisThread] # setup thread list #

        # Log Init Complete #
        self.Logger.Log("Database Interface Instance Creator Initalized", 3)


        # Main Loop #
        while self.ShouldStop():


            # loop throgh threads #
            for ExistingThread in threading.enumerate():
                RunOnce = False
                # loop throgh thread list #
                for Thread in Threads:
                    # check if thread has pymysql #
                    if Thread == threading.Thread.getName (ExistingThread):
                        RunOnce = True

                if RunOnce != True:

                    # Create Pymysql Instance For Thread #
                    Threads.append(threading.Thread.getName (ExistingThread))
                    ExistingThread.PymysqlInstance = pymysql.connect(host = self.DBHost, user = self.DBUsername, password = self.DBPassword, db = self.DBDatabaseName)

                    # Log Instance Creation #
                    self.Logger.Log("Database Instance Created for thread " + threading.Thread.getName (ExistingThread), 3)


        # Log Thread Shutdown #
        self.Logger.Log(f'Shutting down {ThisThread}', 3)