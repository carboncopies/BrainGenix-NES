from Crypto.Cipher import AES
import random

AESkey = ''.join(chr(random.randint(0, 0xFF)) for i in range(16))
iv = ''.join([chr(random.randint(0, 0xFF)) for i in range(16)])
aes = AES.new(AESkey, AES.MODE_CBC, iv)

def EncryptCommand(OriginalCommand: dict):
  
   EncryptedCommand={}
  
   for key, value in OriginalCommand.items():
    
      key= aes.encrypt(key)
      EncryptedCommand[key] = aes.encrypt(value)
   
   return EncryptedCommand

def DecryptCommand(EncryptedCommand: dict):
  
   DecryptedCommand={}
  
   for key, value in EncryptedCommand.items():
    
      key= aes.decrypt(key)
      DecryptedCommand[key] = aes.decrypt(value)
   
   return DecryptedCommand
