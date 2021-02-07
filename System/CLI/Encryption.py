import KeyUtils
from Crypto.Util import number
from Crypto.Hash import SHA256

global prime

prime= number.getPrime(2048) 

def gen_secret(privatekey,publickey):
    secret = pow(int(publickey), int(privatekey), prime)
    
    secret_bytes = str(secret)
	
    #Generate hash key using SHA256
    key = SHA256.new()
    key.update(bytes(secret_bytes))
    secretKey = key.hexdigest()
    return secretKey

def encrypt(Logger:object, Command:str):
    PublicKey,PrivateKey= KeyUtils.GenKeys(Logger:object)
    SecretKey= gen_secret(PublicKey,PrivateKey)
    enc_Command= ""

    for c in Command:
        enc_Command += chr(ord(c)+ SecretKey)

    return enc_Command
    

    

    
        
        
    



