## @package EncryptDecrypt
# Encrypts and Decrypts commands between the UI and NES

from Crypto.Cipher import AES
import random, string, base64

AESkey = ''.join(random.choice(string.ascii_uppercase + string.ascii_lowercase + string.digits) for x in range(32))
iv = ''.join(random.choice(string.ascii_uppercase + string.ascii_lowercase + string.digits) for x in range(16))

## Encrypts commands issued by the user through UI to NES
# @param OriginalCommand Command issued in UI
def EncryptCommand(OriginalCommand: str):

   aes = AES.new(AESkey, AES.MODE_CFB, iv)
   EncryptedCommand = aes.encrypt(OriginalCommand)
   EncryptedCommand = base64.b64encode(EncryptedCommand)

   return EncryptedCommand

## Decrypts commands in NES for actual execution
# @param EncryptedCommand Encrypted Command present in NES
def DecryptCommand(EncryptedCommand: str):

   EncryptedCommand = base64.b64decode(EncryptedCommand)
   aes = AES.new(AESkey, AES.MODE_CFB, iv)
   DecryptedCommand = aes.decrypt(EncryptedCommand)
   # DecryptedCommand = DecryptedCommand[0:len(DecryptedCommand)//16]

   return DecryptedCommand
