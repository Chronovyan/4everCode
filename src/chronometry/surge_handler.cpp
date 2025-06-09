#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include "../../include/sensors/whispering_stone.h"

namespace chronovyan {
namespace chronometry {

// Forward declarations
namespace ui {
class UserInterface {
public:
  void displayWarning(const std::string &message) {
    std::cout << "[UI WARNING] " << message << std::endl;
  }
};
} // namespace ui

namespace monitoring {
class SystemMonitor {
public:
  void flagComponentForMaintenance(const std::string &componentName) {
    std::cout << "[SYSTEM MONITOR] Component flagged for maintenance: "
              << componentName << std::endl;
  }
};
} // namespace monitoring

// Simple logger (same as in whispering_stone.cpp for demonstration)
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

/**
 * @class SurgeHandler
 * @brief Handles temporal surge events in the Chronovyan system
 */
class SurgeHandler {
public:
  /**
   * @brief Constructor
   * @param whisperingStone Reference to the WhisperingStoneSensor
   */
  SurgeHandler(sensors::WhisperingStoneSensor &whisperingStone)
      : m_whisperingStone(whisperingStone), m_systemMonitor(),
        m_userInterface() {
    Logger::info("SurgeHandler: Constructed");
  }

  /**
   * @brief Handle a temporal surge event
   * @param magnitude The magnitude of the surge on the Chronal Scale
   * @param duration The duration of the surge in seconds
   */
  void handleTemporalSurge(float magnitude, float duration) {
    Logger::info("SurgeHandler: Processing temporal surge with magnitude " +
                 std::to_string(magnitude) + " and duration " +
                 std::to_string(duration));

    // Simulate some processing time
    std::this_thread::sleep_for(
        std::chrono::milliseconds(static_cast<int>(duration * 100)));

    // Enhanced sensor recovery for high-magnitude surges
    if (magnitude > 3.7f) {
      Logger::warning("SurgeHandler: High-magnitude surge detected (" +
                      std::to_string(magnitude) +
                      "), performing enhanced recovery");

      // Track the surge magnitude for diagnostics
      m_whisperingStone.setLastSurgeMagnitude(magnitude);

      // Attempt recovery with timeout and retry
      int maxRetries = 3;
      bool resetSuccess = false;

      for (int attempt = 1; attempt <= maxRetries && !resetSuccess; attempt++) {
        Logger::info("SurgeHandler: Reset attempt " + std::to_string(attempt) +
                     " for WhisperingStoneSensor");

        // Perform reset with enhanced procedure
        resetSuccess = m_whisperingStone.resetAfterSurge();

        if (!resetSuccess && attempt < maxRetries) {
          // Wait briefly before retry
          Logger::warning("SurgeHandler: Reset attempt " +
                          std::to_string(attempt) +
                          " failed, waiting before retry");
          std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
      }

      if (!resetSuccess) {
        Logger::error(
            "SurgeHandler: Failed to reset WhisperingStoneSensor after " +
            std::to_string(maxRetries) +
            " attempts. Flagging for maintenance.");

        // Flag sensor for maintenance instead of requiring reboot
        m_systemMonitor.flagComponentForMaintenance("WhisperingStoneSensor");

        // Notify user of issue - Fixed string concatenation issue
        m_userInterface.displayWarning(
            std::string("Whispering Stone sensor requires maintenance. ") +
            std::string("Pressure readings may be unavailable."));
      } else {
        Logger::info("SurgeHandler: WhisperingStoneSensor successfully reset "
                     "after surge");
      }
    } else {
      // Normal reset for low-magnitude surges
      Logger::info(
          "SurgeHandler: Low-magnitude surge, performing standard reset");
      m_whisperingStone.resetAfterSurge();
    }

    Logger::info("SurgeHandler: Surge processing complete");
  }

private:
  sensors::WhisperingStoneSensor &m_whisperingStone;
  monitoring::SystemMonitor m_systemMonitor;
  ui::UserInterface m_userInterface;
};

// Test function to demonstrate the fix
void testSurgeHandler() {
  // Create a WhisperingStoneSensor instance
  auto whisperingStoneSensor =
      std::make_unique<sensors::WhisperingStoneSensorImpl>();

  // Initialize the sensor
  whisperingStoneSensor->initialize();

  // Create a SurgeHandler
  SurgeHandler surgeHandler(*whisperingStoneSensor);

  // Take a baseline reading before the surge
  sensors::PressureReading baselineReading =
      whisperingStoneSensor->getPressureReading();
  std::cout << "Baseline reading before surge: " << baselineReading.getValue()
            << std::endl;

  // Generate a low-magnitude surge (below threshold)
  std::cout << "\n--- Testing Low-Magnitude Surge (3.5) ---\n" << std::endl;
  surgeHandler.handleTemporalSurge(3.5f, 1.0f);

  // Take a reading after the low-magnitude surge
  sensors::PressureReading afterLowSurgeReading =
      whisperingStoneSensor->getPressureReading();
  std::cout << "Reading after low-magnitude surge: "
            << afterLowSurgeReading.getValue()
            << " (valid: " << (afterLowSurgeReading.isValid() ? "yes" : "no")
            << ")" << std::endl;

  // Generate a high-magnitude surge (above threshold)
  std::cout << "\n--- Testing High-Magnitude Surge (4.0) ---\n" << std::endl;
  surgeHandler.handleTemporalSurge(4.0f, 1.5f);

  // Take a reading after the high-magnitude surge
  sensors::PressureReading afterHighSurgeReading =
      whisperingStoneSensor->getPressureReading();
  std::cout << "Reading after high-magnitude surge: "
            << afterHighSurgeReading.getValue()
            << " (valid: " << (afterHighSurgeReading.isValid() ? "yes" : "no")
            << ")" << std::endl;

  // Test multiple rapid surges
  std::cout << "\n--- Testing Multiple Rapid Surges ---\n" << std::endl;
  for (int i = 0; i < 3; i++) {
    surgeHandler.handleTemporalSurge(4.0f + (i * 0.2f), 0.5f);
    sensors::PressureReading reading =
        whisperingStoneSensor->getPressureReading();
    std::cout << "Reading after surge " << (i + 1) << ": " << reading.getValue()
              << " (valid: " << (reading.isValid() ? "yes" : "no") << ")"
              << std::endl;
  }

  std::cout << "\nTest complete." << std::endl;
}

} // namespace chronometry
} // namespace chronovyan

// Main function for testing
int main() {
  std::cout << "=== Whispering Stone Sensor Test ===\n" << std::endl;
  chronovyan::chronometry::testSurgeHandler();
  return 0;
}