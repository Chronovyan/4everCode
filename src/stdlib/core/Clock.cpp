#include <chrono>
#include <ctime>
#include <iomanip>
#include <memory>
#include <sstream>
#include <stdexcept>

#include "chronovyan/stdlib/core/Clock.h"
#include "chronovyan/stdlib/core/Core.h"

namespace chronovyan::stdlib {

ValuePtr Clock::call(const std::vector<ValuePtr> &args) {
  // No arguments: return current time as a number
  if (args.empty()) {
    return Number::create(now());
  }

  // First argument is a string: treat as format string
  if (args[0] && args[0]->type() == Type::STRING) {
    std::string format = std::static_pointer_cast<String>(args[0])->value();
    return String::create(nowFormatted(format));
  }

  // First argument is a number: sleep for that many seconds
  if (args[0] && args[0]->type() == Type::NUMBER) {
    double seconds = std::static_pointer_cast<Number>(args[0])->value();
    sleep(seconds);
    return shared_from_this();
  }

  // Unsupported argument type, return current time as a number
  return Number::create(now());
}

double Clock::now() const {
  using namespace std::chrono;
  return duration_cast<duration<double>>(system_clock::now().time_since_epoch())
      .count();
}

std::string Clock::formatTime(double timestamp,
                              const std::string &format) const {
  using namespace std::chrono;

  // Convert timestamp to time_point
  system_clock::time_point tp{
      duration_cast<system_clock::duration>(duration<double>(timestamp))};

  // Convert to time_t for formatting
  std::time_t time = system_clock::to_time_t(tp);
  std::tm tm;

// Thread-safe time conversion
#ifdef _WIN32
  localtime_s(&tm, &time);
#else
  localtime_r(&time, &tm);
#endif

  // Format the time
  std::ostringstream oss;
  oss << std::put_time(&tm, format.c_str());
  return oss.str();
}

std::string Clock::nowFormatted(const std::string &format) const {
  return formatTime(now(), format);
}

void Clock::sleep(double seconds) const {
  if (seconds > 0) {
    std::this_thread::sleep_for(
        std::chrono::milliseconds(static_cast<long long>(seconds * 1000)));
  }
}

std::shared_ptr<Clock> Clock::instance() {
  static std::shared_ptr<Clock> instance = std::shared_ptr<Clock>(new Clock());
  return instance;
}

} // namespace chronovyan::stdlib
