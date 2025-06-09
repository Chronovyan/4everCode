#include <chrono>
#include <iostream>
#include <thread>

#include "resource_management/resource_processor.h"

int main() {
  std::cout << "Chronovyan Resource Management System" << std::endl;
  std::cout << "===================================" << std::endl;

  // Create a resource processor
  chronovyan::ResourceProcessor processor;

  // Record some sample resource usage
  processor.processCurrentUsage(15.0, 7.5, 1.0, 0.2);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  processor.processCurrentUsage(25.0, 12.5, 1.5, 0.3);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  processor.processCurrentUsage(20.0, 10.0, 1.2, 0.25);

  // Display a summary of resource usage
  std::cout << "\nResource Summary:" << std::endl;
  std::cout << processor.getSummary() << std::endl;

  // Display a visualization
  std::cout << "\nResource Visualization:" << std::endl;
  std::cout << processor.getVisualization() << std::endl;

  return 0;
}