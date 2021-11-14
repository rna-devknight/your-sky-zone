#!/usr/bin/env python3

from opensky_api import OpenSkyApi
import serial
import time

port = input("Enter port: ")

# ser = serial.Serial('/dev/ttyACM2', 9600, 8, 'N', 1)
ser = serial.Serial(port, 9600, 8, 'N', 1)

localCallsigns = []

# BB = Bounding Box
# Small BB for day
# If taking bounding box from:https://boundingbox.klokantech.com/
# Order has to be changed
# 3 1 4 2
# -81.6417, 28.767, -81.2132, 29.0027
# 28.767, 29.0027, -81.6417, -81.2132

# Large BB for night
# -82.7711,28.4265,-81.0792,29.9228
# 28.4265, 29.9228, -82.7711, -81.0792
api = OpenSkyApi()
# BB for North Central Florida
states = api.get_states(bbox=(28.4265, 29.9228, -82.7711, -81.0792))

for s in states.states:
    temp = s.callsign
    # print(temp)
    localCallsigns.append(temp)


# only displaying the first ten callsigns on LCD
for sign in localCallsigns:
    if len(sign) > 8: # sending 8 characters max
        continue
    for i in sign:
        temp = bytes(i, 'utf-8')
        ser.write(temp)
        # print("Sent " + str(temp))
    ser.write(b'!') # choosen terminating character
    time.sleep(7)

ser.close()