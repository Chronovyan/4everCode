#ifndef CHRONOVYAN_WHISPERING_STONE_H
#define CHRONOVYAN_WHISPERING_STONE_H

#include <chrono>
#include <memory>
#include <vector>

#include "pressure_interface.h"

namespace chronovyan {
namespace sensors {

/**
 * @class WhisperingStoneSensorImpl
 * @brief Concrete implementation of the WhisperingStoneSensor interface
 */
class WhisperingStoneSensorImpl : public WhisperingStoneSensor {
public:
    /**
     * @brief Constructor
     */
    WhisperingStoneSensorImpl();

    /**
     * @brief Destructor
     */
    ~WhisperingStoneSensorImpl() override;

    /**
     * @brief Initialize the Whispering Stone sensor
     * @return true if initialization successful, false otherwise
     */
    bool initialize() override;

    /**
     * @brief Get a pressure reading from the Whispering Stone
     * @return The current pressure reading
     */
    PressureReading getPressureReading() override;

    /**
     * @brief Validate that the sensor is in a valid state
     * @return true if sensor state is valid, false otherwise
     */
    bool validateSensorState() override;

    /**
     * @brief Reset the sensor to its initial state
     * @return true if reset successful, false otherwise
     */
    bool reset() override;

    /**
     * @brief Reset the sensor after a temporal surge event
     * @return true if reset successful, false otherwise
     */
    bool resetAfterSurge() override;

    /**
     * @brief Track the magnitude of the last surge event
     * @param magnitude The magnitude of the surge on the Chronal Scale
     */
    void setLastSurgeMagnitude(float magnitude) override;

    /**
     * @brief Reinitialize the sensor hardware connection
     * @return true if successful, false otherwise
     */
    bool reinitializeSensorHardware() override;

    /**
     * @brief Perform a self-test to validate sensor functionality
     * @return true if the sensor is functioning correctly, false otherwise
     */
    bool performSelfTest() override;

private:
    // Sensor state enumeration
    enum SensorState {
        SENSOR_STATE_UNINITIALIZED = 0,
        SENSOR_STATE_READY = 1,
        SENSOR_STATE_ERROR = 2,
        SENSOR_STATE_CALIBRATING = 3
    };

    // Internal buffer structure
    struct Buffer {
        std::vector<float> data;
        size_t capacity;
        size_t size;

        Buffer(size_t cap = 1024) : capacity(cap), size(0) { data.resize(capacity); }

        void clear() { size = 0; }

        bool isValid() const { return size <= capacity; }
    };

    // Take a raw pressure reading
    float takeRawReading();

    // Constants
    static constexpr float MINIMUM_VALID_READING = 0.1f;
    static constexpr float MAXIMUM_VALID_READING = 999.9f;

    // Internal state
    SensorState m_sensorState;
    float m_lastSurgeMagnitude;
    bool m_surgeDetected;

    // Buffers
    Buffer m_primaryBuffer;
    Buffer m_secondaryBuffer;
    Buffer m_pressureReadingsBuffer;

    // Indices and counters
    size_t m_readIndex;
    size_t m_writeIndex;
    size_t m_readingsCount;

    // Calibration data
    struct CalibrationData {
        float offset;
        float scale;
        std::chrono::system_clock::time_point lastCalibration;

        void reset() {
            offset = 0.0f;
            scale = 1.0f;
            lastCalibration = std::chrono::system_clock::now();
        }
    } m_calibrationData;
};

}  // namespace sensors
}  // namespace chronovyan

#endif  // CHRONOVYAN_WHISPERING_STONE_H