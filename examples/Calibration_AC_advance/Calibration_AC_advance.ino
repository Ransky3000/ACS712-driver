/*
   Calibration_AC_advance.ino
   
   This example shows how to calibrate the sensor using a known AC LOAD.
   It saves the simplified Zero Point and Sensitivity to EEPROM.
   
   NOTE: This overwrites the global sensitivity. If you calibrate for AC, 
   it might slightly affect DC readings depending on your sensor's linearity.
*/

#include <ACS712-driver.h>
#include <EEPROM.h>

// Pins
ACS712 sensor(A0, 5.0, 1023);

const int calVal_eepromAdress_Zero = 0;
const int calVal_eepromAdress_Sens = 4;
unsigned long t = 0;

void setup() {
  Serial.begin(9600); delay(10);
  Serial.println();
  Serial.println("Starting AC Calibration...");
  
  // Default start
  sensor.setSensitivity(0.185); 
  
  Serial.println("Send 't' to Tare, 'r' to Calibrate, 'c' to Edit Manual.");
}

void loop() {
  // Print AC Current regularly
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 1000) {
    float amps = sensor.readCurrentAC(60); // 60Hz
    Serial.print("AC Current: ");
    Serial.print(amps, 3);
    Serial.println(" A");
    lastPrint = millis();
  }

  // Command handling
  if (Serial.available() > 0) {
    char inByte = Serial.read();
    
    // Stop printing while we handle commands
    if (inByte == 't') {
        Serial.println("Taring Zero Point...");
        Serial.println("Ensure NO current is flowing.");
        
        sensor.calibrate(); 
        
        Serial.println("Tare complete. Press 'enter' to continue.");
        while(Serial.available() > 0) Serial.read(); 
        while(!Serial.available()); 
        while(Serial.available() > 0) Serial.read(); 
    }
    else if (inByte == 'r') {
        calibrateAC(); 
    }
    else if (inByte == 'c') {
        changeSavedCalFactor();
    }
    
    // Flush
    while(Serial.available() > 0 && Serial.read() != '\n');
  }
}

void calibrateAC() {
  Serial.println("***");
  Serial.println("Start AC calibration:");
  Serial.println("Connect a KNOWN AC Load (e.g. 1.0 Amp).");
  Serial.println("Then send the value of this current IN mA (e.g. 1000).");

  float known_current_mA = 0;
  boolean _resume = false;
  
  while (_resume == false) {
    if (Serial.available() > 0) {
      known_current_mA = Serial.parseFloat();
      if (known_current_mA != 0) {
        Serial.print("Known current: ");
        Serial.print(known_current_mA);
        Serial.println(" mA");
        _resume = true;
      }
      while(Serial.available() > 0 && Serial.read() != '\n'); 
    }
  }

  Serial.println("Reading Sensor... Please wait.");
  
  // 1. Temporarily set sensitivity to 1.0 so we get "RMS Volts (minus zero)" directly
  float oldSens = sensor.getSensitivity();
  sensor.setSensitivity(1.0);
  
  // 2. Take multiple readings to average
  float accumulator = 0;
  int samples = 10;
  for(int i=0; i<samples; i++) {
    accumulator += sensor.readCurrentAC(60);
    delay(10);
  }
  float average_rms_volts = accumulator / samples;
  
  // 3. Restore sensitivity for now
  sensor.setSensitivity(oldSens);
  
  // 4. Calculate actual Sensitivity
  // Sensitivity = RMS_Volts / Known_Amps
  float known_current_Amps = known_current_mA / 1000.0;
  float newSensitivity = average_rms_volts / known_current_Amps;
  
  // Sanity check
  if (newSensitivity < 0) newSensitivity = -newSensitivity;

  Serial.print("Calculated AC Sensitivity: ");
  Serial.print(newSensitivity, 4);
  Serial.println(" V/A");
  
  sensor.setSensitivity(newSensitivity);

  Serial.print("Save this value to EEPROM? y/n");

  _resume = false;
  while (_resume == false) {
    if (Serial.available() > 0) {
      char inByte = Serial.read();
      if (inByte == 'y') {
        float zero = sensor.getZeroPoint();
        EEPROM.put(calVal_eepromAdress_Zero, zero);
        EEPROM.put(calVal_eepromAdress_Sens, newSensitivity);
        Serial.println("Values saved to EEPROM.");
        _resume = true;
      }
      else if (inByte == 'n') {
        Serial.println("Value not saved.");
        _resume = true;
      }
      while(Serial.available() > 0 && Serial.read() != '\n'); 
    }
  }

  Serial.println("End calibration");
  Serial.println("***");
}

void changeSavedCalFactor() {
  float oldSensitivity = sensor.getSensitivity();
  boolean _resume = false;
  Serial.println("***");
  Serial.print("Current sensitivity is: ");
  Serial.println(oldSensitivity, 4);
  Serial.println("Now, send the new value from serial monitor, i.e. 0.185");
  
  float newSensitivity;
  while (_resume == false) {
    if (Serial.available() > 0) {
      newSensitivity = Serial.parseFloat();
      if (newSensitivity != 0) {
        Serial.print("New sensitivity is: ");
        Serial.println(newSensitivity, 4);
        sensor.setSensitivity(newSensitivity);
        _resume = true;
      }
      while(Serial.available() > 0 && Serial.read() != '\n'); 
    }
  }
  
  _resume = false;
  Serial.print("Save this value to EEPROM? y/n");
  while (_resume == false) {
    if (Serial.available() > 0) {
      char inByte = Serial.read();
      if (inByte == 'y') {
        float zero = sensor.getZeroPoint(); // float
        EEPROM.put(calVal_eepromAdress_Zero, zero);
        EEPROM.put(calVal_eepromAdress_Sens, newSensitivity);
        Serial.println("Values saved to EEPROM");
        _resume = true;
      }
      else if (inByte == 'n') {
        Serial.println("Value not saved to EEPROM");
        _resume = true;
      }
      while(Serial.available() > 0 && Serial.read() != '\n'); 
    }
  }
  Serial.println("End change value");
  Serial.println("***");
}
