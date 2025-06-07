---
title: Chronovyan Visualization Theme System
description: Documentation for visualization_themes.md
weight: 100
draft: true
---

# Chronovyan Visualization Theme System

The Chronovyan visualization theme system allows customization of visual output for resource visualizations, making it easier to adapt the display to different environments, user preferences, and output formats.

## Overview

The theme system provides a consistent way to style visualizations across different output formats (TEXT, HTML, JSON, CSV), with special focus on terminal-based text output and HTML rendering. It includes:

- Predefined theme types (DEFAULT, DARK, LIGHT, MINIMAL, ELABORATE)
- Custom theme creation capabilities
- Color settings for different semantic meanings (positive, negative, warning, critical)
- ASCII/Unicode character customization for gauges, charts, and tables
- Formatting options (bold, indentation, etc.)

## Using Predefined Themes

The `ResourceVisualization` class includes several predefined themes that can be easily applied:

```cpp
// Create a visualization object
ResourceVisualization visualizer(runtime, optimizer, debt_tracker);

// Set a predefined theme
visualizer.setThemeType(ThemeType::DARK);

// Generate visualizations with the theme
std::string chart = visualizer.generateASCIIChart(data, 40, 10, "Resource Usage");
```

### Available Theme Types

- `ThemeType::DEFAULT` - Standard balanced theme
- `ThemeType::DARK` - Optimized for dark terminal backgrounds
- `ThemeType::LIGHT` - Optimized for light terminal backgrounds
- `ThemeType::MINIMAL` - Simplified visualization with minimal decoration
- `ThemeType::ELABORATE` - Rich visualization with detailed decoration
- `ThemeType::CUSTOM` - For user-defined themes

## Creating Custom Themes

You can create and apply custom themes to match specific requirements:

```cpp
// Create a custom theme
VisualizationTheme customTheme;

// Set custom colors using ANSI escape codes
customTheme.positive_color = "\033[32m";  // Green
customTheme.negative_color = "\033[31m";  // Red
customTheme.warning_color = "\033[33m";   // Yellow
customTheme.title_color = "\033[1;36m";   // Bold Cyan

// Set custom ASCII characters
customTheme.ascii_chars.filled_gauge = '#';
customTheme.ascii_chars.empty_gauge = '.';
customTheme.ascii_chars.data_point = '*';

// Configure theme features
customTheme.use_unicode = false;  // Use ASCII instead of Unicode
customTheme.use_bold = true;      // Use bold formatting for headers
customTheme.indentation = 2;      // Indentation spaces for nested items

// Apply the custom theme
visualizer.setVisualizationTheme(customTheme);
```

## Theme Properties

### Color Settings

| Property | Description | Default |
|----------|-------------|---------|
| `positive_color` | Used for positive values/trends | Green |
| `negative_color` | Used for negative values/trends | Red |
| `warning_color` | Used for warning indicators | Yellow |
| `critical_color` | Used for critical indicators | Bright Red |
| `header_color` | Used for headers and titles | Cyan |
| `title_color` | Used for main titles | Bold Blue |
| `text_color` | Used for regular text | White |
| `text_muted` | Used for secondary text | Gray |
| `value_high` | Used for high values (>70%) | Green |
| `value_medium` | Used for medium values (30-70%) | Yellow |
| `value_low` | Used for low values (<30%) | Red |

### ASCII Characters

| Property | Description | Default |
|----------|-------------|---------|
| `filled_gauge` | Character used for filled portions of gauges | █ or # |
| `empty_gauge` | Character used for empty portions of gauges | ░ or . |
| `x_axis` | Character used for X-axis lines | ─ or - |
| `y_axis` | Character used for Y-axis lines | │ or \| |
| `data_point` | Character used for data points in charts | ● or * |

### HTML Theme Settings

| Property | Description | Default |
|----------|-------------|---------|
| `html_background_color` | Background color for HTML output | #ffffff or #1e1e1e |
| `html_text_color` | Text color for HTML output | #333333 or #ffffff |
| `html_header_background` | Background color for headers | #f0f0f0 or #333333 |
| `html_accent_color` | Accent color for highlights | #4a86e8 |

## Theme-Aware Methods

The following methods in `ResourceVisualization` support theming:

- `generateASCIIChart()` - Creates charts with themed axes and data points
- `generateASCIIGauge()` - Creates gauges with themed colors based on values
- `generateThemedBar()` - Creates horizontal bars with themed colors
- `visualizeCurrentState()` - Shows current resource state with themed formatting
- `visualizeTrends()` - Shows resource trends with themed formatting
- `generateDashboard()` - Creates a themed dashboard of resource information

## Example Usage

See `examples/theme_visualization_demo.cpp` for a complete demonstration of the theme system, including:

- Displaying the same data with different themes
- Creating and applying a custom theme
- Comparing various visualization methods with themes applied

## Best Practices

1. **Terminal Compatibility**: Some terminal emulators might not support all ANSI colors or Unicode characters. Use `use_unicode = false` for maximum compatibility.

2. **Color Contrast**: Ensure your custom themes maintain good contrast for readability.

3. **Format Consistency**: When generating multiple visualizations for the same report, use the same theme for consistency.

4. **Theme Selection**: Choose themes appropriate for the display environment:
   - DARK theme for dark terminal backgrounds
   - LIGHT theme for light terminal backgrounds
   - MINIMAL theme for limited-color environments or file outputs

5. **Accessibility**: Consider color blindness when creating custom themes - avoid relying solely on red/green distinctions.

## Implementation Details

The theme system is implemented in:
- `include/resource_visualization.h` - Theme structures and declarations
- `src/resource_visualization.cpp` - Theme implementation and application