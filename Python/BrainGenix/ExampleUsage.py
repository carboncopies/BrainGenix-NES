# Test Driver


import NES.Client

def main():

    print("Starting NES Frontend Tests")

    # Setup Client
    ClientConfiguration = NES.Client.Configuration()
    ClientConfiguration.Mode = NES.Client.Modes.Local

    ClientInstance = NES.Client(ClientConfiguration)

    if not ClientInstance.IsConnected():
        print("Cannot Connect To Client")


if __name__ == "__main__":
    main()