#include <chrono>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <thread>
#include <vector>

#include "../include/resource_optimizer.h"
#include "../include/resource_visualization.h"
#include "../include/temporal_debt_tracker.h"
#include "../include/temporal_runtime.h"

using namespace chronovyan;

// Helper function to generate random resource data
std::vector<double> generateRandomData(size_t count, double min, double max) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(min, max);

    std::vector<double> data;
    for (size_t i = 0; i < count; ++i) {
        data.push_back(dis(gen));
    }
    return data;
}

// Demonstrates different themes with the same visualization
void demonstrateThemes(ResourceVisualization& visualizer) {
    std::cout << "\n====== THEME DEMONSTRATION ======\n" << std::endl;

    // Generate some random data for visualization
    std::vector<double> data = generateRandomData(20, 0.1, 0.9);

    // Demonstrate each theme type
    for (int theme = static_cast<int>(ThemeType::DEFAULT);
         theme <= static_cast<int>(ThemeType::CUSTOM); ++theme) {
        ThemeType currentTheme = static_cast<ThemeType>(theme);
        visualizer.setThemeType(currentTheme);

        // Display the current theme name
        std::cout << "\n===== " <<
            [currentTheme]() {
                switch (currentTheme) {
                    case ThemeType::DEFAULT:
                        return "DEFAULT THEME";
                    case ThemeType::DARK:
                        return "DARK THEME";
                    case ThemeType::LIGHT:
                        return "LIGHT THEME";
                    case ThemeType::MINIMAL:
                        return "MINIMAL THEME";
                    case ThemeType::ELABORATE:
                        return "ELABORATE THEME";
                    case ThemeType::CUSTOM:
                        return "CUSTOM THEME";
                    default:
                        return "UNKNOWN THEME";
                }
            }() << " =====\n"
                  << std::endl;

        // 1. Display ASCII Chart
        std::cout << visualizer.generateASCIIChart(data, 40, 10, "Resource Usage Over Time",
                                                   "Time Periods", "Usage")
                  << std::endl;

        // 2. Display ASCII Gauges with different values
        std::cout << visualizer.generateASCIIGauge(0.25, 30, "Low Value Gauge") << std::endl;
        std::cout << visualizer.generateASCIIGauge(0.55, 30, "Medium Value Gauge") << std::endl;
        std::cout << visualizer.generateASCIIGauge(0.85, 30, "High Value Gauge") << std::endl;

        // 3. Display themed bars
        std::cout << "\nThemed Bars (Various Values):" << std::endl;
        std::cout << "  10%: " << visualizer.generateThemedBar(0.1, 40) << std::endl;
        std::cout << "  50%: " << visualizer.generateThemedBar(0.5, 40) << std::endl;
        std::cout << "  90%: " << visualizer.generateThemedBar(0.9, 40) << std::endl;

        // Add a separator between themes
        std::cout << "\n--------------------------------\n" << std::endl;

        // Small pause to let the user see the changes
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

// Create a custom theme and demonstrate it
void demonstrateCustomTheme(ResourceVisualization& visualizer) {
    std::cout << "\n====== CUSTOM THEME CREATION ======\n" << std::endl;

    // Create a custom theme
    VisualizationTheme customTheme;

    // Set custom colors
    customTheme.positive_color = "\033[38;5;120m";  // Light green
    customTheme.negative_color = "\033[38;5;203m";  // Light red
    customTheme.warning_color = "\033[38;5;220m";   // Yellow
    customTheme.critical_color = "\033[38;5;196m";  // Bright red
    customTheme.header_color = "\033[38;5;51m";     // Cyan
    customTheme.title_color = "\033[38;5;213m";     // Pink
    customTheme.text_color = "\033[38;5;252m";      // Light gray
    customTheme.text_muted = "\033[38;5;240m";      // Dark gray

    // Set custom ASCII characters
    customTheme.ascii_chars.filled_gauge = '#';  // Use hash instead of Full block
    customTheme.ascii_chars.empty_gauge = '.';   // Use dot instead of Light shade
    customTheme.ascii_chars.x_axis = '-';        // Use dash instead of Horizontal line
    customTheme.ascii_chars.y_axis = '|';        // Use pipe instead of Vertical line
    customTheme.ascii_chars.data_point = 'O';    // Use capital O instead of Circle

    // Enable Unicode and bold features
    customTheme.use_unicode = true;
    customTheme.use_bold = true;

    // Apply the custom theme
    visualizer.setVisualizationTheme(customTheme);

    // Generate sample data
    std::vector<double> data = generateRandomData(20, 0.1, 0.9);

    // Display the theme name
    std::cout << "\n===== CUSTOM VIBRANT THEME =====\n" << std::endl;

    // Display visualizations with the custom theme
    std::cout << visualizer.generateASCIIChart(data, 40, 10, "Resource Usage (Custom Theme)",
                                               "Time Periods", "Usage")
              << std::endl;

    std::cout << visualizer.generateASCIIGauge(0.35, 30, "Custom Low Gauge") << std::endl;
    std::cout << visualizer.generateASCIIGauge(0.65, 30, "Custom Medium Gauge") << std::endl;
    std::cout << visualizer.generateASCIIGauge(0.95, 30, "Custom High Gauge") << std::endl;
}

int main() {
    std::cout << "=== Chronovyan Resource Visualization Theme Demo ===\n" << std::endl;

    // Create the runtime components
    auto runtime = std::make_shared<TemporalRuntime>();
    auto optimizer = std::make_shared<ResourceOptimizer>(runtime);
    auto debt_tracker = std::make_shared<TemporalDebtTracker>(runtime);

    // Create the visualization object
    ResourceVisualization visualizer(runtime, optimizer, debt_tracker);

    // Demonstrate standard themes
    demonstrateThemes(visualizer);

    // Demonstrate custom theme creation
    demonstrateCustomTheme(visualizer);

    std::cout << "\n=== Theme Demo Completed ===\n" << std::endl;

    return 0;
}