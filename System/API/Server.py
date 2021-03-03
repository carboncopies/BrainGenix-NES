#https://realpython.com/python-https/
#Client -> Proxy(This File, Hosts API) -> Zookeeper -> Leader -> Follower(s)
# IS THERE A BETTER WAY TO DO THIS THAT DOESN'T NEED ALL THESE STEPS? I'M NOT SURE OF ONE #

# server.py
from flask import Flask

SECRET_MESSAGE = "fluffy tail"
app = Flask(__name__)

@app.route("/")
def get_secret_message():
    return SECRET_MESSAGE
