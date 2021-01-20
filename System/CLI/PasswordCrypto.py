###########################################################
## This file is part of the BrainGenix Simulation System ##
###########################################################

import secrets
import hashlib

'''
Name: Password Crypto
Description: This file creates some basic cryptographic functions for creating the password database.
Date-Created: 2021-01-17
'''

def GenerateSalt(): # Generates A Cryptographically Complex Salt For Generating The Password Hash #

    Salt = secrets.token_urlsafe(65535)
    return Salt


def Hash(InputString, HashingDifficulty=524288): # Hashes Input A Number Of Times To Make Attacks More Difficult #

    Hash = InputString.encode()

    for _ in range(HashingDifficulty):
        Hash = hashlib.sha512(Hash)
        Hash = Hash.hexdigest().encode()


    return Hash.decode()


def CombineSaltAndPassword(Salt:str, Password:str): # Combines The Two Into Something Hashable #

    return f'{Password}{Salt}'


def CheckPassword(HashInput:str, Salt:str, Password:str): # Checks A User's Password #

    SaltedPassword = CombineSaltAndPassword(Salt, Password)
    SaltedHash = Hash(SaltedPassword)

    if SaltedHash == HashInput:
        return True
    else:
        return False


def GeneratePassword(Password:str): # Generates A New Password #

    Salt = GenerateSalt()
    SaltedPassword = CombineSaltAndPassword(Salt, Password)
    PasswordHash = Hash(SaltedPassword)

    return Salt, PasswordHash