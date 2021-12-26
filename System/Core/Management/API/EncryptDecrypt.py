from Crypto.Cipher import AES
import os

AESkey = os.urandom(16)
iv = os.urandom(16)
aes = AES.new(AESkey, AES.MODE_CBC, iv)


def EncryptCommand(OriginalCommand: str):
  
   EncryptedCommand= aes.encrypt(OriginalCommand)
   
   return EncryptedCommand
  

def DecryptCommand(EncryptedCommand: str):
  
   DecryptedCommand= aes.decrypt(EncryptedCommand)
   
   return DecryptedCommand
