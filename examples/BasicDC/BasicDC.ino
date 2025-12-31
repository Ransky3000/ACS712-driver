/*
  BasicDC.ino - Example for ACS712 Current Sensor Library
  Counts as a simple usage demonstration.
  
  Created by Ransky3000
*/

#include <ACS712-driver.h>

// Connect the output of the ACS712 to Analog Pin 0
// Connect VCC to 5V and GND to GND
ACS712 sensor(A0, 5.0, 1023);

void setup() {
  Serial.begin(9600);
  
  // Set sensitivity for your specific module:
  // 185 mV/A for 5A model
  // 100 mV/A for 20A model
  // 66 mV/A for 30A model
  sensor.setSensitivity(0.185); 

  Serial.println("Calibrating... Ensure no current is flowing.");
  int zeroPoint = sensor.calibrate();
  Serial.print("Zero Point: ");
  Serial.println(zeroPoint);
  Serial.println("Ready!");
}

void loop() {
  float current = sensor.readCurrentDC();
  
  Serial.print("Current: ");
  Serial.print(current);
  Serial.println(" A");
  
  delay(500);
}
