# QCurrentMeasurement
GUI for the INA219 current measurement device 

PROTOCOL
---------
The current measuring device sends constantly the following data
[device_name] = "QCurrentMeasurement"
[device_version] = "x.y.z"
[shuntvoltage] = <Measured shunt voltage in mV>
[busvoltage] = <Measured bus voltage in V>
[loadvoltage] = <Measured load voltage in V>
[current_mA] = <Measured current in mA>
[power_mW] = <Measured power in mW>

The string QCurrentMeasurement is used to verify that the correct device is connected and not some other serial2usb device, so we don't read gibberish.


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
