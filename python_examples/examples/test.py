from sinric import Sinric
from time import sleep

apiKey = 'Your api Key'  # https://sinric.com
response = any

obj = Sinric(apiKey)
while True:
    response = obj.initialize()
    print(response) #Prints response
    sleep(2)
 