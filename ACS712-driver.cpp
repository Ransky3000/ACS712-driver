#include "ACS712-driver.h"

ACS712::ACS712(int pin, float voltageReference, int adcResolution) {
    _pin = pin;
    _voltageReference = voltageReference;
    _adcResolution = adcResolution;
    _sensitivity = 0.185; // Default to 5A model (185mV/A = 0.185V/A)
    _zeroPoint = _adcResolution / 2; // Default to half scale
    
    // Non-blocking init
    _lastSampleTime = 0;
    _accumulator = 0;
    _sampleCount = 0;
    _lastAmps = 0.0;
}

void ACS712::begin() {
    pinMode(_pin, INPUT);
}

void ACS712::setSensitivity(float sensitivity) {
    _sensitivity = sensitivity;
}

float ACS712::calibrate() {
    long accumulator = 0;
    int samples = 100;
    
    for (int i = 0; i < samples; i++) {
        accumulator += analogRead(_pin);
        delay(2);
    }
    
    _zeroPoint = accumulator / (float)samples;
    return _zeroPoint;
}

float ACS712::readCurrentDC() {
    long accumulator = 0;
    int samples = 100; // Increased for better accuracy
    
    for (int i = 0; i < samples; i++) {
        accumulator += analogRead(_pin);
        // Removed delay(1) to keep it reasonably fast even with high samples
        // The loop execution itself provides small settling time
    }
    
    // Use float for average to keep decimal precision from oversampling
    float avgAdc = accumulator / (float)samples;
    
    // Formula: (Voltage - ZeroPointVoltage) / Sensitivity
    // Voltage = (avgAdc / resolution) * Vref
    // ZeroPointVoltage = (zeroPoint / resolution) * Vref
    
    float voltage = adcToVoltage(avgAdc);
    float zeroPointVoltage = adcToVoltage(_zeroPoint);
    
    return (voltage - zeroPointVoltage) / _sensitivity;
}

float ACS712::readCurrentAC(int frequency) {
    int period = 1000000 / frequency;
    unsigned long startTime = micros();
    
    float accumulator = 0; // Changed from unsigned long to float to prevent truncation!
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

float ACS712::getZeroPoint() {
    return _zeroPoint;
}

void ACS712::setZeroPoint(float zeroPoint) {
    _zeroPoint = zeroPoint;
}

float ACS712::getSensitivity() {
    return _sensitivity;
}

bool ACS712::update() {
    unsigned long now = micros();
    // Sample every 1ms (1000 micros)
    if (now - _lastSampleTime >= 500) { // Increased speed (0.5ms) to fill buffer faster
        _lastSampleTime = now;
        _accumulator += analogRead(_pin);
        _sampleCount++;

        if (_sampleCount >= 100) { // Accumulate 100 samples
            float avgAdc = _accumulator / 100.0;
            float voltage = adcToVoltage(avgAdc);
            float zeroPointVoltage = adcToVoltage(_zeroPoint);
            _lastAmps = (voltage - zeroPointVoltage) / _sensitivity;
            
            // Reset
            _accumulator = 0;
            _sampleCount = 0;
            return true; // New data available
        }
    }
    return false;
}

float ACS712::getAmps() {
    return _lastAmps;
}

float ACS712::adcToVoltage(float adcValue) { // Changed int to float
    return (adcValue / (float)_adcResolution) * _voltageReference;
}
