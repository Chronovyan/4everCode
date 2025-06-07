#include <algorithm>
#include <cassert>
#include <cmath>
#include <fstream>
#include <functional>
#include <iomanip>
#include <numeric>
#include <sstream>

#include "advanced_resource_visualizer.h"

namespace chronovyan {

AdvancedResourceVisualizer::AdvancedResourceVisualizer(const ResourceTracker& tracker)
    : tracker_(tracker) {}

std::string AdvancedResourceVisualizer::generateResourceVisualization(
    VisualizationOutputFormat format, ChartType chart_type,
    const ChartConfiguration& config) const {
    const auto& data = tracker_.getHistoricalData();
    if (data.empty()) {
        return "No data available for visualization.";
    }

    // Extract resource usage values
    std::vector<double> chronon_values;
    std::vector<double> aethel_values;
    std::vector<double> temporal_debt_values;
    std::vector<double> paradox_risk_values;

    for (const auto& point : data) {
        chronon_values.push_back(point.chronon_usage);
        aethel_values.push_back(point.aethel_usage);
        temporal_debt_values.push_back(point.temporal_debt);
        paradox_risk_values.push_back(point.paradox_risk);
    }

    // Create a combined visualization
    std::stringstream ss;

    // Add title
    std::string title = config.title.empty() ? "Chronovyan Resource Visualization" : config.title;

    switch (format) {
        case VisualizationOutputFormat::ASCII: {
            // Create an ASCII visualization
            ss << "═══════════════════════════════════════════════════════════\n";
            ss << "║ " << title << "\n";
            ss << "═══════════════════════════════════════════════════════════\n\n";

            // Generate appropriate chart based on type
            switch (chart_type) {
                case ChartType::LINE:
                    ss << "Chronon Usage (Line Chart):\n";
                    ss << generateAsciiLineChart(chronon_values, config) << "\n\n";
                    ss << "Aethel Usage (Line Chart):\n";
                    ss << generateAsciiLineChart(aethel_values, config) << "\n\n";
                    break;

                case ChartType::BAR:
                    ss << "Chronon Usage (Bar Chart):\n";
                    ss << generateAsciiBarChart(chronon_values, config) << "\n\n";
                    ss << "Aethel Usage (Bar Chart):\n";
                    ss << generateAsciiBarChart(aethel_values, config) << "\n\n";
                    break;

                case ChartType::HEATMAP:
                    ss << "Resource Intensity (Heatmap):\n";
                    ss << generateAsciiHeatmap(chronon_values, config) << "\n\n";
                    break;

                case ChartType::SCATTER: {
                    ss << "Chronon vs Aethel Usage (Scatter Plot):\n";
                    // Use only matching pairs of data points
                    size_t min_size = std::min(chronon_values.size(), aethel_values.size());
                    std::vector<double> x_values(chronon_values.begin(),
                                                 chronon_values.begin() + min_size);
                    std::vector<double> y_values(aethel_values.begin(),
                                                 aethel_values.begin() + min_size);
                    ss << generateAsciiScatterPlot(x_values, y_values, config) << "\n\n";
                    break;
                }

                case ChartType::COMBINED:
                    ss << "Combined Resource Visualization:\n";
                    ss << generateCombinedResourceVisualization(format, config);
                    break;

                default:
                    ss << "Unsupported chart type for ASCII output.";
                    break;
            }
            break;
        }

        case VisualizationOutputFormat::SVG: {
            ss << generateSvgChart(chronon_values, chart_type, config);
            break;
        }

        case VisualizationOutputFormat::HTML: {
            ss << generateHtmlChart(chronon_values, chart_type, config);
            break;
        }

        case VisualizationOutputFormat::JSON: {
            ss << "{\n";
            ss << "  \"title\": \"" << title << "\",\n";
            ss << "  \"chronon_data\": " << generateJsonData(chronon_values, config) << ",\n";
            ss << "  \"aethel_data\": " << generateJsonData(aethel_values, config) << ",\n";
            ss << "  \"temporal_debt_data\": " << generateJsonData(temporal_debt_values, config)
               << ",\n";
            ss << "  \"paradox_risk_data\": " << generateJsonData(paradox_risk_values, config)
               << "\n";
            ss << "}\n";
            break;
        }

        default:
            ss << "Unsupported visualization format.";
            break;
    }

    return ss.str();
}

std::string AdvancedResourceVisualizer::generateChronoVisualization(
    VisualizationOutputFormat format, ChartType chart_type,
    const ChartConfiguration& config) const {
    const auto& data = tracker_.getHistoricalData();
    if (data.empty()) {
        return "No Chronon data available for visualization.";
    }

    // Extract Chronon usage values
    std::vector<double> chronon_values;
    for (const auto& point : data) {
        chronon_values.push_back(point.chronon_usage);
    }

    // Create a custom configuration with Chronon-specific settings
    ChartConfiguration chronon_config = config;
    if (chronon_config.title.empty()) {
        chronon_config.title = "Chronon Usage Visualization";
    }

    // Generate chart based on format and type
    switch (format) {
        case VisualizationOutputFormat::ASCII: {
            std::stringstream ss;
            ss << "═══════════════════════════════════════════════════════════\n";
            ss << "║ " << chronon_config.title << "\n";
            ss << "═══════════════════════════════════════════════════════════\n\n";

            switch (chart_type) {
                case ChartType::LINE:
                    ss << generateAsciiLineChart(chronon_values, chronon_config);
                    break;
                case ChartType::BAR:
                    ss << generateAsciiBarChart(chronon_values, chronon_config);
                    break;
                case ChartType::HEATMAP:
                    ss << generateAsciiHeatmap(chronon_values, chronon_config);
                    break;
                default:
                    ss << "Unsupported chart type for Chronon visualization.";
                    break;
            }
            return ss.str();
        }

        case VisualizationOutputFormat::SVG:
            return generateSvgChart(chronon_values, chart_type, chronon_config);

        case VisualizationOutputFormat::HTML:
            return generateHtmlChart(chronon_values, chart_type, chronon_config);

        case VisualizationOutputFormat::JSON:
            return generateJsonData(chronon_values, chronon_config);

        default:
            return "Unsupported visualization format for Chronon visualization.";
    }
}

std::string AdvancedResourceVisualizer::generateAethelVisualization(
    VisualizationOutputFormat format, ChartType chart_type,
    const ChartConfiguration& config) const {
    const auto& data = tracker_.getHistoricalData();
    if (data.empty()) {
        return "No Aethel data available for visualization.";
    }

    // Extract Aethel usage values
    std::vector<double> aethel_values;
    for (const auto& point : data) {
        aethel_values.push_back(point.aethel_usage);
    }

    // Create a custom configuration with Aethel-specific settings
    ChartConfiguration aethel_config = config;
    if (aethel_config.title.empty()) {
        aethel_config.title = "Aethel Usage Visualization";
    }

    // Generate chart based on format and type
    switch (format) {
        case VisualizationOutputFormat::ASCII: {
            std::stringstream ss;
            ss << "═══════════════════════════════════════════════════════════\n";
            ss << "║ " << aethel_config.title << "\n";
            ss << "═══════════════════════════════════════════════════════════\n\n";

            switch (chart_type) {
                case ChartType::LINE:
                    ss << generateAsciiLineChart(aethel_values, aethel_config);
                    break;
                case ChartType::BAR:
                    ss << generateAsciiBarChart(aethel_values, aethel_config);
                    break;
                case ChartType::HEATMAP:
                    ss << generateAsciiHeatmap(aethel_values, aethel_config);
                    break;
                default:
                    ss << "Unsupported chart type for Aethel visualization.";
                    break;
            }
            return ss.str();
        }

        case VisualizationOutputFormat::SVG:
            return generateSvgChart(aethel_values, chart_type, aethel_config);

        case VisualizationOutputFormat::HTML:
            return generateHtmlChart(aethel_values, chart_type, aethel_config);

        case VisualizationOutputFormat::JSON:
            return generateJsonData(aethel_values, aethel_config);

        default:
            return "Unsupported visualization format for Aethel visualization.";
    }
}

std::string AdvancedResourceVisualizer::generateCombinedResourceVisualization(
    VisualizationOutputFormat format, const ChartConfiguration& config) const {
    const auto& data = tracker_.getHistoricalData();
    if (data.empty()) {
        return "No data available for combined visualization.";
    }

    // Extract all resource types
    std::vector<double> chronon_values;
    std::vector<double> aethel_values;
    std::vector<double> temporal_debt_values;
    std::vector<double> paradox_risk_values;

    for (const auto& point : data) {
        chronon_values.push_back(point.chronon_usage);
        aethel_values.push_back(point.aethel_usage);
        temporal_debt_values.push_back(point.temporal_debt);
        paradox_risk_values.push_back(point.paradox_risk);
    }

    // Custom configuration for the combined view
    ChartConfiguration combined_config = config;
    if (combined_config.title.empty()) {
        combined_config.title = "Combined Resource Metrics";
    }

    switch (format) {
        case VisualizationOutputFormat::ASCII: {
            std::stringstream ss;

            // Create a multi-line chart with all metrics
            const size_t width = combined_config.width;
            const size_t height = combined_config.height;

            // Find max values for scaling
            double max_chronon = *std::max_element(chronon_values.begin(), chronon_values.end());
            double max_aethel = *std::max_element(aethel_values.begin(), aethel_values.end());
            double max_debt =
                *std::max_element(temporal_debt_values.begin(), temporal_debt_values.end());
            double max_risk =
                *std::max_element(paradox_risk_values.begin(), paradox_risk_values.end());

            // Create normalized values for display
            std::vector<size_t> norm_chronon = normalizeValues(chronon_values, height);
            std::vector<size_t> norm_aethel = normalizeValues(aethel_values, height);
            std::vector<size_t> norm_debt = normalizeValues(temporal_debt_values, height);
            std::vector<size_t> norm_risk = normalizeValues(paradox_risk_values, height);

            // Chart title and header
            ss << "╔" << std::string(width - 2, '=') << "╗\n";
            ss << "║ " << combined_config.title
               << std::string(width - combined_config.title.length() - 4, ' ') << "║\n";
            ss << "╠" << std::string(width - 2, '=') << "╣\n";

            // Draw the chart grid
            std::vector<std::string> grid(height);
            for (size_t row = 0; row < height; ++row) {
                std::string line = "║ ";
                const std::string reset_for_padding = combined_config.reset_color;

                // Add data points for current row
                size_t y_pos = height - row - 1;

                for (size_t x = 0; x < std::min(width - 4, norm_chronon.size()); ++x) {
                    char cell = ' ';
                    std::string color = "";

                    // Determine which points to plot for this cell
                    bool has_chronon = (x < norm_chronon.size() && norm_chronon[x] == y_pos);
                    bool has_aethel = (x < norm_aethel.size() && norm_aethel[x] == y_pos);
                    bool has_debt = (x < norm_debt.size() && norm_debt[x] == y_pos);
                    bool has_risk = (x < norm_risk.size() && norm_risk[x] == y_pos);

                    // Color code the different metrics
                    if (has_chronon) {
                        cell = 'C';
                        color = combined_config.colors[0];  // Red for Chronon
                    }
                    if (has_aethel) {
                        cell = 'A';
                        color = combined_config.colors[1];  // Green for Aethel
                    }
                    if (has_debt) {
                        cell = 'D';
                        color = combined_config.colors[2];  // Yellow for Debt
                    }
                    if (has_risk) {
                        cell = 'R';
                        color = combined_config.colors[3];  // Blue for Risk
                    }

                    // If multiple metrics in same position, use a special character
                    int overlap_count = has_chronon + has_aethel + has_debt + has_risk;
                    if (overlap_count > 1) {
                        cell = '*';
                        color = combined_config.colors[4];  // Magenta for overlap
                    }

                    line += color + cell + reset_for_padding;
                }

                // Pad to full width
                line +=
                    std::string(width - line.length() + reset_for_padding.length() - 1, ' ') + "║";
                grid[row] = line;
            }

            // Add the grid to the output
            for (const auto& line : grid) {
                ss << line << "\n";
            }

            // Add x-axis
            ss << "╚" << std::string(width - 2, '=') << "╝\n";

            // Add legend
            ss << "\nLegend:\n";
            ss << combined_config.colors[0] << "C" << combined_config.reset_color
               << " - Chronon Usage\n";
            ss << combined_config.colors[1] << "A" << combined_config.reset_color
               << " - Aethel Usage\n";
            ss << combined_config.colors[2] << "D" << combined_config.reset_color
               << " - Temporal Debt\n";
            ss << combined_config.colors[3] << "R" << combined_config.reset_color
               << " - Paradox Risk\n";
            ss << combined_config.colors[4] << "*" << combined_config.reset_color
               << " - Multiple Metrics\n";

            return ss.str();
        }

        case VisualizationOutputFormat::JSON: {
            std::stringstream ss;
            ss << "{\n";
            ss << "  \"title\": \"" << combined_config.title << "\",\n";
            ss << "  \"data\": {\n";
            ss << "    \"chronon\": " << generateJsonData(chronon_values, combined_config) << ",\n";
            ss << "    \"aethel\": " << generateJsonData(aethel_values, combined_config) << ",\n";
            ss << "    \"temporal_debt\": "
               << generateJsonData(temporal_debt_values, combined_config) << ",\n";
            ss << "    \"paradox_risk\": " << generateJsonData(paradox_risk_values, combined_config)
               << "\n";
            ss << "  }\n";
            ss << "}\n";
            return ss.str();
        }

        // For other formats, we'd implement more complex visualization
        // But for now we'll just return a placeholder
        default:
            return "Combined visualization not implemented for the selected format.";
    }
}

std::string AdvancedResourceVisualizer::generateParadoxRiskHeatmap(
    VisualizationOutputFormat format, const ChartConfiguration& config) const {
    const auto& data = tracker_.getHistoricalData();
    if (data.empty()) {
        return "No paradox risk data available for visualization.";
    }

    // Extract paradox risk values
    std::vector<double> risk_values;
    for (const auto& point : data) {
        risk_values.push_back(point.paradox_risk);
    }

    // Custom configuration for the paradox risk heatmap
    ChartConfiguration risk_config = config;
    if (risk_config.title.empty()) {
        risk_config.title = "Paradox Risk Heatmap";
    }

    switch (format) {
        case VisualizationOutputFormat::ASCII:
            return generateAsciiHeatmap(risk_values, risk_config);

        // For other formats, similar approach as in previous methods
        default:
            return "Paradox risk heatmap not implemented for the selected format.";
    }
}

bool AdvancedResourceVisualizer::exportVisualization(const std::string& visualization,
                                                     const std::string& filename,
                                                     VisualizationOutputFormat format) const {
    // Create an output file stream
    std::ofstream outfile(filename);
    if (!outfile.is_open()) {
        return false;
    }

    // Write the visualization to the file
    outfile << visualization;

    // Check if the write was successful
    if (outfile.bad()) {
        outfile.close();
        return false;
    }

    outfile.close();
    return true;
}

// Helper methods for chart generation

std::string AdvancedResourceVisualizer::generateAsciiChart(const std::vector<double>& values,
                                                           ChartType chart_type,
                                                           const ChartConfiguration& config) const {
    switch (chart_type) {
        case ChartType::LINE:
            return generateAsciiLineChart(values, config);
        case ChartType::BAR:
            return generateAsciiBarChart(values, config);
        case ChartType::HEATMAP:
            return generateAsciiHeatmap(values, config);
        case ChartType::SCATTER:
            // Scatter plot needs x and y values - can't generate from single vector
            return "Scatter plot requires both x and y values.";
        default:
            return "Unsupported chart type for ASCII output.";
    }
}

std::string AdvancedResourceVisualizer::generateSvgChart(const std::vector<double>& values,
                                                         ChartType chart_type,
                                                         const ChartConfiguration& config) const {
    // SVG generation requires more complex code
    // For now, return a placeholder SVG template
    std::stringstream ss;
    ss << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n";
    ss << "<svg width=\"" << config.width * 10 << "\" height=\"" << config.height * 10
       << "\" xmlns=\"http://www.w3.org/2000/svg\">\n";

    ss << "  <rect width=\"100%\" height=\"100%\" fill=\"white\"/>\n";
    ss << "  <text x=\"50%\" y=\"20\" text-anchor=\"middle\" font-family=\"Arial\" "
          "font-size=\"16\">"
       << config.title << "</text>\n";

    // Placeholder message
    ss << "  <text x=\"50%\" y=\"50%\" text-anchor=\"middle\" font-family=\"Arial\" "
          "font-size=\"14\">"
       << "SVG chart generation not fully implemented yet" << "</text>\n";

    ss << "</svg>\n";

    return ss.str();
}

std::string AdvancedResourceVisualizer::generateHtmlChart(const std::vector<double>& values,
                                                          ChartType chart_type,
                                                          const ChartConfiguration& config) const {
    // Create a simple HTML template with embedded data
    std::stringstream ss;
    ss << "<!DOCTYPE html>\n";
    ss << "<html>\n";
    ss << "<head>\n";
    ss << "  <title>" << config.title << "</title>\n";
    ss << "  <style>\n";
    ss << "    body { font-family: Arial, sans-serif; margin: 20px; }\n";
    ss << "    .chart-container { width: 80%; height: 400px; margin: 20px auto; }\n";
    ss << "    h1 { text-align: center; }\n";
    ss << "  </style>\n";
    ss << "</head>\n";
    ss << "<body>\n";
    ss << "  <h1>" << config.title << "</h1>\n";
    ss << "  <div class=\"chart-container\">\n";
    ss << "    <p>HTML chart visualization requires JavaScript libraries.<br>\n";
    ss << "       This is a placeholder for future implementation.</p>\n";
    ss << "    <pre>" << generateAsciiLineChart(values, config) << "</pre>\n";
    ss << "  </div>\n";
    ss << "</body>\n";
    ss << "</html>\n";

    return ss.str();
}

std::string AdvancedResourceVisualizer::generateJsonData(const std::vector<double>& values,
                                                         const ChartConfiguration& config) const {
    std::stringstream ss;
    ss << "[";

    for (size_t i = 0; i < values.size(); ++i) {
        ss << values[i];
        if (i < values.size() - 1) {
            ss << ", ";
        }
    }

    ss << "]";

    return ss.str();
}

std::vector<size_t> AdvancedResourceVisualizer::normalizeValues(const std::vector<double>& values,
                                                                size_t height) const {
    if (values.empty()) {
        return {};
    }

    // Find the maximum value for scaling
    double max_value = *std::max_element(values.begin(), values.end());

    // If all values are 0, return a flat line
    if (max_value == 0) {
        return std::vector<size_t>(values.size(), 0);
    }

    // Scale values to fit within the height range
    std::vector<size_t> normalized(values.size());
    for (size_t i = 0; i < values.size(); ++i) {
        normalized[i] = static_cast<size_t>((values[i] / max_value) * (height - 1));
    }

    return normalized;
}

std::string AdvancedResourceVisualizer::getColorForValue(double value, double max_value) const {
    // Choose color based on value intensity (for heatmaps and other visualizations)
    if (max_value == 0) {
        return "\033[0m";  // Reset color if no data
    }

    double ratio = value / max_value;

    if (ratio < 0.2) {
        return "\033[32m";  // Green for low values
    } else if (ratio < 0.5) {
        return "\033[33m";  // Yellow for medium values
    } else if (ratio < 0.8) {
        return "\033[35m";  // Magenta for high values
    } else {
        return "\033[31m";  // Red for very high values
    }
}

std::string AdvancedResourceVisualizer::generateAsciiLineChart(
    const std::vector<double>& values, const ChartConfiguration& config) const {
    if (values.empty()) {
        return "[No data]";
    }

    const size_t width = config.width;
    const size_t height = config.height;

    // Normalize values to fit within the height
    auto normalized = normalizeValues(values, height);

    // Prepare the grid for the chart
    std::vector<std::string> grid(height);
    for (size_t y = 0; y < height; ++y) {
        grid[y] = std::string(width, ' ');
    }

    // Generate the line chart
    for (size_t x = 0; x < std::min(width, normalized.size()); ++x) {
        // Draw the data point
        size_t y = height - normalized[x] - 1;

        // Choose color based on value
        std::string color =
            getColorForValue(values[x], *std::max_element(values.begin(), values.end()));

        // Replace character with a colored data point
        if (y < height) {
            grid[y][x] = '*';

            // Connect points with lines if not the first point
            if (x > 0 && x < normalized.size()) {
                size_t prev_y = height - normalized[x - 1] - 1;
                if (prev_y < height) {
                    // Draw connecting line
                    size_t start_y = std::min(y, prev_y);
                    size_t end_y = std::max(y, prev_y);

                    for (size_t line_y = start_y + 1; line_y < end_y; ++line_y) {
                        grid[line_y][x] = '|';
                    }
                }
            }
        }
    }

    // Build the chart string with frame
    std::stringstream ss;

    // Add y-axis labels and chart frame
    for (size_t y = 0; y < height; ++y) {
        ss << "│ ";

        // Add colored data points
        for (size_t x = 0; x < width; ++x) {
            char point = grid[y][x];
            if (point != ' ') {
                std::string color =
                    getColorForValue(values[std::min(x, values.size() - 1)],
                                     *std::max_element(values.begin(), values.end()));
                ss << color << point << config.reset_color;
            } else {
                ss << ' ';
            }
        }
        ss << " │\n";
    }

    // Add x-axis
    ss << "└─" << std::string(width, '-') << "─┘\n";

    // Add x-axis labels if space permits
    if (width > 10) {
        ss << "  ";
        size_t label_interval = std::max(size_t(1), values.size() / 5);
        for (size_t i = 0; i < std::min(width, values.size()); i += label_interval) {
            ss << std::setw(label_interval) << i;
        }
        ss << "\n";
    }

    return ss.str();
}

std::string AdvancedResourceVisualizer::generateAsciiBarChart(
    const std::vector<double>& values, const ChartConfiguration& config) const {
    std::stringstream ss;
    if (values.empty()) {
        return "No data to display for bar chart.";
    }

    size_t width = static_cast<size_t>(config.width);
    size_t height = static_cast<size_t>(config.height);
    std::vector<size_t> normalized_values =
        normalizeValues(values, height - 1);  // -1 for base line

    ss << config.title << "\n";
    // Top border of the chart box. Original char was 'ΓöÇ' or similar.
    ss << "╔" << std::string(width, '=') << "╗\n";

    for (size_t i = 0; i < height; ++i) {
        ss << "║";
        for (size_t j = 0; j < width; ++j) {
            if (normalized_values[j] >= i) {
                ss << "█";
            } else {
                ss << " ";
            }
        }
        ss << "║\n";
    }
    // Bottom border of the chart box. Original char was 'ΓöÇ' or similar.
    ss << "╚" << std::string(width - 2, '=') << "╝\n";
    return ss.str();
}

std::string AdvancedResourceVisualizer::generateAsciiHeatmap(
    const std::vector<double>& values, const ChartConfiguration& config) const {
    if (values.empty()) {
        return "[No data]";
    }

    // Find min and max for scaling
    double min_value = *std::min_element(values.begin(), values.end());
    double max_value = *std::max_element(values.begin(), values.end());

    // Ensure range is non-zero
    if (max_value == min_value) {
        max_value = min_value + 1.0;
    }

    // Calculate heatmap dimensions
    size_t width = std::min(config.width, values.size());
    size_t height = std::min(config.height, (values.size() + width - 1) / width);

    std::stringstream ss;

    // Draw the heatmap
    for (size_t y = 0; y < height; ++y) {
        ss << "│ ";

        for (size_t x = 0; x < width; ++x) {
            size_t index = y * width + x;
            if (index < values.size()) {
                // Calculate intensity (0.0 to 1.0)
                double intensity = (values[index] - min_value) / (max_value - min_value);

                // Choose color based on intensity
                std::string color = getColorForValue(values[index], max_value);

                // Choose character based on intensity
                char symbol;
                if (intensity < 0.25) {
                    symbol = '.';
                } else if (intensity < 0.5) {
                    symbol = '+';
                } else if (intensity < 0.75) {
                    symbol = '#';
                } else {
                    symbol = '#';
                }

                ss << color << symbol << config.reset_color;
            } else {
                ss << " ";
            }
        }

        ss << " │\n";
    }

    // Add frame bottom
    ss << "└─" << std::string(width, '-') << "─┘\n";

    // Add legend
    ss << "\nLegend: ";
    ss << getColorForValue(min_value, max_value) << "." << config.reset_color << " Low  ";
    ss << getColorForValue(min_value + (max_value - min_value) * 0.33, max_value) << "+"
       << config.reset_color << " Medium-Low  ";
    ss << getColorForValue(min_value + (max_value - min_value) * 0.66, max_value) << "#"
       << config.reset_color << " Medium-High  ";
    ss << getColorForValue(max_value, max_value) << "#" << config.reset_color << " High";

    return ss.str();
}

std::string AdvancedResourceVisualizer::generateAsciiScatterPlot(
    const std::vector<double>& x_values, const std::vector<double>& y_values,
    const ChartConfiguration& config) const {
    if (x_values.empty() || y_values.empty() || x_values.size() != y_values.size()) {
        return "[Invalid data for scatter plot]";
    }

    const size_t width = config.width;
    const size_t height = config.height;

    // Find ranges for both axes
    double x_min = *std::min_element(x_values.begin(), x_values.end());
    double x_max = *std::max_element(x_values.begin(), x_values.end());
    double y_min = *std::min_element(y_values.begin(), y_values.end());
    double y_max = *std::max_element(y_values.begin(), y_values.end());

    // Ensure ranges are non-zero
    if (x_max == x_min)
        x_max = x_min + 1.0;
    if (y_max == y_min)
        y_max = y_min + 1.0;

    // Prepare grid for the plot
    std::vector<std::string> grid(height, std::string(width, ' '));

    // Plot each data point
    for (size_t i = 0; i < x_values.size(); ++i) {
        // Normalize coordinates to fit within the grid
        size_t x = static_cast<size_t>((x_values[i] - x_min) / (x_max - x_min) * (width - 1));
        size_t y = height - 1 -
                   static_cast<size_t>((y_values[i] - y_min) / (y_max - y_min) * (height - 1));

        // Check bounds
        if (x < width && y < height) {
            grid[y][x] = '+';
        }
    }

    // Build the scatter plot
    std::stringstream ss;

    // Add y-axis labels and plot frame
    for (size_t y = 0; y < height; ++y) {
        ss << "│ ";

        for (size_t x = 0; x < width; ++x) {
            ss << grid[y][x];
        }

        ss << " │\n";
    }

    // Add x-axis
    ss << "└─" << std::string(width, '-') << "─┘\n";

    // Add axis labels
    ss << "  X: " << x_min << " to " << x_max << "  Y: " << y_min << " to " << y_max << "\n";

    return ss.str();
}

std::string AdvancedResourceVisualizer::generateAsciiChartFrame(
    const std::vector<std::string>& chart_rows, const ChartConfiguration& config) const {
    std::stringstream ss;

    // Add top frame
    ss << "┌─" << std::string(config.width, '-') << "─┐\n";

    // Add title if provided
    if (!config.title.empty()) {
        ss << "│ " << config.title << std::string(config.width - config.title.length(), ' ')
           << " │\n";
        ss << "├─" << std::string(config.width, '-') << "─┤\n";
    }

    // Add chart content
    for (const auto& row : chart_rows) {
        ss << "│ " << row << std::string(config.width - row.length(), ' ') << " │\n";
    }

    // Add bottom frame
    ss << "└─" << std::string(config.width, '-') << "─┘\n";

    return ss.str();
}

std::string AdvancedResourceVisualizer::generateAsciiAxisLabels(
    const std::vector<double>& values, const ChartConfiguration& config) const {
    if (values.empty()) {
        return "";
    }

    // Find min and max values
    double min_value = *std::min_element(values.begin(), values.end());
    double max_value = *std::max_element(values.begin(), values.end());

    // Generate y-axis labels
    std::stringstream ss;

    // Add max value at top
    ss << std::fixed << std::setprecision(2) << max_value << "\n";

    // Add mid-point
    double mid_value = (min_value + max_value) / 2.0;
    ss << std::fixed << std::setprecision(2) << mid_value << "\n";

    // Add min value at bottom
    ss << std::fixed << std::setprecision(2) << min_value << "\n";

    return ss.str();
}

}  // namespace chronovyan