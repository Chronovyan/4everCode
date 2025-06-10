#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include "resource_optimizer.h"
#include "resource_visualization.h"
#include "temporal_debt_tracker.h"
#include "temporal_runtime.h"

#ifndef CHRONOVYAN_NO_GRAPHICAL_VISUALIZATION
#include "graphical_resource_visualizer.h"
#endif

using namespace chronovyan;

// Remove the forward declarations since we are already importing the headers
// and using the chronovyan namespace

void printUsage() {
  std::cout << "Chronovyan Visualization CLI" << std::endl;
  std::cout << "============================" << std::endl;
  std::cout << std::endl;
  std::cout << "Usage: viz_cli [options]" << std::endl;
  std::cout << std::endl;
  std::cout << "Options:" << std::endl;
  std::cout << "  --report <type>      Generate a report of the specified type"
            << std::endl;
  std::cout << "  --format <format>    Specify the output format" << std::endl;
  std::cout << "  --output <file>      Save the output to the specified file"
            << std::endl;
  std::cout << "  --rebel-op <type>    Specify the rebel operation type for "
               "impact reports"
            << std::endl;
  std::cout << "  --projection <int>   Specify the number of cycles to project "
               "for debt impact"
            << std::endl;
  std::cout << "  --operations <int>   Specify the operations per cycle for "
               "rebel impact reports"
            << std::endl;

#ifndef CHRONOVYAN_NO_GRAPHICAL_VISUALIZATION
  // Only show graphical options if graphical visualization is enabled
  std::cout << "  --chart <type>       Generate a chart of the specified type"
            << std::endl;
  std::cout << "  --graph-format <fmt> Specify the graph output format (PNG, "
               "SVG, etc.)"
            << std::endl;
  std::cout << "  --graph-output <file> Save the graph to the specified file"
            << std::endl;
  std::cout
      << "  --resource <type>    Specify the resource type for resource charts"
      << std::endl;
  std::cout << "  --history <int>      Specify the history length for time "
               "series charts"
            << std::endl;
  std::cout << "  --title <string>     Specify the chart title" << std::endl;
  std::cout << "  --width <int>        Specify the chart width in pixels"
            << std::endl;
  std::cout << "  --height <int>       Specify the chart height in pixels"
            << std::endl;
#else
  std::cout << std::endl;
  std::cout << "Note: Graphical visualization is disabled in this build."
            << std::endl;
  std::cout
      << "Install Matplot++ library and rebuild to enable graphical features."
      << std::endl;
#endif

  std::cout << std::endl;
  std::cout << "Report Types:" << std::endl;
  std::cout << "  summary             Basic summary of current resource usage"
            << std::endl;
  std::cout << "  detailed            Detailed report with all metrics and "
               "historical data"
            << std::endl;
  std::cout << "  efficiency          Focus on efficiency metrics and "
               "optimization opportunities"
            << std::endl;
  std::cout << "  debt-analysis       Detailed analysis of temporal debt"
            << std::endl;
  std::cout
      << "  debt-impact         Debt impact over time with repayment strategies"
      << std::endl;
  std::cout << "  rebel-impact        Analysis of Rebel operation impact on "
               "debt and stability"
            << std::endl;
  std::cout
      << "  historical-trends   Analysis of resource usage trends over time"
      << std::endl;
  std::cout
      << "  complete            Complete report with all available information"
      << std::endl;

  std::cout << std::endl;
  std::cout << "Output Formats:" << std::endl;
  std::cout << "  text                Simple text output" << std::endl;
  std::cout << "  json                JSON formatted output for web interfaces"
            << std::endl;
  std::cout << "  csv                 CSV formatted output for spreadsheets"
            << std::endl;
  std::cout << "  compact             Compact single-line representation"
            << std::endl;
  std::cout << "  html                HTML formatted output" << std::endl;

  std::cout << std::endl;
}

RebelOperationType string_to_operation_type(const std::string &op_str) {
  if (op_str == "rewind-flow")
    return RebelOperationType::TEMPORAL_BORROWING;
  if (op_str == "quantum-manipulation")
    return RebelOperationType::TEMPORAL_BORROWING;
  if (op_str == "suspend-timeline")
    return RebelOperationType::TEMPORAL_BORROWING;
  if (op_str == "temporal-echo-loop")
    return RebelOperationType::TEMPORAL_BORROWING;
  if (op_str == "shatter-iteration")
    return RebelOperationType::TEMPORAL_BORROWING;

  // Default
  return RebelOperationType::TEMPORAL_BORROWING;
}

VisualizationFormat getFormatFromString(const std::string &format_str) {
  if (format_str == "json")
    return VisualizationFormat::JSON;
  if (format_str == "csv")
    return VisualizationFormat::CSV;
  if (format_str == "compact")
    return VisualizationFormat::COMPACT;
  if (format_str == "html")
    return VisualizationFormat::HTML;

  // Default to text
  return VisualizationFormat::TEXT;
}

ReportType getReportTypeFromString(const std::string &report_str) {
  if (report_str == "detailed")
    return ReportType::DETAILED;
  if (report_str == "efficiency")
    return ReportType::EFFICIENCY;
  if (report_str == "debt-analysis")
    return ReportType::DEBT_ANALYSIS;
  if (report_str == "debt-impact")
    return ReportType::DEBT_IMPACT;
  if (report_str == "rebel-impact")
    return ReportType::REBEL_IMPACT;
  if (report_str == "historical-trends")
    return ReportType::HISTORICAL_TRENDS;
  if (report_str == "complete")
    return ReportType::COMPLETE;

  // Default to summary
  return ReportType::SUMMARY;
}

#ifndef CHRONOVYAN_NO_GRAPHICAL_VISUALIZATION
ChartType getChartTypeFromString(const std::string &chart_type_str) {
  if (chart_type_str == "bar")
    return ChartType::BAR;
  if (chart_type_str == "pie")
    return ChartType::PIE;
  if (chart_type_str == "area")
    return ChartType::AREA;
  if (chart_type_str == "scatter")
    return ChartType::SCATTER;
  if (chart_type_str == "histogram")
    return ChartType::HISTOGRAM;
  if (chart_type_str == "heatmap")
    return ChartType::HEATMAP;
  if (chart_type_str == "gauge")
    return ChartType::GAUGE;
  if (chart_type_str == "sankey")
    return ChartType::SANKEY;
  if (chart_type_str == "radar")
    return ChartType::RADAR;

  // Default to line
  return ChartType::LINE;
}

GraphOutputFormat
getGraphOutputFormatFromString(const std::string &format_str) {
  if (format_str == "svg")
    return GraphOutputFormat::SVG;
  if (format_str == "pdf")
    return GraphOutputFormat::PDF;
  if (format_str == "html")
    return GraphOutputFormat::HTML;
  if (format_str == "jpeg")
    return GraphOutputFormat::JPEG;

  // Default to PNG
  return GraphOutputFormat::PNG;
}
#endif

int main(int argc, char *argv[]) {
  // Create the necessary components
  auto runtime = std::make_shared<TemporalRuntime>();
  auto debt_tracker = std::make_shared<TemporalDebtTracker>(runtime);
  auto optimizer = std::make_shared<ResourceOptimizer>(runtime, debt_tracker);

  // Create the resource visualization
  auto visualization =
      std::make_shared<ResourceVisualization>(runtime, optimizer, debt_tracker);

#ifndef CHRONOVYAN_NO_GRAPHICAL_VISUALIZATION
  // Create the graphical resource visualizer
  auto graphical_visualization =
      std::make_shared<GraphicalResourceVisualizer>(visualization);
#endif

  // Parse command line arguments
  std::string report_type_str = "summary";
  std::string format_str = "text";
  std::string filename;
  std::string rebel_op_str = "rewind-flow";
  int projection_cycles = 10;
  int operations_per_cycle = 1;

#ifndef CHRONOVYAN_NO_GRAPHICAL_VISUALIZATION
  bool graphical = false;
  std::string chart_type_str = "line";
  std::string graph_format_str = "png";
  int width = 800;
  int height = 600;
#endif

  // Parse command line arguments
  for (int i = 1; i < argc; i++) {
    std::string arg = argv[i];

    if (arg == "--help") {
      printUsage();
      return 0;
    } else if (arg == "--report" && i + 1 < argc) {
      report_type_str = argv[++i];
    } else if (arg == "--format" && i + 1 < argc) {
      format_str = argv[++i];
    } else if (arg == "--output" && i + 1 < argc) {
      filename = argv[++i];
    } else if (arg == "--rebel-op" && i + 1 < argc) {
      rebel_op_str = argv[++i];
    } else if (arg == "--projection" && i + 1 < argc) {
      projection_cycles = std::stoi(argv[++i]);
    } else if (arg == "--operations" && i + 1 < argc) {
      operations_per_cycle = std::stoi(argv[++i]);
    }
#ifndef CHRONOVYAN_NO_GRAPHICAL_VISUALIZATION
    else if (arg == "--graphical") {
      graphical = true;
    } else if (arg == "--chart" && i + 1 < argc) {
      chart_type_str = argv[++i];
    } else if (arg == "--graph-format" && i + 1 < argc) {
      graph_format_str = argv[++i];
    } else if (arg == "--width" && i + 1 < argc) {
      width = std::stoi(argv[++i]);
    } else if (arg == "--height" && i + 1 < argc) {
      height = std::stoi(argv[++i]);
    }
#endif
  }

  // Convert string arguments to enum values
  ReportType report_type = getReportTypeFromString(report_type_str);
  VisualizationFormat format = getFormatFromString(format_str);
  RebelOperationType rebel_op = string_to_operation_type(rebel_op_str);

#ifndef CHRONOVYAN_NO_GRAPHICAL_VISUALIZATION
  ChartType chart_type = getChartTypeFromString(chart_type_str);
  GraphOutputFormat graph_format =
      getGraphOutputFormatFromString(graph_format_str);
#endif

  // Generate the requested output
  std::string output;
  bool export_result = false;

#ifndef CHRONOVYAN_NO_GRAPHICAL_VISUALIZATION
  if (graphical) {
    // Generate graphical output
    ChartOptions options;
    options.width = width;
    options.height = height;

    // TODO: Implement graphical visualization
    std::cout << "Graphical visualization is not fully implemented yet."
              << std::endl;
    return 1;
  } else {
#endif
    // Generate text output
    if (report_type == ReportType::REBEL_IMPACT) {
      if (filename.empty()) {
        // Output to console
        output = visualization->visualizeRebelOperationImpact(
            rebel_op, projection_cycles, operations_per_cycle, format);
        std::cout << output << std::endl;
      } else {
        // Export to file
        export_result = visualization->exportRebelOperationImpactReport(
            rebel_op, format, projection_cycles, operations_per_cycle,
            filename);
        if (export_result) {
          std::cout << "Rebel operation impact report exported to " << filename
                    << std::endl;
        }
      }
    } else {
      // Handle other report types
      if (filename.empty()) {
#ifdef CHRONOVYAN_NO_GRAPHICAL_VISUALIZATION
        // Even without graphical visualization, we can still generate text
        // reports
        output = visualization->generateReport(report_type, format);
        std::cout << output << std::endl;
        export_result = true;
#else
      // Output to console
      output = visualization->generateReport(report_type, format);
      std::cout << output << std::endl;
#endif
      } else {
#ifdef CHRONOVYAN_NO_GRAPHICAL_VISUALIZATION
        // Fallback when graphical visualization is disabled
        std::ofstream outFile(filename);
        if (outFile.is_open()) {
          outFile << "Report generation is limited in this build. Graphical "
                     "visualization is disabled.";
          outFile.close();
          export_result = true;
          std::cout << "Simple report exported to " << filename << std::endl;
        } else {
          export_result = false;
        }
#else
      // Export to file
      export_result =
          visualization->exportReport(report_type, format, filename);
      if (export_result) {
        std::cout << "Report exported to " << filename << std::endl;
      }
#endif
      }
    }
#ifndef CHRONOVYAN_NO_GRAPHICAL_VISUALIZATION
  }
#endif

  if (!export_result && !filename.empty()) {
    std::cerr << "Failed to export to " << filename << std::endl;
    return 1;
  }

  return 0;
}