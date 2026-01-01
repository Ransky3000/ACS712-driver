#ifndef ACS712_DRIVER_H
#define ACS712_DRIVER_H

#include <Arduino.h>

class ACS712 {
public:
    /**
     * @brief Constructor
     * @param pin The analog pin connected to the sensor's VOUT
     * @param voltageReference The Arduino voltage reference (usually 5.0 or 3.3)
     * @param adcResolution The ADC resolution (usually 1023 for 10-bit)
     */
    ACS712(int pin, float voltageReference = 5.0, int adcResolution = 1023);

    /**
     * @brief Initialize the sensor (sets pinMode)
     */
    void begin();

    /**
     * @brief Set the sensitivity of the sensor.
     * Common values:
     * - ACS712-05B: 0.185 V/A
     * - ACS712-20A: 0.100 V/A
     * - ACS712-30A: 0.066 V/A
     * @param sensitivity Volts per Ampere (V/A)
     */
    void setSensitivity(float sensitivity);

    /**
     * @brief Calibrate the zero point offset.
     * Ensure no current is flowing through the sensor when calling this.
     * @return The calculated zero point ADC value.
     */
    float calibrate();

    /**
     * @brief Read DC Current
     * @return Current in Amperes
     */
    float readCurrentDC();

    /**
     * @brief Read AC Current (RMS)
     * @param frequency The frequency of the AC signal in Hz (default 60Hz)
     * @return RMS Current in Amperes
     */
    float readCurrentAC(int frequency = 60);

    // --- Non-Blocking API ---

    /**
     * @brief Update routine, call this in your loop() as fast as possible.
     * @return true if a new sample set is adequate and a new current value is calculated.
     */
    bool update();

    /**
     * @brief Get the last calculated DC Current (from update).
     * @return Current in Amperes
     */
    float getAmps();

    // ------------------------

    /**
     * @brief Get the currently set zero point
     * @return Zero point ADC value
     */
    float getZeroPoint();

    /**
     * @brief Manually set the zero point (e.g. from EEPROM)
     * @param zeroPoint The ADC value to use as zero
     */
    void setZeroPoint(float zeroPoint);

    /**
     * @brief Get the current sensitivity
     * @return Sensitivity in V/A
     */
    float getSensitivity();

private:
    int _pin;
    float _voltageReference;
    int _adcResolution;
    float _sensitivity;
    float _zeroPoint;
    
    float adcToVoltage(float adcValue);

    // Non-blocking state
    unsigned long _lastSampleTime;
    long _accumulator;
    int _sampleCount;
    float _lastAmps;
};

#endif
