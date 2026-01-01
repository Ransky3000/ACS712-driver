/*
  AdvancedCalibration.ino - Interactive Calibration with EEPROM support
  
  This sketch guides you through:
  1. Calibrating the Zero Point (Tare)
  2. Calibrating the Sensitivity (Slope) using a known load
  3. Saving these values to EEPROM (or printing them if you don't use EEPROM)
  
  Usage:
  - Open Serial Monitor (9600 baud)
  - Follow the on-screen instructions
*/

#include <ACS712-driver.h>
#include <EEPROM.h>

// Connect the output of the ACS712 to Analog Pin 0
ACS712 sensor(A0, 5.0, 1023);

// EEPROM Addresses
const int ADDR_ZERO_POINT = 0;   // Takes 2 bytes (int)
const int ADDR_SENSITIVITY = 4;  // Takes 4 bytes (float)

void setup() {
  Serial.begin(9600);
  
  // Wait for serial to be ready
  while (!Serial) { delay(10); }

  Serial.println("\n=== ACS712 Advanced Calibration Tool ===");
  Serial.println("Commands:");
  Serial.println("  't' - Tare (Calibrate Zero Point)");
  Serial.println("  'c' - Calibrate Slope (requires known current)");
  Serial.println("  's' - Save current settings to EEPROM");
  Serial.println("  'l' - Load settings from EEPROM");
  Serial.println("  'r' - Reset to defaults");
  Serial.println("========================================");
  
  // Default start
  sensor.setSensitivity(0.185); // Start with typical 5A sensitivity
}

void loop() {
  if (Serial.available()) {
    char cmd = Serial.read();
    // Flush rest of line
    while(Serial.available() > 0 && Serial.read() != '\n'); 

    if (cmd == 't') {
      calibrateZero();
    } else if (cmd == 'c') {
      calibrateSlope();
    } else if (cmd == 's') {
      saveToEEPROM();
    } else if (cmd == 'l') {
      loadFromEEPROM();
    } else if (cmd == 'r') {
      sensor.setSensitivity(0.185);
      Serial.println("Reset to default sensitivity (0.185 V/A). Zero point not changed.");
    }
  }

  // Show live reading
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 500) {
    float current = sensor.readCurrentDC();
    Serial.print("Current: ");
    Serial.print(current, 3);
    Serial.print(" A | Zero: ");
    Serial.print(sensor.getZeroPoint());
    Serial.print(" | Sens: ");
    Serial.println(sensor.getSensitivity(), 4);
    lastPrint = millis();
  }
}

void calibrateZero() {
  Serial.println("\n--- Zero Point Calibration ---");
  Serial.println("Ensure NO Current is flowing through the sensor.");
  Serial.println("Type 'y' and press ENTER when ready...");
  
  while (!Serial.available());
  char c = Serial.read();
  // Flush
  while(Serial.available() > 0 && Serial.read() != '\n');
  
  if (c == 'y' || c == 'Y') {
    Serial.println("Calibrating...");
    int zero = sensor.calibrate();
    Serial.print("New Zero Point: ");
    Serial.println(zero);
  } else {
    Serial.println("Cancelled.");
  }
}

void calibrateSlope() {
  Serial.println("\n--- Slope (Sensitivity) Calibration ---");
  Serial.println("1. You must have already Calibrated Zero ('t').");
  Serial.println("2. Apply a KNOWN current (e.g., 1.0 Amp constant load).");
  Serial.println("   (Ideally use a multimeter in series to verify the actual current)");
  Serial.println("Enter the KNOWN current in Amps (e.g., 1.05) and press ENTER:");
  
  while (!Serial.available());
  float knownCurrent = Serial.parseFloat();
  // Flush
  while(Serial.available() > 0 && Serial.read() != '\n');

  if (knownCurrent == 0) {
    Serial.println("Error: Current cannot be zero for slope calibration. Use 't' instead.");
    return;
  }
  
  Serial.print("Measuring current with OLD sensitivity...");
  
  // Take a good long average
  long adcSum = 0;
  int samples = 50;
  for (int i=0; i<samples; i++) {
    adcSum += analogRead(A0);
    delay(10);
  }
  int avgAdc = adcSum / samples;
  
  // Calculate voltage from the reading
  // Volts = (ADC / 1023) * 5.0
  float voltageRead = (avgAdc / 1023.0) * 5.0;
  
  // Calculate voltage at Zero Point
  // VoltsZero = (Zero / 1023) * 5.0
  float voltageZero = (sensor.getZeroPoint() / 1023.0) * 5.0;
  
  // The logic:
  // Current = (VoltageRead - VoltageZero) / Sensitivity
  // Therefore: Sensitivity = (VoltageRead - VoltageZero) / KnownCurrent
  
  float diffVoltage = voltageRead - voltageZero;
  float newSensitivity = diffVoltage / knownCurrent;
  
  // Safety check, sensitivity should be positive
  if (newSensitivity < 0) newSensitivity = -newSensitivity;
  
  Serial.println(" Done.");
  Serial.print("Old Sensitivity: "); Serial.println(sensor.getSensitivity(), 4);
  Serial.print("New Sensitivity: "); Serial.println(newSensitivity, 4);
  
  sensor.setSensitivity(newSensitivity);
  Serial.println("Sensitivity updated! Don't forget to Save ('s').");
}

void saveToEEPROM() {
  Serial.println("\n--- Saving to EEPROM ---");
  int zero = sensor.getZeroPoint();
  float sens = sensor.getSensitivity();
  
  EEPROM.put(ADDR_ZERO_POINT, zero);
  EEPROM.put(ADDR_SENSITIVITY, sens);
  
  Serial.println("Saved!");
}

void loadFromEEPROM() {
  Serial.println("\n--- Loading from EEPROM ---");
  int zero;
  float sens;
  
  EEPROM.get(ADDR_ZERO_POINT, zero);
  EEPROM.get(ADDR_SENSITIVITY, sens);
  
  Serial.print("Loaded Zero: "); Serial.println(zero);
  Serial.print("Loaded Sens: "); Serial.println(sens, 4);
  
  // Basic validation
  if (zero < 0 || zero > 1023 || sens < 0.001 || sens > 1.0) {
    Serial.println("Warning: EEPROM values look invalid. Ignoring.");
  } else {
    sensor.setZeroPoint(zero);
    sensor.setSensitivity(sens);
    Serial.println("Applied settings.");
  }
}
