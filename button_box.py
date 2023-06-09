import serial
import pydirectinput

with serial.Serial() as ser:
    ser.baudrate = 9600
    ser.port = 'COM3'
    ser.open()
    button = '0'

    while(button != 'q'):
        button = ser.read(1);
        button = button.decode('utf-8')
        pydirectinput.press(button)
        lastPressed = button
        #print(button)

    ser.close()