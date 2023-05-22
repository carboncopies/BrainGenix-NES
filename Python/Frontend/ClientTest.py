# Test Driver

import Frontend as BrainGenix



def main():

    print("Starting NES Frontend Tests")
    ClientInstance = BrainGenix.NES.Client(Endpoint="127.0.0.1", Port="8765", Token="blah")

    if not ClientInstance.IsConnected():
        print("Cannot Connect To Client")