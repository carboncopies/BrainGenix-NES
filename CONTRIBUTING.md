# Contributing to BrainGenix

Thanks for taking the time to contribute! We really do appreciate it.
Below is an outline on how to contribute:



# Function Formatting 

BrainGenix tries to format it's code as defined in the UppserCamelCase standard, with a few modifications for readabillity. Below is a list of guidelines that should be followed:

 * Specify object when defining arguments to a function. This simplifies the debugging of the system, as we can check the type of the argument passed to see if it's right. Example: `Level:int=0`
 * Add a comment on the definition of a function. Just write a bit about what your function does so that if it needs to be changed in the future then it's relatively simple to do.
 * Write a description of the function in more deatail below the definition line. Please use the tripple single-quote, so that it's all uniform. This should be as long as you need to make it to explain your function well.
 * Block out parts of your code. If your code has certain lines that can be grouped together, then please do so. These lines should also be preceded by a comment block formatted as # comment #. This block should briefly describe the purpose of that code "block".
 * Please also make sure to use well-maintained libraries and create a function to abstract them, so that they can be easily swapped out in the future if that is needed.


```
def Log(self, Message:str, Level:int=0): # Handles The Log Of An Item #

	'''
	This function is the main logging function for the entire system.
	etc...
	'''

	# Update Log Buffer #
	Level = str(Level)

	CallStack = inspect.stack()
	CallingModuleName = CallStack[1][1]
	CallingFunctionName = CallStack[1][3]

	etc...
```

# File Formatting

When creating a new file you should make sure to add the following headers to the top of your file:

 * A BrainGenix header which just says that this file is part of the BrainGenix simulation system.
 * A Descriptor below your imports which specifies the date that your file was created, and the description/name of the file.
 
 ```
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
```


