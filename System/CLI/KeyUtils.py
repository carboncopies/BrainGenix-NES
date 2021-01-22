###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

import os

from cryptography.hazmat.primitives import serialization as crypto_serialization
from cryptography.hazmat.primitives.asymmetric import rsa
from cryptography.hazmat.backends import default_backend as crypto_default_backend

'''
Name: GenKeys
Description: This file generates ssh keys used by the BG SSH server.
Date-Created: 2021-01-17
'''

def GenKeys(Logger:object, KeyLength:int=8192): # Generates A Set Of RSA Keys For SSH Encryption #

    '''
    Generates a new keypair for encrypting CLI traffic.
    This should generate a keypair of kength 8192 bits by default, however the KeyLength argument can be specified to set another length.
    *Please don't use this function unless you know what you're doing!*
    '''

    Logger.Log(f'Generating New Pub/Private Key Pair Of Length {KeyLength} Characters, Using The RSA Method')

    KeyPair = rsa.generate_private_key(backend=crypto_default_backend(), public_exponent=65537, key_size=KeyLength)

    PrivateKey = KeyPair.private_bytes(crypto_serialization.Encoding.PEM, crypto_serialization.PrivateFormat.TraditionalOpenSSL, crypto_serialization.NoEncryption())
    PublicKey = KeyPair.public_key().public_bytes(crypto_serialization.Encoding.OpenSSH, crypto_serialization.PublicFormat.OpenSSH)

    Logger.Log('Generated Keypair')

    return PublicKey, PrivateKey


def WriteKeys(Logger:object, PublicKey:bytes, PrivateKey:bytes): # Writes The Keys To A File #

    '''
    This function writes the keyfiles to a set of files.
    This is not secure, as anyone can read these files.
    In the future, this function needs to be updated to prevent these key files from being read by anyone.
    *Please don't use this function unless you know what you're doing!*
    '''

    with open('Config/Key.pub', 'wb') as File:
        File.write(PublicKey)

    with open('Config/PrivateKey.key', 'wb') as File:
        File.write(PrivateKey)

    Logger.Log('Wrote New Key Files')

    #####################################################################################################
    # Fix this, if the code is read from network storage, the entire system will share the same keypair #
    #####################################################################################################


def ReadKeys(Logger:object): # Reads The SSL Keys From The Key Files #

    '''
    This function reads the insecure key files.
    This also needs to be updated in the future to prevent these keys from being readable in plaintext.
    *Please don't use this function unless you know what you're doing!*
    '''

    Logger.Log('Reading Key Files')

    with open('Config/Key.pub', 'rb') as File:
        PublicKey = File.read()

    with open('Config/PrivateKey.key', 'rb') as File:
        PrivateKey = File.read()

    return PublicKey, PrivateKey


def CheckIfKeysExist(Logger:object): # Checks If Keypair Has Already Been Generated #

    '''
    This function checks if the keys already exist.
    It uses os to check if the files exist, and assumes the keyfiles are there if the files exist.
    This does NOT check if the keyfiles are corrupted.
    If you suspect the files are corrupted, delete them and they'll be regenerated on next start.
    *Please don't use this function unless you know what you're doing!*
    '''

    Logger.Log('Checking If Keys Exist')

    if os.path.exists('Config/Key.pub') != True:
        Logger.Log('Failed To Find Public Key File')
        return False
    if os.path.exists('Config/PrivateKey.key') != True:
        Logger.Log('Failed To Find Private Key File')
        return False
    return True