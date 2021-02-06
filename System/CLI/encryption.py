import hashlib

from Crypto import Random
from Crypto.Cipher import AES
from base64 import b64encode

class Cipher(object):

    def __init__(self,key):
        self.block_size= AES.block_size
        self.key= hashlib.sha256(key.encode()).digest()

    def __pad(self,traffic):
        nbytes= self.block_size- len(traffic)
        ascii_string= chr(nbytes)
        extra= nbytes*ascii_string
        padded_input= traffic + extra
        return padded_input

    def encrypt(self,traffic):
        input_traffic= self._pad(traffic)
        iv= Random.new().read(self.block_size)
        cipher= AES.new(self.key, AES.MODE_CBC, iv)
        enc= cipher.encrypt(input_traffic.encode())
        return b64encode(iv + enc).decode("utf-8")
        
        
    



