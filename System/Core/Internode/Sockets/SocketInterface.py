###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

'''
Name: Socket Interface
Description: This provides a level of abstraction between SocketIO operations and the Queue Backend system.
Date-Created: 2021-04-30
'''

# import multiprocessing
# #import atexit
# #import socket
# import time


# class ProducerSocket(): # Class For Queue That Pushes Data To Another Server #

#     def __init__(self, Address:str, Port:str, InputQueue:object): # Initialization #

#         # Create Local Pointers #
#         self.Address = Address
#         self.Port = Port

#         # Create Socket "Server" (Consumer Connects To Producer) #
#         size = 0
#         t = time.time()
#         for z in range(100):
#             print(z)
#             size+=len(InputQueue.get(timeout=1))

#         print(size/(time.time()-t))

# D = b'asdfsadf'
# for z in range(25):
#     print(f'{z}/25')
#     D += D
# Q = multiprocessing.Queue()



# A = multiprocessing.Process(target=ProducerSocket, args=('', '', Q, ))
# A.start()


# print(len(D))

# for z in range(100):
#     print(f'{z}/100')
#     Q.put(D)





