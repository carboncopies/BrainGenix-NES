###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################



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

        # Finalize Init #
        self.Logger.Log('Finished CLAS Initialization')



    def ReadLog(self, Lines=500): # Read Lines From The Log, Default=500Lines #

        # Get Log Text #
        LogEntries = self.Logger.PullLog(Lines)

        # Return Log Text #
        return LogEntries


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
            output.update({NodeName : []})

        # Sort Into List #
        for LogItem in LogEntries:
            LineNodeName = LogItem[6]
            Output[LineNodeName].append(LogItem)

        # Return Output #
        return Output