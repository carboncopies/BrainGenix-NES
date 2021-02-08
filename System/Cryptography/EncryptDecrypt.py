###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

from Cryptography.KeyUtils import GenKeys
from Crypto.Util import number
from Crypto.Hash import SHA256

'''
Name: Encryption
Description: This file performs encryption and decryption on the BrainGenix Command Line Traffic using the Diffie Hellman Algorithm.
Date-Created: 2021-02-07
'''

class EncryptDecrypt:
	
	def __init__(self, PrimeLength:int=2048): # Initialize The Class #

        # Generate A Prime Nubmer Of Specified Length #
		self.Prime= hex(number.getPrime(PrimeLength)) 


	def GenerateSecret(self, PrivateKey, PublicKey): # Generates A New Secret #

		# Gen Secret #
		Secret = pow(int(PublicKey), int(PrivateKey), self.Prime)
		SecretBytes = str(Secret)

	    # Generate Hash Key Using SHA256 #
	    Key = SHA256.new()
	    Key.update(bytes(SecretBytes))
	   	SecretKey = Key.hexdigest()
	    
		# Return Secret Key #
		return SecretKey


	def Encrypt(self, Logger:object, Command:str):

		# Load The Pub/Private Keys #
		PublicKey, PrivateKey = ReadKeys(Logger)
		SecretKey = GenerateSecret(PublicKey, PrivateKey)

		## CAN WE REPLACE THIS WITH AES 256 ##
		EncryptedCommand = ""

		for c in Command:
			EncryptedCommand += chr(ord(c)+ SecretKey)


		# Return The Encrypted Command #
		return EncryptedCommand
	

	def Decrypt(self, Logger:object, encCommand:str):

	    PublicKey, PrivateKey = ReadKeys(Logger)
	    SecretKey = GenerateSecret(PublicKey, PrivateKey)

		## CAN WE REPLACE THIS WITH AES 256 ##
		DecryptedCommand = ""
			
		for c in encCommand:
			DecryptedCommand += chr(ord(c)-SecretKey)
		
		# Return The Decrypted Command #
		return DecryptedCommand


    

    
        
        
    



