# Test Driver

import Client


def main():

    print("Starting NES Frontend Tests")
    ClientInstance = Client(Endpoint="127.0.0.1", Port="8765", Token="blah")

    if not ClientInstance.IsConnected():
        print("Cannot Connect To Client")


if __name__ == "__main__":
    main()