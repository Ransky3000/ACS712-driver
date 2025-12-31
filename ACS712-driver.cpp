#include "ACS712-driver.h"

ACS712::ACS712(int pin, float voltageReference, int adcResolution) {
    _pin = pin;
    _voltageReference = voltageReference;
    _adcResolution = adcResolution;
    _sensitivity = 0.185; // Default to 5A model (185mV/A = 0.185V/A)
    _zeroPoint = _adcResolution / 2; // Default to half scale
}

void ACS712::begin() {
    pinMode(_pin, INPUT);
}

void ACS712::setSensitivity(float sensitivity) {
    _sensitivity = sensitivity;
}

int ACS712::calibrate() {
    long accumulator = 0;
    int samples = 100;
    
    for (int i = 0; i < samples; i++) {
        accumulator += analogRead(_pin);
        delay(2);
    }
    
    _zeroPoint = accumulator / samples;
    return _zeroPoint;
}

float ACS712::readCurrentDC() {
    long accumulator = 0;
    int samples = 10;
    
    for (int i = 0; i < samples; i++) {
        accumulator += analogRead(_pin);
        delay(1); 
    }
    
    int adcValue = accumulator / samples;
    
    // Formula: (Voltage - ZeroPointVoltage) / Sensitivity
    // Voltage = (adcValue / resolution) * Vref
    // ZeroPointVoltage = (zeroPoint / resolution) * Vref
    
    float voltage = adcToVoltage(adcValue);
    float zeroPointVoltage = adcToVoltage(_zeroPoint);
    
    return (voltage - zeroPointVoltage) / _sensitivity;
}

float ACS712::readCurrentAC(int frequency) {
    int period = 1000000 / frequency;
    unsigned long startTime = micros();
    
    unsigned long accumulator = 0;
    unsigned long samples = 0;
    
    while ((micros() - startTime) < period) {
        int adcValue = analogRead(_pin);
        float voltage = adcToVoltage(adcValue);
        float zeroPointVoltage = adcToVoltage(_zeroPoint);
        float current = (voltage - zeroPointVoltage) / _sensitivity;
        
        accumulator += (current * current); 
        samples++;
    }
    
    // RMS = sqrt(mean(squares))
    // Note: This is an approximation. For high precision, more sophisticated sampling is needed,
    // but this behaves well for general Arduino use cases.
    if (samples == 0) return 0;
    
    float mean = accumulator / (float)samples;
    return sqrt(mean);
}

int ACS712::getZeroPoint() {
    return _zeroPoint;
}

float ACS712::adcToVoltage(int adcValue) {
    return (adcValue / (float)_adcResolution) * _voltageReference;
}
