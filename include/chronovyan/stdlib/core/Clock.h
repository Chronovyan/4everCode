#pragma once

#include <chrono>
#include <ctime>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "chronovyan/stdlib/core/Core.h"

namespace chronovyan::stdlib {

// Forward declarations
class Clock;
using ClockPtr = std::shared_ptr<Clock>;

// Clock class declaration
class Clock : public Value, public std::enable_shared_from_this<Clock> {
public:
    ~Clock() override = default;

    // Value interface implementation
    Type type() const override { return Type::CLOCK; }
    std::string toString() const override { return "<native fn>"; }

    // Clone the clock (returns the singleton instance)
    ValuePtr clone() const override { return instance(); }

    // Override base class methods to avoid hiding
    using Value::call;
    using Value::get;
    using Value::has;
    using Value::set;

    // Override call method
    ValuePtr call(const std::vector<ValuePtr>& args) override;

    // Clock-specific functionality
    double now() const;
    std::string formatTime(double timestamp, const std::string& format) const;
    std::string nowFormatted(const std::string& format = "%Y-%m-%d %H:%M:%S") const;
    void sleep(double seconds) const;

    // Singleton instance
    static std::shared_ptr<Clock> instance();

    // Support copy and move (returns the singleton instance)
    Clock(const Clock&) = default;
    Clock(Clock&&) = default;
    Clock& operator=(const Clock&) = default;
    Clock& operator=(Clock&&) = default;

private:
    // Private constructor for singleton
    Clock() = default;
};

}  // namespace chronovyan::stdlib
