#include <algorithm>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

// Simplified resource tracking data structures
struct ResourceDataPoint {
    std::chrono::time_point<std::chrono::system_clock> timestamp;
    double chronon_usage;
    double aethel_usage;
    double temporal_debt;
    double paradox_risk;
};

class ResourceTracker {
public:
    ResourceTracker(size_t max_history = 100) : max_history_size_(max_history) {}

    void recordCurrentUsage(double chronon_usage, double aethel_usage, double temporal_debt = 0.0,
                            double paradox_risk = 0.0) {
        ResourceDataPoint point;
        point.timestamp = std::chrono::system_clock::now();
        point.chronon_usage = chronon_usage;
        point.aethel_usage = aethel_usage;
        point.temporal_debt = temporal_debt;
        point.paradox_risk = paradox_risk;

        history_.push_back(point);

        // Limit history size
        if (history_.size() > max_history_size_) {
            history_.erase(history_.begin());
        }
    }

    const std::vector<ResourceDataPoint>& getHistoricalData() const { return history_; }

private:
    std::vector<ResourceDataPoint> history_;
    size_t max_history_size_;
};

// ASCII chart generator for resource visualization
class AsciiChartGenerator {
public:
    AsciiChartGenerator(const ResourceTracker& tracker) : tracker_(tracker) {}

    // Generate a line chart for a specific resource type
    std::string generateLineChart(int width, int height, bool showChronon, bool showAethel) const {
        const auto& data = tracker_.getHistoricalData();
        if (data.empty()) {
            return "No data available for visualization.";
        }

        // Extract values
        std::vector<double> chronon_values;
        std::vector<double> aethel_values;

        for (const auto& point : data) {
            chronon_values.push_back(point.chronon_usage);
            aethel_values.push_back(point.aethel_usage);
        }

        // Find max values for scaling
        double max_chronon = *std::max_element(chronon_values.begin(), chronon_values.end());
        double max_aethel = *std::max_element(aethel_values.begin(), aethel_values.end());

        // Create a grid for drawing
        std::vector<std::string> grid(height, std::string(width, ' '));

        // Draw chronon line if requested
        if (showChronon) {
            drawLine(grid, chronon_values, max_chronon, height, 'C');
        }

        // Draw aethel line if requested
        if (showAethel) {
            drawLine(grid, aethel_values, max_aethel, height, 'A');
        }

        // Render the grid with frame
        std::stringstream ss;

        // Add title
        ss << "+--" << std::string(width, '-') << "--+\n";
        ss << "| Chronovyan Resource Visualization " << std::string(width - 32, ' ') << "|\n";
        ss << "+--" << std::string(width, '-') << "--+\n";

        // Add the chart grid
        for (const auto& row : grid) {
            ss << "| " << row << " |\n";
        }

        // Add bottom frame
        ss << "+--" << std::string(width, '-') << "--+\n";

        // Add legend
        ss << "Legend: C = Chronon, A = Aethel, * = Both\n";

        return ss.str();
    }

    // Generate a bar chart for resource usage
    std::string generateBarChart(int width, int height) const {
        const auto& data = tracker_.getHistoricalData();
        if (data.empty()) {
            return "No data available for visualization.";
        }

        // Extract last few data points
        size_t num_bars = std::min(size_t(10), data.size());
        std::vector<double> chronon_values;
        std::vector<double> aethel_values;

        for (size_t i = data.size() - num_bars; i < data.size(); ++i) {
            chronon_values.push_back(data[i].chronon_usage);
            aethel_values.push_back(data[i].aethel_usage);
        }

        // Find max for scaling
        double max_value = std::max(*std::max_element(chronon_values.begin(), chronon_values.end()),
                                    *std::max_element(aethel_values.begin(), aethel_values.end()));

        // Create the bar chart
        std::stringstream ss;

        // Add title
        ss << "+--" << std::string(width, '-') << "--+\n";
        ss << "| Resource Usage Bar Chart " << std::string(width - 25, ' ') << "|\n";
        ss << "+--" << std::string(width, '-') << "--+\n";

        // Draw bars from top to bottom
        for (int row = height - 1; row >= 0; --row) {
            ss << "| ";

            // Calculate threshold for this row
            double threshold = (double)row / (height - 1) * max_value;

            // Draw bars for each data point
            for (size_t i = 0; i < chronon_values.size(); ++i) {
                // Draw chronon bar
                if (chronon_values[i] > threshold) {
                    ss << "C ";
                } else {
                    ss << "  ";
                }

                // Draw aethel bar
                if (aethel_values[i] > threshold) {
                    ss << "A ";
                } else {
                    ss << "  ";
                }

                // Add space between bar groups
                ss << " ";
            }

            // Fill remaining space
            size_t chars_used = 2 + chronon_values.size() * 5;
            if (chars_used < width + 2) {
                ss << std::string(width + 2 - chars_used, ' ');
            }

            ss << "|\n";
        }

        // Add x-axis
        ss << "| " << std::string(width, '-') << " |\n";

        // Add x-axis labels
        ss << "| ";
        for (size_t i = 0; i < num_bars; ++i) {
            ss << std::setw(4) << (i + 1) << " ";
        }

        // Fill remaining space
        size_t chars_used = 2 + num_bars * 5;
        if (chars_used < width + 2) {
            ss << std::string(width + 2 - chars_used, ' ');
        }
        ss << "|\n";

        // Add bottom frame
        ss << "+--" << std::string(width, '-') << "--+\n";

        // Add legend
        ss << "Legend: C = Chronon, A = Aethel\n";

        return ss.str();
    }

    // Generate a combined resource chart with all metrics
    std::string generateCombinedChart(int width, int height) const {
        // Simply use the line chart with both resources
        return generateLineChart(width, height, true, true);
    }

private:
    const ResourceTracker& tracker_;

    // Helper method to draw a line on the grid
    void drawLine(std::vector<std::string>& grid, const std::vector<double>& values,
                  double max_value, int height, char symbol) const {
        if (values.empty() || max_value == 0) {
            return;
        }

        const int width = grid[0].size();

        // Scale points to fit in grid
        for (size_t i = 0; i < std::min(values.size(), size_t(width)); ++i) {
            // Calculate y position (0 = top of grid)
            int y = height - 1 - (int)((values[i] / max_value) * (height - 1));
            if (y >= 0 && y < height) {
                // If another symbol exists at this position, use * for overlap
                if (grid[y][i] != ' ') {
                    grid[y][i] = '*';
                } else {
                    grid[y][i] = symbol;
                }

                // Draw connecting lines if not the first point
                if (i > 0) {
                    int prev_y = height - 1 - (int)((values[i - 1] / max_value) * (height - 1));
                    if (prev_y >= 0 && prev_y < height) {
                        // Connect points with vertical line
                        int start_y = std::min(y, prev_y);
                        int end_y = std::max(y, prev_y);

                        for (int line_y = start_y + 1; line_y < end_y; ++line_y) {
                            if (grid[line_y][i] == ' ') {
                                grid[line_y][i] = '|';
                            }
                        }
                    }
                }
            }
        }
    }
};

// Helper function to generate sample data
void generateSampleData(ResourceTracker& tracker, int count) {
    for (int i = 0; i < count; ++i) {
        // Generate sine waves for demo data
        double chronon_usage = 50.0 + 40.0 * std::sin(i * 0.2);
        double aethel_usage = 30.0 + 25.0 * std::cos(i * 0.3);
        double temporal_debt = 5.0 + 5.0 * std::sin(i * 0.1);
        double paradox_risk = 0.1 + 0.4 * std::abs(std::sin(i * 0.4));

        tracker.recordCurrentUsage(chronon_usage, aethel_usage, temporal_debt, paradox_risk);

        // Small delay to simulate real-time data collection
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

int main() {
    std::cout << "Chronovyan Resource Visualization Demo" << std::endl;
    std::cout << "=====================================" << std::endl << std::endl;

    // Create a resource tracker
    ResourceTracker tracker;

    // Generate sample data
    std::cout << "Generating sample resource data..." << std::endl;
    generateSampleData(tracker, 50);
    std::cout << "Sample data generated." << std::endl << std::endl;

    // Create chart generator
    AsciiChartGenerator chartGen(tracker);

    // Generate and display different chart types
    std::cout << "CHRONON LINE CHART:" << std::endl;
    std::cout << chartGen.generateLineChart(60, 15, true, false) << std::endl;

    std::cout << "AETHEL LINE CHART:" << std::endl;
    std::cout << chartGen.generateLineChart(60, 15, false, true) << std::endl;

    std::cout << "COMBINED RESOURCE CHART:" << std::endl;
    std::cout << chartGen.generateCombinedChart(60, 20) << std::endl;

    std::cout << "RESOURCE BAR CHART:" << std::endl;
    std::cout << chartGen.generateBarChart(60, 15) << std::endl;

    std::cout << "Demo completed successfully." << std::endl;

    return 0;
}