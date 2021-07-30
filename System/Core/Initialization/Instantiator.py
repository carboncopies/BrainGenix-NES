###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

import time

'''
Name: Subsystem Instantiator
Description: This file is used to create instances of other subsystems.
Date-Created: 2021-03-24
'''


from Core.Management.Logger.Logger import SysLog



def InstantiateLogger(SystemConfiguration): # Instantiates Kafka #

    # Log Message #
    print('Initializing Centralized Logging System')


    # Instantiate Kafka #
    try:

        Logger = SysLog(SystemConfiguration)

        # Log Success #
        Logger.Log('Centralized Logging Daemon Started')

        # Return Instantiated Kafka Interface Object #
        return Logger

    # If Something Fails During Instantiation #
    except Exception as E:

        T = time.time

        # Print Exception Message #
        ErrorMessage = ''
        ErrorMessage += f'Error During Logger Initialization at time: {T}\n'
        ErrorMessage += f'Fatal Exception: {E}'

        # Save Error Output To Disk#
        with open('README-BRAINGENIX-CLS-ERROR.txt', 'w') as FileObject:
            FileObject.write(ErrorMessage)

        # Print Error Message #
        print(ErrorMessage)

        # Exit #
        exit()
