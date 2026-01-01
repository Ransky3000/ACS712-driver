/*
  NonBlocking.ino - Example of non-blocking current reading
  
  This example shows how to use the update() method inside loop()
  to read current without stopping your other code (like blinking an LED).
*/

#include <ACS712-driver.h>

ACS712 sensor(A0, 5.0, 1023);

unsigned long lastBlink = 0;
bool ledState = false;

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  
  sensor.setSensitivity(0.185);
  sensor.calibrate(); // This is still blocking, but only runs once!
}

void loop() {
  // 1. Call update() as fast as possible
  if (sensor.update()) {
    // Returns true when a new average is ready (every ~10ms)
    float amps = sensor.getAmps();
    
    // Print only occasionally so we don't spam Serial
    static unsigned long lastPrint = 0;
    if (millis() - lastPrint > 500) {
      Serial.print("Non-Blocking Current: ");
      Serial.println(amps);
      lastPrint = millis();
    }
  }
  
  // 2. Do other things simultaneously!
  // Blink the LED every 200ms without delay()
  if (millis() - lastBlink > 200) {
    ledState = !ledState;
    digitalWrite(LED_BUILTIN, ledState);
    lastBlink = millis();
  }
}
