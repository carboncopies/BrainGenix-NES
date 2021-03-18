import time
import requests

def avg(listin):
    return sum(listin)/len(listin)

timelist = []
for z in range(100):
    t = time.time()
    requests.get('http://10.0.0.82:8000/test')
    timelist.append(time.time()-t)

print(avg(timelist))
