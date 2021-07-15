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

    def __init__(self, Logger): # Initialization Function #

        # Create Local Pointers To Logger Object #
        self.Logger = Logger

        # Print "Welcome" Message #
        self.Logger.Log('Initializing Centralized Logger Aggregation System')


        # Read MYSQL Information From System Configuration Dictionary #
        MySQLUname = self.SystemConfiguration['DatabaseUsername']
        MySQLPassword = self.SystemConfiguration['DatabasePassword']
        MySQLHost = self.SystemConfiguration['DatabaseHost']
        MySQLName = self.SystemConfiguration['DatabaseName']

        # Connect To DB Server #
        self.DatabaseConnection = pymysql.connect(
            host = DBHost,
            user = DBUsername,
            password = DBPassword,
            db = DBDatabaseName
        )


        # Finalize Init #
        self.Logger.Log('Finished CLAS Initialization')



    def ReadLog(self, Lines=500): # Read Lines From The Log, Default=500Lines #

        # Get Log Text #
        LogEntries = self.Logger.PullLog(Lines)

        # Return Log Text #
        return LogEntries


    # TODO: This is moved from the Logger and needs to be adapted/fixed/reworked
    def PullLog(self, NumberOfLines:int): # Pull n most recent entries from the log table #

        # Pull Lines From Database #
        PullStatement= ("SELECT * FROM log LIMIT %d" % int(NumberOfLines))
        self.LoggerCursor.execute(PullStatement)

        Rows = self.LoggerCursor.fetchall()

        # Return Them #
        return Rows


    def ProcessLog(self, Lines=500): # Read And Filter Lines From The Log #

        # Get Log Text #
        LogEntries = self.Logger.PullLog(Lines)

        # Identify Nodes #
        Nodes = []

        for Item in LogEntries:
            if Item[6] not in Nodes:
                Nodes.append(Item[6])

        # Generate Output Format #
        Output = {}

        for NodeName in Nodes:
            Output.update({NodeName : []})

        # Sort Into List #
        for LogItem in LogEntries:
            LineNodeName = LogItem[6]
            Output[LineNodeName].append(LogItem)

        # Return Output #
        return Output


    # TODO: Below is the code moved from the Logger; it needs to be adapted/fixed/reworked

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
        