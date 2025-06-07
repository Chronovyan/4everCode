#include <matplot/matplot.h>

#include <vector>

int main() {
    using namespace matplot;

    // Create some data
    std::vector<double> x = linspace(0, 10, 100);
    std::vector<double> y = transform(x, [](auto x) { return sin(x); });

    // Create a line plot
    auto h = plot(x, y, "-o");
    h->line_width(2);
    h->marker_size(4);

    // Add title and labels
    title("Simple Plot Example");
    xlabel("X-axis");
    ylabel("sin(x)");

    // Show the plot
    show();

    return 0;
}
