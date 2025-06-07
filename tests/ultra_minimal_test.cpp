#include <map>
#include <memory>
#include <stdexcept>
#include <string>

#include "gtest/gtest.h"

// Ultra-minimalist implementation of resource management
class SimpleResourceManager {
private:
    double m_aethelLevel = 0.0;
    double m_chrononLevel = 0.0;
    std::map<std::string, double> m_variables;
    std::shared_ptr<SimpleResourceManager> m_enclosing;

public:
    SimpleResourceManager() = default;

    explicit SimpleResourceManager(std::shared_ptr<SimpleResourceManager> enclosing)
        : m_enclosing(std::move(enclosing)) {}

    // Resource level getters and setters
    double getAethelLevel() const { return m_aethelLevel; }
    double getChrononLevel() const { return m_chrononLevel; }
    void setAethelLevel(double level) { m_aethelLevel = level; }
    void setChrononLevel(double level) { m_chrononLevel = level; }

    // Resource availability check
    bool hasEnoughAethel(double amount) const { return m_aethelLevel >= amount; }
    bool hasEnoughChronon(double amount) const { return m_chrononLevel >= amount; }

    // Resource expenditure
    void expendAethel(double amount) {
        if (!hasEnoughAethel(amount)) {
            throw std::runtime_error("Not enough Aethel");
        }
        m_aethelLevel -= amount;
    }

    void expendChronon(double amount) {
        if (!hasEnoughChronon(amount)) {
            throw std::runtime_error("Not enough Chronon");
        }
        m_chrononLevel -= amount;
    }

    // Variable management
    void define(const std::string& name, double value) { m_variables[name] = value; }

    bool exists(const std::string& name) const {
        return m_variables.find(name) != m_variables.end() ||
               (m_enclosing && m_enclosing->exists(name));
    }

    double get(const std::string& name) const {
        auto it = m_variables.find(name);
        if (it != m_variables.end()) {
            return it->second;
        }

        if (m_enclosing) {
            return m_enclosing->get(name);
        }

        throw std::runtime_error("Undefined variable: " + name);
    }

    void assign(const std::string& name, double value) {
        if (m_variables.find(name) != m_variables.end()) {
            m_variables[name] = value;
            return;
        }

        if (m_enclosing && m_enclosing->exists(name)) {
            m_enclosing->assign(name, value);
            return;
        }

        throw std::runtime_error("Undefined variable: " + name);
    }
};

// Test fixture for SimpleResourceManager
class UltraMinimalTest : public ::testing::Test {
protected:
    SimpleResourceManager manager;

    void SetUp() override {
        manager.setAethelLevel(100.0);
        manager.setChrononLevel(100.0);
    }
};

// Test for resource level management
TEST_F(UltraMinimalTest, ResourceLevels) {
    EXPECT_EQ(manager.getAethelLevel(), 100.0);
    EXPECT_EQ(manager.getChrononLevel(), 100.0);

    manager.setAethelLevel(50.0);
    manager.setChrononLevel(75.0);

    EXPECT_EQ(manager.getAethelLevel(), 50.0);
    EXPECT_EQ(manager.getChrononLevel(), 75.0);
}

// Test for resource availability
TEST_F(UltraMinimalTest, ResourceAvailability) {
    EXPECT_TRUE(manager.hasEnoughAethel(50.0));
    EXPECT_TRUE(manager.hasEnoughChronon(75.0));

    EXPECT_FALSE(manager.hasEnoughAethel(150.0));
    EXPECT_FALSE(manager.hasEnoughChronon(125.0));
}

// Test for resource expenditure
TEST_F(UltraMinimalTest, ResourceExpenditure) {
    manager.expendAethel(25.0);
    manager.expendChronon(15.0);

    EXPECT_EQ(manager.getAethelLevel(), 75.0);
    EXPECT_EQ(manager.getChrononLevel(), 85.0);

    EXPECT_THROW(manager.expendAethel(100.0), std::runtime_error);
}

// Test for variable management
TEST_F(UltraMinimalTest, VariableManagement) {
    // Define variables
    manager.define("aethel_cost", 30.0);
    manager.define("chronon_cost", 20.0);

    // Check existence
    EXPECT_TRUE(manager.exists("aethel_cost"));
    EXPECT_TRUE(manager.exists("chronon_cost"));
    EXPECT_FALSE(manager.exists("undefined_var"));

    // Retrieve values
    EXPECT_EQ(manager.get("aethel_cost"), 30.0);
    EXPECT_EQ(manager.get("chronon_cost"), 20.0);
    EXPECT_THROW(manager.get("undefined_var"), std::runtime_error);

    // Assign new values
    manager.assign("aethel_cost", 45.0);
    EXPECT_EQ(manager.get("aethel_cost"), 45.0);

    // Try to assign to undefined variable
    EXPECT_THROW(manager.assign("undefined_var", 10.0), std::runtime_error);
}

// Test for enclosing environments
TEST_F(UltraMinimalTest, EnclosingEnvironments) {
    // Define variables in the outer environment
    manager.define("outer_var", 42.0);

    // Create an inner environment with direct reference to the outer environment
    auto innerManager =
        std::make_shared<SimpleResourceManager>(std::make_shared<SimpleResourceManager>(manager));

    // Define variables in the inner environment
    innerManager->define("inner_var", 84.0);

    // Check that the inner environment can access outer variables
    EXPECT_TRUE(innerManager->exists("outer_var"));
    EXPECT_EQ(innerManager->get("outer_var"), 42.0);

    // Check that the inner environment has its own variables
    EXPECT_TRUE(innerManager->exists("inner_var"));
    EXPECT_EQ(innerManager->get("inner_var"), 84.0);

    // Check that the outer environment can't access inner variables
    EXPECT_FALSE(manager.exists("inner_var"));
    EXPECT_THROW(manager.get("inner_var"), std::runtime_error);

    // Since we're not using a reference to the original manager but rather a copy,
    // modifying the outer variable from the inner environment won't affect the original
    innerManager->assign("outer_var", 99.0);
    EXPECT_EQ(manager.get("outer_var"), 42.0);        // Should still be 42.0
    EXPECT_EQ(innerManager->get("outer_var"), 99.0);  // But the inner's view is updated
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}