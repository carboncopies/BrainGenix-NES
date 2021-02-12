###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

import pymysql
import atexit

'''
Name: Database Interface
Description: This file creates an interface from BrainGenix to mySQL.
Date-Created: 2021-01-17
'''

class DBInterface(): # Interface to MySQL database #

    '''
    This class is used to provide a layer of abstraction between the BrainGenix system and mysql. 
    It uses pymysql to interact with the database over sockets.
    This class is designed with some specific methods to check user information, and other such information.
    As the database changes structure, this file will also change, to add new database features.
    '''

    def __init__(self, Logger:object, Username:str, Password:str, Host:str, Database:str):

        '''
        Initializes the DB Interface by connecting to mysql.
        The connection information is passed in as arguments.
        These arguments are loaded from the local config YAML file.
        '''

        # Create Local Pointer #
        self.Logger = Logger

        # Establish DB Connection #
        self.DBConnection = pymysql.connect(host=Host, user=Username, password=Password, db=Database)

        self.Logger.Log(f'Connected To DB At {Host}')

        # Register Shutdown Function #
        atexit.register(self.ShutdownConnection)

    def CreateCursor(self): # Creates A Database Cursor #

        '''
        This method creates a cursor object as an attribute in self.
        '''

        # Create Cursor #
        self.DatabaseCursor = pymysql.cursors.Cursor()

    
    def CheckIfUserExists(self, UserName:str, table:str): # Checks If A Given User Exists #

        '''
        Returns a boolean indicating if the target username exists or does not exist.
        This method is part of the DBInterface class. 
        '''

        # Execute Some MYSQL Cursor Code #
        # something here...
        
       
        #Count of number of Users having the same UserName in the table
        cnt= self.DatabaseCursor.execute("SELECT * from "+table+" where UserName='"+UserName+"'")
        
        UserExists= True
        #If User Exists
        if cnt==1:
            UserExists=True
            
        else:
            UserExists=False

        return UserExists


    def GetUserInformation(self, UserName:str, table:str): # Returns A List Of User Information #

        '''
        Returns some user information from the specific row of the user.
        This is going to include everything outlined in the user DB block diagram.
        The specific format of the information is a dictionary with key values being the names of the columns.
        '''

        sql = "SELECT UserID, UserName, FirstName, LastName, AccountEnabled, AccountExpirationDate FROM "+table+" WHERE UserName ='"+UserName+"'"
        self.DatabaseCursor.execute(sql)
        
        #List of user details in the table
        UserDetails=[]
        for row in cur:
            l=[]
            l.append(row['UserID'])
            l.append(row['UserName'])
            l.append(row['FirstName'])
            l.append(row['LastName'])
            l.append(row['AccountEnabled'])
            l.append(row['AccountExpirationDate'])
            UserDetails.append(l)
            
        return UserDetails


    def ReadNeuronInformation(self, UserName:str): # ... #

        raise NotYetImplementedError


    def ShutdownConnection(self): # Closes The Database Connection #

        # Close DB Connection #
        self.DBConnection.close()
        self.Logger.Log('Shutdown Database Connection')
