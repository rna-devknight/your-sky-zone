# import time
# import datetime from time
# from datetime import time
# import serial from datetime
# from datetime import serial
import serial
# import gmtime
# import strftime

# Open serial port
ser = serial.Serial()
ser.baudrate = 9600
ser.port = 'COM3'
ser.parity = serial.PARITY_NONE
ser.stopbits = serial.STOPBITS_ONE
ser.bytesize = serial.EIGHTBITS

ser.timeout = 1

# Check which port was used
print(ser.name)

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

# print(len(whole))
# print(len(inputString))

# print(whole.upper())


while incomplete:
    # print(str(count) + ": " + "Sending " + inputString[count] + " to MC")
    hero = inputString[count]
    hero = bytes(hero, encoding='ascii')

    ser.write(hero)

    for x in range(200):
        mcReader = ser.read(count + 1).decode('ascii')
        # If it contains a digit or number
        if mcReader.isalnum:
            # print("Searching for: " + mcReader)

            if mcReader in whole:
                # print("Within: " + mcReader)
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
