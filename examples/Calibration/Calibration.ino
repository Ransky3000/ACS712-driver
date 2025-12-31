/*
  Calibration.ino - Utility to check the raw Zero Point of the ACS712
  Use this to find the offset value if you want to hardcode it later,
  or just to check if your sensor is noisy.
*/

#include <ACS712-driver.h>

ACS712 sensor(A0, 5.0, 1023);

void setup() {
  Serial.begin(9600);
  Serial.println("ACS712 Calibration Tool");
  Serial.println("Ensure NO Current is flowing through the sensor.");
  delay(1000);
  
  Serial.println("Calibrating...");
  int zero = sensor.calibrate();
  
  Serial.print("Calculated Zero Point: ");
  Serial.println(zero);
  
  if (zero < 500 || zero > 524) {
    Serial.println("Warning: Zero point is quite far from 512 (ideal center).");
    Serial.println("Check power supply stability or sensor wiring.");
  } else {
    Serial.println("Zero point looks good!");
  }
}

void loop() {
  // Just print the raw value to see noise
  int raw = analogRead(A0);
  Serial.print("Raw: ");
  Serial.println(raw);
  delay(100);
}
