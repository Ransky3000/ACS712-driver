/*
  BasicAC.ino - Example of reading AC Current (RMS)
  
  This sketch measures the RMS current of an AC load (like a lamp, heater, or motor).
  Note: readCurrentAC() acts as a blocking function for at least one full cycle 
  (16.6ms for 60Hz, 20ms for 50Hz).
*/

#include <ACS712-driver.h>

// ACS712 5A  uses 185 mV/A
// ACS712 20A uses 100 mV/A
// ACS712 30A uses 66 mV/A

ACS712 sensor(A0, 5.0, 1023); 

void setup() {
  Serial.begin(9600);
  
  // Set your sensor type here
  sensor.setSensitivity(0.100); // 20A model
  
  Serial.println("Calibrating zero point... Ensure no current is flowing.");
  sensor.calibrate();
  Serial.println("Ready!");
}

void loop() {
  // Read RMS Current at 60Hz
  // For 50Hz (Europe/Asia), change to: sensor.readCurrentAC(50)
  float ampsRMS = sensor.readCurrentAC(60);
  
  Serial.print("Current: ");
  Serial.print(ampsRMS, 3);
  Serial.println(" A (RMS)");
  
  delay(500);
}
