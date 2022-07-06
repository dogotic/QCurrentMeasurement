# QCurrentMeasurement
GUI for the INA219 current measurement device 

PROTOCOL
---------
The current measuring device sends constantly the following stream of data

| BYTE_0             | BYTE_1             | BYTE_2             | BYTE_3             | BYTE_4             | BYTE_5             | BYTE_5             | 
|--------------------|--------------------|--------------------|--------------------|--------------------|--------------------|--------------------|
| QCurrentMeasurement|bus_voltage         |load_voltage        |shunt_voltage       |current_mA          |power_mW            |\r\n                |

The first byte is always the string QCurrentMeasurement, so we can verify that the correct device has been connected.


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
