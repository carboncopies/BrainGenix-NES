###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

from Cryptography.KeyUtils import GenKeys
from Crypto.Util import number
from Crypto.Hash import SHA256

'''
Name: Encryption
Description: This file encrypts the BrainGenix Command Line Traffic using the Diffie Hellman Algorithm.
Date-Created: 2021-02-07
'''

class Encryption:
	
	def __init__(self):
		self.Prime= hex(number.getPrime(2048))

	def GenerateSecret(self, PrivateKey, PublicKey): # Generates A New Secret #
		Secret = pow(int(PublicKey), int(PrivateKey), self.Prime)
		SecretBytes = str(Secret)

	    	# Generate Hash Key Using SHA256 #
	    	Key = SHA256.new()
	    	Key.update(bytes(SecretBytes))
	   	 SecretKey = Key.hexdigest()
	    	return SecretKey

	def Encrypt(self, Logger:object, Command:str):

	    	# Load The Pub/Private Keys #
	    	PublicKey, PrivateKey = ReadKeys(Logger)
	    	SecretKey = GenerateSecret(PublicKey, PrivateKey)
	    	enc_Command= ""

	    	for c in Command:
			enc_Command += chr(ord(c)+ SecretKey)

	    	return enc_Command


    

    
        
        
    



