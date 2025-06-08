#include <chronovyan/chronovyan.hpp>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace chronovyan {

class Chronovyan::Impl {
public:
  Impl() = default;
  ~Impl() = default;

  bool is_ready_{true};
};

Chronovyan::Chronovyan() : pimpl_(std::make_unique<Impl>()) {}
Chronovyan::~Chronovyan() = default;

std::string Chronovyan::version() noexcept { return "0.1.0"; }

bool Chronovyan::is_ready() const noexcept {
  return pimpl_ ? pimpl_->is_ready_ : false;
}

std::chrono::system_clock::time_point Chronovyan::now() const {
  return std::chrono::system_clock::now();
}

std::string
Chronovyan::format_time(const std::chrono::system_clock::time_point &time,
                        const std::string &format) const {

  // Convert to time_t for C-style formatting
  auto time_t = std::chrono::system_clock::to_time_t(time);

  // Convert to tm struct
  std::tm tm;
#ifdef _WIN32
  localtime_s(&tm, &time_t);
#else
  tm = *std::localtime(&time_t);
#endif

  // Format the time
  std::ostringstream oss;
  oss << std::put_time(&tm, format.c_str());

  // Add milliseconds if needed
  if (format.find("%f") != std::string::npos) {
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                  time.time_since_epoch()) %
              1000;
    std::string ms_str = std::to_string(ms.count());
    ms_str = std::string(3 - ms_str.length(), '0') + ms_str;

    // Replace %f with milliseconds
    std::string result = oss.str();
    size_t pos = result.find("%f");
    if (pos != std::string::npos) {
      result.replace(pos, 2, ms_str);
    }
    return result;
  }

  return oss.str();
}

std::chrono::system_clock::time_point
Chronovyan::parse_time(const std::string &time_str,
                       const std::string &format) const {

  std::tm tm = {};
  std::istringstream ss(time_str);

  // Parse the time string according to the format
  ss >> std::get_time(&tm, format.c_str());

  if (ss.fail()) {
    throw std::runtime_error("Failed to parse time string: " + time_str);
  }

  // Convert tm to time_t
  std::time_t time = std::mktime(&tm);
  if (time == -1) {
    throw std::runtime_error("Failed to convert time");
  }

  // Handle timezone conversion (mktime assumes local time)
  auto local_time = std::chrono::system_clock::from_time_t(time);

  // Get the current time zone offset
  auto now = std::chrono::system_clock::now();
  auto now_time_t = std::chrono::system_clock::to_time_t(now);

  std::tm local_tm = {};
  std::tm utc_tm = {};

#ifdef _WIN32
  localtime_s(&local_tm, &now_time_t);
  gmtime_s(&utc_tm, &now_time_t);
#else
  local_tm = *std::localtime(&now_time_t);
  utc_tm = *std::gmtime(&now_time_t);
#endif

  // Calculate timezone offset in seconds
  auto local_secs = std::mktime(&local_tm);
  auto utc_secs = std::mktime(&utc_tm);
  auto tz_offset = std::difftime(local_secs, utc_secs);

  // Adjust the parsed time by the timezone offset
  return local_time - std::chrono::seconds(static_cast<long long>(tz_offset));
}

} // namespace chronovyan
