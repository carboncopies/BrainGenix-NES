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

        self.Logger.Log('Initializing Centralized Logger Aggregation System')