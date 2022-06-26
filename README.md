# QCurrentMeasurement
GUI for the INA219 current measurement device 

PROTOCOL
---------
The current measuring device sends constantly the following data
byte_0             |byte_1     |byte_2      |byte_3       |byte_4      |byte_5       |
-------------------|-----------|------------|-------------|------------|-------------|
QCurrentMeasurement|bus_voltage|load_voltage|shunt_voltage|current_mA  |power_mW     |

The device name and device version is used to verify that the connected device is 
our device and not any other usb - serial  connected device, so we don't read gibberish


PC Application
---------------
This is a GUI application for the INA219 chip.  
The PC should connect via the serial port or the usb to serial port to some board 
(ESP32, Arduino Uno etc.) that has an I2C controller to which an INA219 is connected.

Hardware
--------
In my case its an ESP32 Huzzah Feather32. 
It can be any board with an I2C controller.
The firmware is the modified currentMeasurement from Adafruit's INA219 library.
It returns a json formatted string via the serial port. 

TO DO
------
- dodati connect gumb i listWidget za biranje porta (baud rate je hardkodirani na 115200)
- dodati automatsko prepoznavanje uređaja
- dodati provjeru veze u radu aplikacije
- u slucaju da veza pukne, prekinuti recording (ako je u toku )i zasiviti gumb START RECORDING
