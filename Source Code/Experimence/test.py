import serial

#Threshold ratio
tem_ratio = 1.05
humi_ratio = 0.95
light_ratio = 1.15
CO_ratio = 0.85

#Data before 30s
tem30 = -1
humi30 = -1
light30 = -1
CO30 = -1

ser = serial.Serial('COM5', 9600)
while True:
    data = ser.readline().decode('utf-8').strip() # Đọc dữ liệu từ cổng Serial USB
    data_array = data.split();
    

