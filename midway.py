from opensky_api import OpenSkyApi
import serial

localCallsigns = ["Hello"]

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

# print(localCallsigns)

for stuff in localCallsigns:
    print(stuff)


# Open serial port
ser = serial.Serial()
ser.baudrate = 9600
ser.port = 'COM3'
ser.parity = serial.PARITY_NONE
ser.stopbits = serial.STOPBITS_ONE
ser.bytesize = serial.EIGHTBITS

ser.timeout = 1

# Check which port was used
# print(ser.name)

# Makes sure to open the port
if(ser.isOpen() == False):
    ser.open()


def splitter(word):
    return [char for char in word]


whole = "academic"
inputString = splitter(whole)


tracker = ""
count = 0

incomplete = True
exit = False

while incomplete:
    # print(str(count) + ": " + "Sending " + inputString[count] + " to MC")
    hero = inputString[count]
    hero = bytes(hero, encoding='ascii')

    ser.write(hero)

    for x in range(200):
        mcReader = ser.read(count + 1).decode('ascii')
        # If it contains a digit or number
        if mcReader.isalnum:

            if mcReader in whole:
                count += 1
                tracker += mcReader
                if count == 7:
                    incomplete = False
                    exit = True

                break

    if exit:
        incomplete = False
        break

hero = inputString[len(inputString) - 1]
hero = bytes(hero, encoding='ascii')
for shutup in range(4):
    ser.write(hero)


# print("Obtained input:")
# print("Tracker: " + tracker)
# print(ser.readline().decode('ascii'))
# print("a:" + ser.read(32).decode('ascii'))

ser.close()
