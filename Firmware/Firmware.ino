#include <Wire.h>
#include <Adafruit_INA219.h>
#include <ArduinoJson.h>

Adafruit_INA219 ina219;

StaticJsonDocument<200> doc;

void setup(void) 
{
  Serial.begin(115200);
  while (!Serial) {
      // will pause Zero, Leonardo, etc until serial console opens
      delay(1);
  }
  
  // Initialize the INA219.
  // By default the initialization will use the largest range (32V, 2A).  However
  // you can call a setCalibration function to change this range (see comments).
  if (! ina219.begin()) {
    Serial.println("Failed to find INA219 chip");
    while (1) { delay(10); }
  }
  // To use a slightly lower 32V, 1A range (higher precision on amps):
  //ina219.setCalibration_32V_1A();
  // Or to use a lower 16V, 400mA range (higher precision on volts and amps):
  //ina219.setCalibration_16V_400mA();
 
}

void loop(void) 
{
  float shuntvoltage = 0;
  float busvoltage = 0;
  float current_mA = 0;
  float loadvoltage = 0;
  float power_mW = 0;

  shuntvoltage = ina219.getShuntVoltage_mV();
  busvoltage = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
  power_mW = ina219.getPower_mW();
  loadvoltage = busvoltage + (shuntvoltage / 1000);

  doc["device_name"] = "QCurrentMeasurement";
  doc["device_version"] = "1.0.0";
  doc["shuntvoltage"] = shuntvoltage;
  doc["busvoltage"] = busvoltage;
  doc["current_mA"] =  current_mA;
  doc["power_mW"] =  power_mW;
  doc["loadvoltage"] = loadvoltage;

  serializeJson(doc, Serial);

  delay(100);
}
