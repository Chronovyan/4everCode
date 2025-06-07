#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "error_handler.h"
#include "interpreter.h"
#include "resource_visualization.h"
#include "temporal_runtime.h"
#include "user_type_system.h"

using namespace chronovyan;

// Helper function to print separators for better readability
void printSeparator(const std::string& title = "") {
    std::cout << "\n";
    if (!title.empty()) {
        std::cout << "=== " << title << " ===\n";
    }
    std::cout << "================================================\n";
}

// Helper function to print a fancy table header
void printTableHeader(const std::vector<std::string>& columns, const std::vector<int>& widths) {
    std::cout << "| ";
    for (size_t i = 0; i < columns.size(); i++) {
        std::cout << std::left << std::setw(widths[i]) << columns[i] << " | ";
    }
    std::cout << "\n|";
    for (size_t i = 0; i < columns.size(); i++) {
        std::cout << std::string(widths[i] + 2, '-') << "|";
    }
    std::cout << "\n";
}

int main() {
    // Create error handler
    auto& errorHandler = ErrorHandler::getInstance();
    errorHandler.setVerbosity(ErrorVerbosity::VERBOSE);

    // Create interpreter with runtime
    auto runtime = std::make_shared<TemporalRuntime>(100000);  // 100,000 chronons limit
    Interpreter interpreter(runtime);

    std::cout << "Chronovyan Temporal Inventory System Example\n";
    std::cout << "===========================================\n\n";

    std::cout
        << "This example demonstrates a practical application of the user-definable TYPE system\n";
    std::cout
        << "by implementing a temporal inventory tracking system that can track item history.\n\n";

    // Define the inventory system types
    printSeparator("Setting Up Type Definitions");

    std::string typeDefinitionsScript = R"(
        // Define Product Category enum
        DEFINE_ENUM("ProductCategory", {
            "ELECTRONICS": 1,
            "CLOTHING": 2,
            "FOOD": 3,
            "BOOKS": 4,
            "HOUSEHOLD": 5
        });
        
        // Define Supplier struct
        DEFINE_STRUCT("Supplier", {
            "id": "int",
            "name": "string",
            "contactEmail": "string",
            "contactPhone": {
                "type": "string",
                "required": false
            },
            "reliability": {
                "type": "float",
                "default": 0.95
            }
        });
        
        // Define Product struct
        DEFINE_STRUCT("Product", {
            "id": "int",
            "name": "string",
            "description": {
                "type": "string",
                "required": false
            },
            "category": "ProductCategory",
            "price": "float",
            "supplier": "Supplier",
            "tags": "Array"
        });
        
        // Define temporal InventoryItem struct
        DEFINE_STRUCT("InventoryItem", {
            "product": "Product",
            "quantity": "int",
            "location": "string",
            "lastRestocked": "int",
            "reserved": {
                "type": "int",
                "default": 0
            },
            "notes": {
                "type": "string",
                "required": false
            }
        }, true);  // true for temporal support
        
        // Define Order struct
        DEFINE_STRUCT("Order", {
            "id": "int",
            "customerName": "string",
            "items": "Array",  // Array of InventoryItems
            "orderDate": "int",
            "status": "string",
            "total": "float"
        }, true);  // temporal to track order status changes
        
        // Return success
        return "Type definitions created successfully";
    )";

    try {
        Value result = interpreter.interpretText(typeDefinitionsScript);
        std::cout << "Result: " << result.toString() << "\n";
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << "\n";
        return 1;
    }

    // Set up initial inventory data
    printSeparator("Creating Initial Inventory Data");

    std::string setupInventoryScript = R"(
        // Create suppliers
        var acmeCorp = CREATE_INSTANCE("Supplier", {
            "id": 1001,
            "name": "Acme Corporation",
            "contactEmail": "contact@acme.com",
            "contactPhone": "555-123-4567",
            "reliability": 0.98
        });
        
        var techInnovations = CREATE_INSTANCE("Supplier", {
            "id": 1002,
            "name": "Tech Innovations",
            "contactEmail": "support@techinnovations.com",
            "reliability": 0.92
        });
        
        var bookHaven = CREATE_INSTANCE("Supplier", {
            "id": 1003,
            "name": "Book Haven Publishing",
            "contactEmail": "orders@bookhaven.com",
            "contactPhone": "555-987-6543"
        });
        
        // Create products
        var laptop = CREATE_INSTANCE("Product", {
            "id": 101,
            "name": "Premium Laptop",
            "description": "High-performance laptop with 16GB RAM and 1TB SSD",
            "category": CREATE_INSTANCE("ProductCategory", {"value": "ELECTRONICS"}),
            "price": 1299.99,
            "supplier": techInnovations,
            "tags": ["computer", "high-end", "work"]
        });
        
        var tShirt = CREATE_INSTANCE("Product", {
            "id": 102,
            "name": "Cotton T-Shirt",
            "description": "Comfortable 100% cotton t-shirt",
            "category": CREATE_INSTANCE("ProductCategory", {"value": "CLOTHING"}),
            "price": 19.99,
            "supplier": acmeCorp,
            "tags": ["apparel", "casual", "summer"]
        });
        
        var novel = CREATE_INSTANCE("Product", {
            "id": 103,
            "name": "Mystery Novel",
            "description": "Bestselling mystery novel by J.R. Writer",
            "category": CREATE_INSTANCE("ProductCategory", {"value": "BOOKS"}),
            "price": 12.95,
            "supplier": bookHaven,
            "tags": ["fiction", "paperback", "bestseller"]
        });
        
        // Create inventory items
        var laptopInventory = CREATE_INSTANCE("InventoryItem", {
            "product": laptop,
            "quantity": 15,
            "location": "Warehouse A, Shelf 12",
            "lastRestocked": 1698523200,  // October 29, 2023
            "notes": "High demand item, reorder when below 5 units"
        });
        
        var tShirtInventory = CREATE_INSTANCE("InventoryItem", {
            "product": tShirt,
            "quantity": 150,
            "location": "Warehouse B, Shelf 5",
            "lastRestocked": 1696863600,  // October 9, 2023
            "reserved": 25
        });
        
        var novelInventory = CREATE_INSTANCE("InventoryItem", {
            "product": novel,
            "quantity": 75,
            "location": "Warehouse A, Shelf 30",
            "lastRestocked": 1697727600  // October 19, 2023
        });
        
        // Store items in global variables for later access
        define laptopItem = laptopInventory;
        define tShirtItem = tShirtInventory;
        define novelItem = novelInventory;
        
        return [laptopInventory, tShirtInventory, novelInventory];
    )";

    try {
        Value result = interpreter.interpretText(setupInventoryScript);
        std::cout << "Initial inventory items created: " << result.toString() << "\n";
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << "\n";
        return 1;
    }

    // Simulate inventory operations over time
    printSeparator("Simulating Inventory Operations Over Time");

    std::string simulateOperationsScript = R"(
        // Display initial inventory
        function displayInventory(items) {
            print("\nCurrent Inventory (Cycle " + GET_CURRENT_CYCLE() + "):");
            print("----------------------------------------------------------");
            print("| Product                 | Qty | Reserved | Location     |");
            print("----------------------------------------------------------");
            
            foreach(item in items) {
                var product = GET_INSTANCE_FIELD(item, "product");
                var productName = GET_INSTANCE_FIELD(product, "name");
                var qty = GET_INSTANCE_FIELD(item, "quantity");
                var reserved = GET_INSTANCE_FIELD(item, "reserved");
                var location = GET_INSTANCE_FIELD(item, "location");
                
                // Format the output
                var nameDisplay = productName.substring(0, 22);
                if (nameDisplay.length() < 22) {
                    nameDisplay = nameDisplay + " ".repeat(22 - nameDisplay.length());
                }
                
                print("| " + nameDisplay + " | " + 
                     qty.toString().padLeft(3) + " | " + 
                     reserved.toString().padLeft(8) + " | " + 
                     location.substring(0, 12) + " |");
            }
            print("----------------------------------------------------------");
        }
        
        var inventoryItems = [laptopItem, tShirtItem, novelItem];
        displayInventory(inventoryItems);
        
        // Cycle 1: Sell some laptops
        ADVANCE_CYCLE();
        print("\nCycle 1: Selling 3 laptops");
        var currentQty = GET_INSTANCE_FIELD(laptopItem, "quantity");
        SET_INSTANCE_FIELD(laptopItem, "quantity", currentQty - 3);
        displayInventory(inventoryItems);
        
        // Cycle 2: Reserve some t-shirts for a bulk order
        ADVANCE_CYCLE();
        print("\nCycle 2: Reserving 50 more t-shirts");
        var currentReserved = GET_INSTANCE_FIELD(tShirtItem, "reserved");
        SET_INSTANCE_FIELD(tShirtItem, "reserved", currentReserved + 50);
        displayInventory(inventoryItems);
        
        // Cycle 3: Restock novels
        ADVANCE_CYCLE();
        print("\nCycle 3: Restocking novels with 25 more copies");
        var currentNovelQty = GET_INSTANCE_FIELD(novelItem, "quantity");
        SET_INSTANCE_FIELD(novelItem, "quantity", currentNovelQty + 25);
        SET_INSTANCE_FIELD(novelItem, "lastRestocked", 1699041600);  // November 3, 2023
        displayInventory(inventoryItems);
        
        // Cycle 4: Fulfill t-shirt reservation
        ADVANCE_CYCLE();
        print("\nCycle 4: Fulfilling t-shirt reservation (50 items)");
    var tshirtQty = GET_INSTANCE_FIELD(tShirtItem, "quantity");
    var tshirtReserved = GET_INSTANCE_FIELD(tShirtItem, "reserved");
    SET_INSTANCE_FIELD(tShirtItem, "quantity", tshirtQty - 50);
    SET_INSTANCE_FIELD(tShirtItem, "reserved", tshirtReserved - 50);
    displayInventory(inventoryItems);

    // Cycle 5: Move laptop inventory to a new location
    ADVANCE_CYCLE();
    print("\nCycle 5: Moving laptops to a new warehouse location");
    SET_INSTANCE_FIELD(laptopItem, "location", "Warehouse C, Shelf 8");
    SET_INSTANCE_FIELD(laptopItem, "notes",
                       GET_INSTANCE_FIELD(laptopItem, "notes") + " - Moved to high-security area");
    displayInventory(inventoryItems);

    return "Operations simulation completed";
    )";
    
    try {
        Value result = interpreter.interpretText(simulateOperationsScript);
        std::cout << "\nResult: " << result.toString() << "\n";
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << "\n";
        return 1;
    }

    // Analyze inventory history
    printSeparator("Analyzing Inventory History");

    std::string analyzeHistoryScript = R"(
        // Function to analyze and display an item's history
        function analyzeItemHistory(item, name) {
            print("\nHistory for " + name + ":");
            print("----------------------------------------------------------");
            print("| Cycle | Quantity | Reserved | Location                 |");
            print("----------------------------------------------------------");
            
            var history = GET_INSTANCE_HISTORY(item);
            
            // Sort by cycle
            var cycles = [];
            foreach(cycle in history.keys()) {
                cycles.push(parseInt(cycle));
            }
            cycles.sort();
            
            foreach(cycle in cycles) {
                var itemAtCycle = GET_INSTANCE_AT_CYCLE(item, cycle);
                var qty = GET_INSTANCE_FIELD(itemAtCycle, "quantity");
                var reserved = GET_INSTANCE_FIELD(itemAtCycle, "reserved");
                var location = GET_INSTANCE_FIELD(itemAtCycle, "location");
                
                // Format the output
                var cycleStr = cycle.toString().padLeft(5);
                var qtyStr = qty.toString().padLeft(8);
                var reservedStr = reserved.toString().padLeft(8);
                var locationStr = location.substring(0, 24);
                if (locationStr.length() < 24) {
                    locationStr = locationStr + " ".repeat(24 - locationStr.length());
                }
                
                print("| " + cycleStr + " | " + qtyStr + " | " + reservedStr + " | " + locationStr + " |");
            }
            print("----------------------------------------------------------");
            
            // Calculate statistics
            var totalQtyChange = GET_INSTANCE_FIELD(GET_INSTANCE_AT_CYCLE(item, cycles[cycles.length-1]), "quantity") - 
                               GET_INSTANCE_FIELD(GET_INSTANCE_AT_CYCLE(item, cycles[0]), "quantity");
            
            print("Total quantity change: " + totalQtyChange);
            
            // Detect if location changed
            var initialLocation = GET_INSTANCE_FIELD(GET_INSTANCE_AT_CYCLE(item, cycles[0]), "location");
            var finalLocation = GET_INSTANCE_FIELD(GET_INSTANCE_AT_CYCLE(item, cycles[cycles.length-1]), "location");
            
            if (initialLocation != finalLocation) {
                print("Location changed from \"" + initialLocation + "\" to \"" + finalLocation + "\"");
            }
            
            return history;
        }
        
        // Analyze each item's history
        var laptopHistory = analyzeItemHistory(laptopItem, "Premium Laptop");
        var tShirtHistory = analyzeItemHistory(tShirtItem, "Cotton T-Shirt");
        var novelHistory = analyzeItemHistory(novelItem, "Mystery Novel");
        
        // Perform advanced temporal query - find items with quantity decreases
        print("\nItems with decreasing quantity:");
        var itemsWithDecreasingQty = [];
        
        function hasDecreasingQuantity(item) {
            var history = GET_INSTANCE_HISTORY(item);
            var cycles = [];
            foreach(cycle in history.keys()) {
                cycles.push(parseInt(cycle));
            }
            cycles.sort();
            
            if (cycles.length < 2) return false;
            
            var initialQty = GET_INSTANCE_FIELD(GET_INSTANCE_AT_CYCLE(item, cycles[0]), "quantity");
            var finalQty = GET_INSTANCE_FIELD(GET_INSTANCE_AT_CYCLE(item, cycles[cycles.length-1]), "quantity");
            
            return finalQty < initialQty;
        }
        
        var inventoryItems = [laptopItem, tShirtItem, novelItem];
        foreach(item in inventoryItems) {
            if (hasDecreasingQuantity(item)) {
                var product = GET_INSTANCE_FIELD(item, "product");
                var name = GET_INSTANCE_FIELD(product, "name");
                itemsWithDecreasingQty.push(name);
            }
        }
        
        print(itemsWithDecreasingQty);
        
        return "History analysis completed";
    )";

    try {
        Value result = interpreter.interpretText(analyzeHistoryScript);
        std::cout << "\nResult: " << result.toString() << "\n";
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << "\n";
        return 1;
    }

    // Create an order using the inventory
    printSeparator("Creating and Processing an Order");

    std::string orderProcessingScript = R"(
        // Create a new order
        var order = CREATE_INSTANCE("Order", {
            "id": 5001,
            "customerName": "Jane Smith",
            "items": [laptopItem, novelItem],
            "orderDate": 1699214400,  // November 5, 2023
            "status": "Pending",
            "total": 0.0
        });
        
        // Calculate the total
        function calculateOrderTotal(order) {
            var items = GET_INSTANCE_FIELD(order, "items");
            var total = 0.0;
            
            foreach(item in items) {
                var product = GET_INSTANCE_FIELD(item, "product");
                var price = GET_INSTANCE_FIELD(product, "price");
                total += price;
            }
            
            return total;
        }
        
        var orderTotal = calculateOrderTotal(order);
        SET_INSTANCE_FIELD(order, "total", orderTotal);
        
        print("Order created: ");
        print("  ID: " + GET_INSTANCE_FIELD(order, "id"));
        print("  Customer: " + GET_INSTANCE_FIELD(order, "customerName"));
        print("  Status: " + GET_INSTANCE_FIELD(order, "status"));
        print("  Total: $" + GET_INSTANCE_FIELD(order, "total"));
        
        // Process the order through various states
        ADVANCE_CYCLE();
        print("\nCycle " + GET_CURRENT_CYCLE() + ": Processing payment");
        SET_INSTANCE_FIELD(order, "status", "Payment Processing");
        
        ADVANCE_CYCLE();
        print("\nCycle " + GET_CURRENT_CYCLE() + ": Payment confirmed");
        SET_INSTANCE_FIELD(order, "status", "Payment Confirmed");
        
        ADVANCE_CYCLE();
        print("\nCycle " + GET_CURRENT_CYCLE() + ": Order shipped");
        SET_INSTANCE_FIELD(order, "status", "Shipped");
        
        ADVANCE_CYCLE();
        print("\nCycle " + GET_CURRENT_CYCLE() + ": Order delivered");
        SET_INSTANCE_FIELD(order, "status", "Delivered");
        
        // Display order history
        print("\nOrder Status History:");
        print("----------------------------------------------------------");
        print("| Cycle | Status               |");
        print("----------------------------------------------------------");
        
        var orderHistory = GET_INSTANCE_HISTORY(order);
        var cycles = [];
        foreach(cycle in orderHistory.keys()) {
            cycles.push(parseInt(cycle));
        }
        cycles.sort();
        
        foreach(cycle in cycles) {
            var orderAtCycle = GET_INSTANCE_AT_CYCLE(order, cycle);
            var status = GET_INSTANCE_FIELD(orderAtCycle, "status");
            
            var cycleStr = cycle.toString().padLeft(5);
            var statusStr = status;
            if (statusStr.length() < 20) {
                statusStr = statusStr + " ".repeat(20 - statusStr.length());
            }
            
            print("| " + cycleStr + " | " + statusStr + " |");
        }
        print("----------------------------------------------------------");
        
        return order;
    )";

    try {
        Value result = interpreter.interpretText(orderProcessingScript);
        std::cout << "\nResult: " << result.toString() << "\n";
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << "\n";
        return 1;
    }

    // Report resource usage
    printSeparator("Resource Usage");
    std::cout << "Total chronons used: " << runtime->getChrononsUsed() << "\n\n";

    try {
        // Visualize resource usage
        ResourceVisualization::visualizeResourceUsage(*runtime);
    } catch (const std::exception& e) {
        std::cout << "Resource visualization not available: " << e.what() << "\n";
    }

    return 0;
}