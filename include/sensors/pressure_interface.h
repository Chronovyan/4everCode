#ifndef CHRONOVYAN_PRESSURE_INTERFACE_H
#define CHRONOVYAN_PRESSURE_INTERFACE_H

#include <chrono>
#include <string>

namespace chronovyan {
namespace sensors {

/**
 * @class PressureReading
 * @brief Represents a pressure reading from a sensor with error handling capability
 */
class PressureReading {
public:
    /**
     * @brief Default constructor creates a null reading
     */
    PressureReading() : m_value(0.0f), m_isValid(false), m_errorMessage("") {}

    /**
     * @brief Constructor for a valid pressure reading
     * @param value The pressure value
     */
    explicit PressureReading(float value) : m_value(value), m_isValid(true), m_errorMessage("") {}

    /**
     * @brief Check if the reading is valid
     * @return true if valid, false if null or error
     */
    bool isValid() const { return m_isValid; }

    /**
     * @brief Get the pressure value
     * @return The pressure value (0.0 if invalid)
     */
    float getValue() const { return m_value; }

    /**
     * @brief Get the error message if any
     * @return Error message or empty string if no error
     */
    std::string getErrorMessage() const { return m_errorMessage; }

    /**
     * @brief Create an error reading with a specific error message
     * @param errorMessage The error message to include
     * @return A PressureReading object with error state
     */
    static PressureReading createErrorReading(const std::string& errorMessage) {
        PressureReading reading;
        reading.m_isValid = false;
        reading.m_errorMessage = errorMessage;
        return reading;
    }

private:
    float m_value;
    bool m_isValid;
    std::string m_errorMessage;
};

/**
 * @class PressureSensor
 * @brief Base interface for pressure sensors in the Chronovyan system
 */
class PressureSensor {
public:
    /**
     * @brief Virtual destructor
     */
    virtual ~PressureSensor() = default;

    /**
     * @brief Initialize the sensor
     * @return true if initialization successful, false otherwise
     */
    virtual bool initialize() = 0;

    /**
     * @brief Get a pressure reading from the sensor
     * @return The current pressure reading
     */
    virtual PressureReading getPressureReading() = 0;

    /**
     * @brief Get a pressure reading with error handling
     * @return A valid reading or an error reading if sensor state is invalid
     */
    virtual PressureReading getPressureReadingSafe() {
        if (!validateSensorState()) {
            return PressureReading::createErrorReading("Sensor state invalid");
        }
        return getPressureReading();
    }

    /**
     * @brief Validate that the sensor is in a valid state
     * @return true if sensor state is valid, false otherwise
     */
    virtual bool validateSensorState() = 0;

    /**
     * @brief Reset the sensor to its initial state
     * @return true if reset successful, false otherwise
     */
    virtual bool reset() = 0;
};

/**
 * @class WhisperingStoneSensor
 * @brief Interface for the Whispering Stone pressure sensor
 */
class WhisperingStoneSensor : public PressureSensor {
public:
    /**
     * @brief Reset the sensor after a temporal surge event
     * @return true if reset successful, false otherwise
     */
    virtual bool resetAfterSurge() = 0;

    /**
     * @brief Track the magnitude of the last surge event
     * @param magnitude The magnitude of the surge on the Chronal Scale
     */
    virtual void setLastSurgeMagnitude(float magnitude) = 0;

    /**
     * @brief Reinitialize the sensor hardware connection
     * @return true if successful, false otherwise
     */
    virtual bool reinitializeSensorHardware() = 0;

    /**
     * @brief Perform a self-test to validate sensor functionality
     * @return true if the sensor is functioning correctly, false otherwise
     */
    virtual bool performSelfTest() = 0;
};

}  // namespace sensors
}  // namespace chronovyan

#endif  // CHRONOVYAN_PRESSURE_INTERFACE_H