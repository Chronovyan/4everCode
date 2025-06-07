#include <iostream>
#include <memory>
#include <string>

#include "error_handler.h"
#include "interpreter.h"
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

// Execute a Chronovyan script and print its result
void executeScript(Interpreter& interpreter, const std::string& script,
                   const std::string& title = "Script Result") {
    printSeparator(title);
    std::cout << "Script:\n" << script << "\n\n";

    try {
        Value result = interpreter.interpretText(script);
        std::cout << "Result: " << result.toString() << "\n";
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << "\n";
    }
}

int main() {
    // Create error handler
    auto& errorHandler = ErrorHandler::getInstance();
    errorHandler.setVerbosity(ErrorVerbosity::VERBOSE);

    // Create interpreter with runtime
    auto runtime = std::make_shared<TemporalRuntime>(10000);  // 10,000 chronons limit
    Interpreter interpreter(runtime);

    std::cout << "Chronovyan User-Definable TYPE System Demonstration\n";
    std::cout << "=================================================\n\n";

    // 1. Defining a simple struct type
    printSeparator("1. Defining a Simple Struct");

    std::string definePersonScript = R"(
        // Define a Person struct type
        DEFINE_STRUCT("Person", {
            "name": "string",
            "age": "int",
            "isActive": "bool"
        });
        
        // Create an instance
        var person = CREATE_INSTANCE("Person", {
            "name": "John Doe",
            "age": 30,
            "isActive": true
        });
        
        // Access fields
        print("Person: " + person);
        print("Name: " + GET_INSTANCE_FIELD(person, "name"));
        print("Age: " + GET_INSTANCE_FIELD(person, "age"));
        print("Active: " + GET_INSTANCE_FIELD(person, "isActive"));
        
        // Modify a field
        SET_INSTANCE_FIELD(person, "age", 31);
        print("Updated Age: " + GET_INSTANCE_FIELD(person, "age"));
        
        return person;
    )";

    executeScript(interpreter, definePersonScript);

    // 2. Defining an enum type
    printSeparator("2. Defining an Enum Type");

    std::string defineColorScript = R"(
        // Define a Color enum type
        DEFINE_ENUM("Color", {
            "RED": 1,
            "GREEN": 2,
            "BLUE": 3,
            "YELLOW": 4
        });
        
        // Create an instance and use it
        var color = CREATE_INSTANCE("Color", { "value": "RED" });
        print("Color: " + color);
        
        // Check for specific value
        if (GET_INSTANCE_FIELD(color, "value") == "RED") {
            print("This is a red color!");
        }
        
        // Change the color
        SET_INSTANCE_FIELD(color, "value", "BLUE");
        print("New color: " + GET_INSTANCE_FIELD(color, "value"));
        
        return color;
    )";

    executeScript(interpreter, defineColorScript);

    // 3. Using a temporal struct
    printSeparator("3. Using a Temporal Struct");

    std::string temporalVehicleScript = R"(
        // Define a temporal Vehicle struct
        DEFINE_STRUCT("Vehicle", {
            "model": "string",
            "year": "int",
            "mileage": "int",
            "owner": "string"
        }, true);  // true for temporal support
        
        // Create an instance
        var car = CREATE_INSTANCE("Vehicle", {
            "model": "Tesla Model 3",
            "year": 2023,
            "mileage": 0,
            "owner": "First Owner"
        });
        
        print("Initial state: " + car);
        
        // Advance cycle and modify
        ADVANCE_CYCLE();
        SET_INSTANCE_FIELD(car, "mileage", 1000);
        print("After 1 cycle, mileage: " + GET_INSTANCE_FIELD(car, "mileage"));
        
        // Advance cycle and modify again
        ADVANCE_CYCLE();
        SET_INSTANCE_FIELD(car, "mileage", 5000);
        SET_INSTANCE_FIELD(car, "owner", "Second Owner");
        print("After 2 cycles, mileage: " + GET_INSTANCE_FIELD(car, "mileage"));
        print("After 2 cycles, owner: " + GET_INSTANCE_FIELD(car, "owner"));
        
        // Check history
        var history = GET_INSTANCE_HISTORY(car);
        print("History entries: " + history.size());
        
        // Get state at specific cycle
        var carAtCycle1 = GET_INSTANCE_AT_CYCLE(car, 1);
        print("Car at cycle 1: " + carAtCycle1);
        print("Mileage at cycle 1: " + GET_INSTANCE_FIELD(carAtCycle1, "mileage"));
        print("Owner at cycle 1: " + GET_INSTANCE_FIELD(carAtCycle1, "owner"));
        
        return car;
    )";

    executeScript(interpreter, temporalVehicleScript);

    // 4. Defining a complex type with nested types
    printSeparator("4. Complex Type with Nested Types");

    std::string complexTypeScript = R"(
        // Define an Address struct
        DEFINE_STRUCT("Address", {
            "street": "string",
            "city": "string",
            "zipCode": "string",
            "country": "string"
        });
        
        // Define a Contact struct that uses Address
        DEFINE_STRUCT("Contact", {
            "firstName": "string",
            "lastName": "string",
            "email": {
                "type": "string",
                "required": false
            },
            "phone": {
                "type": "string",
                "required": true
            },
            "address": "Address"  // Using the Address type
        });
        
        // Create an address instance
        var address = CREATE_INSTANCE("Address", {
            "street": "123 Main St",
            "city": "San Francisco",
            "zipCode": "94105",
            "country": "USA"
        });
        
        // Create a contact using the address
        var contact = CREATE_INSTANCE("Contact", {
            "firstName": "Jane",
            "lastName": "Smith",
            "phone": "555-123-4567",
            "address": address
        });
        
        print("Contact: " + contact);
        print("Contact's city: " + 
              GET_INSTANCE_FIELD(
                  GET_INSTANCE_FIELD(contact, "address"), 
                  "city"
              )
        );
        
        // Add an email later
        SET_INSTANCE_FIELD(contact, "email", "jane.smith@example.com");
        print("Email added: " + GET_INSTANCE_FIELD(contact, "email"));
        
        return contact;
    )";

    executeScript(interpreter, complexTypeScript);

    // 5. Using type aliases and unions
    printSeparator("5. Type Aliases and Unions");

    std::string aliasAndUnionScript = R"(
        // Define a type alias
        DEFINE_ALIAS("Username", "string");
        
        // Define a union type
        DEFINE_UNION("IDType", ["string", "int"]);
        
        // Define a User struct using these types
        DEFINE_STRUCT("User", {
            "username": "Username",  // Using the alias
            "id": "IDType",          // Using the union
            "createdAt": "int"
        });
        
        // Create users with different ID types
        var user1 = CREATE_INSTANCE("User", {
            "username": "johndoe",
            "id": "ABC123",          // String ID
            "createdAt": 1620000000
        });
        
        var user2 = CREATE_INSTANCE("User", {
            "username": "janedoe",
            "id": 456789,            // Integer ID
            "createdAt": 1630000000
        });
        
        print("User 1: " + user1);
        print("User 1 ID: " + GET_INSTANCE_FIELD(user1, "id"));
        print("User 1 ID type: " + TYPE_OF(GET_INSTANCE_FIELD(user1, "id")));
        
        print("User 2: " + user2);
        print("User 2 ID: " + GET_INSTANCE_FIELD(user2, "id"));
        print("User 2 ID type: " + TYPE_OF(GET_INSTANCE_FIELD(user2, "id")));
        
        return [user1, user2];
    )";

    executeScript(interpreter, aliasAndUnionScript);

    // Report resource usage
    printSeparator("Resource Usage");
    std::cout << "Total chronons used: " << runtime->getChrononsUsed() << "\n";

    return 0;
}