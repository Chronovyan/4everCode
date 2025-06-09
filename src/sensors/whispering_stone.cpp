#include <iostream>
#include <random>
#include <thread>

#include "../../include/sensors/whispering_stone.h"

namespace chronovyan {
namespace sensors {

// Simple logger for demonstration purposes
class Logger {
public:
  static void info(const std::string &message) {
    std::cout << "[INFO] " << message << std::endl;
  }

  static void warning(const std::string &message) {
    std::cout << "[WARNING] " << message << std::endl;
  }

  static void error(const std::string &message) {
    std::cout << "[ERROR] " << message << std::endl;
  }
};

// Constructor
WhisperingStoneSensorImpl::WhisperingStoneSensorImpl()
    : m_sensorState(SENSOR_STATE_UNINITIALIZED), m_lastSurgeMagnitude(0.0f),
      m_surgeDetected(false), m_readIndex(0), m_writeIndex(0),
      m_readingsCount(0) {
  // Initialize buffers
  m_primaryBuffer = Buffer(1024);
  m_secondaryBuffer = Buffer(1024);
  m_pressureReadingsBuffer = Buffer(512);

  // Initialize calibration data
  m_calibrationData.reset();

  Logger::info("WhisperingStoneSensor: Constructed");
}

// Destructor
WhisperingStoneSensorImpl::~WhisperingStoneSensorImpl() {
  Logger::info("WhisperingStoneSensor: Destroyed");
}

// Initialize the sensor
bool WhisperingStoneSensorImpl::initialize() {
  Logger::info("WhisperingStoneSensor: Initializing");

  // Clear all buffers
  m_primaryBuffer.clear();
  m_secondaryBuffer.clear();
  m_pressureReadingsBuffer.clear();

  // Reset indices and counters
  m_readIndex = 0;
  m_writeIndex = 0;
  m_readingsCount = 0;
  m_surgeDetected = false;
  m_lastSurgeMagnitude = 0.0f;

  // Reset calibration data
  m_calibrationData.reset();

  // Simulate hardware initialization
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  // Set sensor state to ready
  m_sensorState = SENSOR_STATE_READY;

  Logger::info("WhisperingStoneSensor: Initialization complete");
  return true;
}

// Get a pressure reading
PressureReading WhisperingStoneSensorImpl::getPressureReading() {
  // Check if sensor is ready
  if (m_sensorState != SENSOR_STATE_READY) {
    return PressureReading::createErrorReading("Sensor not ready");
  }

  // Take a raw reading
  float rawReading = takeRawReading();

  // Apply calibration
  float calibratedReading =
      rawReading * m_calibrationData.scale + m_calibrationData.offset;

  // Store the reading in the buffer
  if (m_pressureReadingsBuffer.size < m_pressureReadingsBuffer.capacity) {
    m_pressureReadingsBuffer.data[m_pressureReadingsBuffer.size++] =
        calibratedReading;
  }

  return PressureReading(calibratedReading);
}

// Validate sensor state
bool WhisperingStoneSensorImpl::validateSensorState() {
  // Check if the sensor is in READY state
  if (m_sensorState != SENSOR_STATE_READY) {
    return false;
  }

  // Check if all buffers are valid
  if (!m_primaryBuffer.isValid() || !m_secondaryBuffer.isValid() ||
      !m_pressureReadingsBuffer.isValid()) {
    return false;
  }

  // Take a test reading to ensure the sensor is responsive
  float testReading = takeRawReading();
  if (testReading < MINIMUM_VALID_READING ||
      testReading > MAXIMUM_VALID_READING) {
    return false;
  }

  return true;
}

// Reset the sensor
bool WhisperingStoneSensorImpl::reset() {
  Logger::info("WhisperingStoneSensor: Performing basic reset");

  // Clear all buffers
  m_primaryBuffer.clear();
  m_secondaryBuffer.clear();
  m_pressureReadingsBuffer.clear();

  // Reset indices and counters
  m_readIndex = 0;
  m_writeIndex = 0;
  m_readingsCount = 0;

  // Set sensor state to ready
  m_sensorState = SENSOR_STATE_READY;

  return true;
}

// Take a raw reading (simulated)
float WhisperingStoneSensorImpl::takeRawReading() {
  // Simple random number generator for simulation
  static std::random_device rd;
  static std::mt19937 gen(rd());
  static std::uniform_real_distribution<float> dis(5.0f, 15.0f);

  // Simulate some delay
  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  // Return a random value
  return dis(gen);
}

// Set the last surge magnitude
void WhisperingStoneSensorImpl::setLastSurgeMagnitude(float magnitude) {
  m_lastSurgeMagnitude = magnitude;
  m_surgeDetected = true;

  Logger::info("WhisperingStoneSensor: Surge magnitude set to " +
               std::to_string(magnitude));
}

// Reinitialize sensor hardware
bool WhisperingStoneSensorImpl::reinitializeSensorHardware() {
  Logger::info("WhisperingStoneSensor: Reinitializing hardware");

  // Simulate hardware reinitialization
  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  // Return success (in a real implementation, would check hardware status)
  return true;
}

// Perform self-test
bool WhisperingStoneSensorImpl::performSelfTest() {
  Logger::info("WhisperingStoneSensor: Performing self-test");

  // Attempt to take a test reading
  float testReading = takeRawReading();

  // Validate reading is within expected range
  if (testReading < MINIMUM_VALID_READING ||
      testReading > MAXIMUM_VALID_READING) {
    Logger::error(
        "WhisperingStoneSensor: Self-test failed - reading out of range");
    return false;
  }

  // Verify all internal buffers are properly initialized
  if (!m_primaryBuffer.isValid() || !m_secondaryBuffer.isValid() ||
      !m_pressureReadingsBuffer.isValid()) {
    Logger::error(
        "WhisperingStoneSensor: Self-test failed - buffer validation failed");
    return false;
  }

  Logger::info("WhisperingStoneSensor: Self-test passed");
  return true;
}

// NEW FIXED VERSION
bool WhisperingStoneSensorImpl::resetAfterSurge() {
  // Log the reset attempt
  Logger::info(
      "WhisperingStoneSensor: Performing comprehensive reset after surge");

  // First, preserve the surge magnitude for diagnostic purposes
  float surgeMagnitude = m_lastSurgeMagnitude;

  // Complete reinitialization of ALL internal buffers
  m_primaryBuffer.clear();
  m_secondaryBuffer.clear();
  m_pressureReadingsBuffer.clear();

  // Reset all internal pointers and counters
  m_readIndex = 0;
  m_writeIndex = 0;
  m_readingsCount = 0;
  m_surgeDetected = false;
  m_lastSurgeMagnitude = 0.0f;

  // Reinitialize the sensor hardware connection
  if (!reinitializeSensorHardware()) {
    m_sensorState = SENSOR_STATE_ERROR;
    Logger::error(
        "WhisperingStoneSensor: Failed to reinitialize sensor hardware");
    return false;
  }

  // Perform self-test to validate sensor functionality
  if (!performSelfTest()) {
    m_sensorState = SENSOR_STATE_ERROR;
    Logger::error("WhisperingStoneSensor: Self-test failed after reset");
    return false;
  }

  // Only set to READY if validation succeeds
  m_sensorState = SENSOR_STATE_READY;

  // Log success with diagnostic information
  Logger::info(
      "WhisperingStoneSensor: Reset successful after surge of magnitude " +
      std::to_string(surgeMagnitude));

  return true;
}

} // namespace sensors
} // namespace chronovyan