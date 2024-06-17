import serial
from helpers import split_data, check_ratio

#Threshold ratio
temp_ratio = 1.05
humi_ratio = 0.95
ldr_ratio = 1.15
CO_ratio = 0.85

#Data before 30s
temp30 = -1
humi30 = -1
ldr30 = -1
CO30 = -1

#Count number of parameter exceeds the threshold ratio value
temp_C = 0
humi_C = 0
ldr_C = 0
CO_C = 0

#Open Ser
ser = serial.Serial('COM5', 9600)

while True:
    data = ser.readline().decode('utf-8').strip() 
    humi, temp, CO, ldr = split_data(data)
    humi/=100
    temp/=100

    print(f"temperature: {temp},  relative humidity: {humi}, light intensity level: {ldr}, CO level: {CO}")

    if temp30 == -1 or humi30 == -1 or ldr30 == -1 or CO30 == -1:
        temp30 = temp
        humi30 = humi
        ldr30 = ldr
        CO30 = CO
        continue
    
    check_ratio(temp_C, temp, temp30, temp_ratio, "temperature")
    check_ratio(humi_C, humi, humi30, humi_ratio, "humidity")
    check_ratio(ldr_C, ldr, ldr30, ldr_ratio, "light intensity")
    check_ratio(CO_C, CO, CO30, CO_ratio, "CO")

    temp30 = temp
    humi30 = humi
    ldr30 = ldr
    CO30 = CO

    

