# ACS712 Driver Library

A lightweight Arduino library for the ACS712 Hall effect current sensor. Supports both DC and AC (RMS) current readings with easy calibration.

## Features
- **readCurrentDC()**: Read DC current (e.g., batteries).
- **readCurrentAC()**: Read AC current (RMS) (e.g., mains).
- **calibrate()**: Automatically find the zero-point offset.
- **Accuracy**: Uses 100-sample averaging for high stability.
- **setSensitivity()**: Support for 5A, 20A, and 30A modules.

## Installation

1. Download this repository as a ZIP file.
2. Open the Arduino IDE.
3. Go to **Sketch** > **Include Library** > **Add .ZIP Library...**
4. Select the downloaded ZIP.

## Wiring

| ACS712 | Arduino |
|:-------|:--------|
| VCC    | 5V      |
| GND    | GND     |
| OUT    | A0 (or any Analog Pin) |

## Quick Start

```cpp
#include <ACS712-driver.h>

// Pin A0, 5.0V reference, 1023 ADC resolution
ACS712 sensor(A0, 5.0, 1023);

void setup() {
  Serial.begin(9600);
  sensor.setSensitivity(0.185); // 0.185 for 5A, 0.100 for 20A, 0.066 for 30A
  sensor.calibrate();
}

void loop() {
  float amps = sensor.readCurrentDC();
  Serial.println(amps);
  delay(100);
}
```

## API Documentation

### `ACS712(int pin, float voltageReference, int adcResolution)`
Constructor.
- `pin`: Analog pin connected to OUT.
- `voltageReference`: System voltage (usually 5.0 or 3.3).
- `adcResolution`: ADC bit depth (usually 1023).

### `void setSensitivity(float sensitivity)`
Sets the sensitivity in Volts per Ampere.
- 5A module: 0.185
- 20A module: 0.100
- 30A module: 0.066

### `int calibrate()`
Reads the sensor 100 times to find the average zero-point offset. Returns the calculated offset.

### `float readCurrentDC()`
Reads the current value for DC circuits.

### `float readCurrentAC(int frequency)`
Reads the RMS current for AC circuits. Default frequency is 60Hz. This is a blocking function.

## Non-Blocking API
For applications where you cannot afford to block execution (e.g., LED or motor control).

### `bool update()`
Call this in your `loop()` as often as possible. Returns `true` when a new reading is available.

### `float getAmps()`
Returns the last calculated DC current value.

## License
MIT License
