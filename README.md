# QCurrentMeasurement
GUI for the INA219 current measurement device 

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
