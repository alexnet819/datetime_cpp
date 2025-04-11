#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <memory>
#include <format>
#include <vector>
#include <optional>
#include <mutex>

// Define formatter for custom time type
namespace std {
    template<>
    struct formatter<tm> {
        constexpr format_parse_context::iterator parse(format_parse_context& dt_ctx) {
            return dt_ctx.begin();
        }
        
        format_context::iterator format(const tm& dt_t, format_context& dt_ctx) const {
            // Format year, month, day, hour, minute, second from tm struct
            return format_to(dt_ctx.out(), "{:04d}-{:02d}-{:02d} {:02d}:{:02d}:{:02d}",
                             dt_t.tm_year + 1900, dt_t.tm_mon + 1, dt_t.tm_mday,
                             dt_t.tm_hour, dt_t.tm_min, dt_t.tm_sec);
        }
    };
}

// Add exception class for error handling
class DateTimeException : public std::runtime_error {
public:
    explicit DateTimeException(const std::string& dt_message) : std::runtime_error(dt_message) {}
};

class DateTime {
public:
    // Structure representing region time information
    struct RegionTime {
        std::string identifier;   // Region identifier (e.g., "JST", "UTC")
        int hourOffset;           // Hour offset from reference time
        int minuteOffset;         // Additional minute offset from reference time

        RegionTime(const std::string& dt_regionId, int dt_hours, int dt_minutes = 0)
            : identifier(dt_regionId), hourOffset(dt_hours), minuteOffset(dt_minutes) {}
    };

    // Commonly used region time definitions
    static const RegionTime WorldTime;   // UTC
    static const RegionTime JapanTime;   // JST
    static const RegionTime EasternTime; // EST
    static const RegionTime PacificTime; // PST
    
    // Constructors
    DateTime();
    explicit DateTime(const std::chrono::system_clock::time_point& dt_time);
    // Constructor with region time specification
    DateTime(int dt_year, int dt_month, int dt_day, int dt_hour = 0, int dt_minute = 0, int dt_second = 0, int dt_millisecond = 0, const RegionTime& dt_region=WorldTime);
    DateTime(const std::chrono::system_clock::time_point& dt_time, const RegionTime& dt_region);
    
    // Get current time
    static std::shared_ptr<DateTime> current();
    static std::shared_ptr<DateTime> current(const RegionTime& dt_region);
    
    // Set date and time
    void setDate(int dt_year, int dt_month, int dt_day);
    void setTime(int dt_hour, int dt_minute, int dt_second, int dt_millisecond = 0);
    
    // Set and get region time
    void setRegion(const RegionTime& dt_region);
    const RegionTime& getRegion() const;
    
    // Get datetime in a different region time
    DateTime convertToRegion(const RegionTime& dt_targetRegion) const;
    
    // Get date and time components
    int getYear() const;
    int getMonth() const;
    int getDay() const;
    int getHour() const;
    int getMinute() const;
    int getSecond() const;
    int getMillisecond() const;
    int getDayOfWeek() const; // Get day of week (0=Sunday, 6=Saturday)
    
    // Date and time arithmetic
    DateTime plusYears(int dt_years) const;
    DateTime plusMonths(int dt_months) const;
    DateTime plusDays(int dt_days) const;
    DateTime plusHours(int dt_hours) const;
    DateTime plusMinutes(int dt_minutes) const;
    DateTime plusSeconds(int dt_seconds) const;
    DateTime plusMilliseconds(int dt_milliseconds) const;
    
    // Comparison operators
    bool operator==(const DateTime& dt_other) const;
    bool operator!=(const DateTime& dt_other) const;
    bool operator<(const DateTime& dt_other) const;
    bool operator<=(const DateTime& dt_other) const;
    bool operator>(const DateTime& dt_other) const;
    bool operator>=(const DateTime& dt_other) const;
    
    // Formatting
    std::string toString(const std::string& dt_format = "%Y-%m-%d %H:%M:%S") const;
    // Formatting with region time information
    std::string toStringWithRegion(const std::string& dt_format = "%Y-%m-%d %H:%M:%S %Z") const;
    
    // String formatting using C++20 features
    std::string formatString(std::string_view dt_fmt) const;
    
    // Get the difference between two datetimes
    static std::chrono::seconds timeBetween(const DateTime& dt_dt1, const DateTime& dt_dt2);
    
    // Get internal time point
    std::chrono::system_clock::time_point getSystemTime() const;

    // Date and time validity verification methods
    static bool isValidDate(int dt_year, int dt_month, int dt_day);
    static bool isValidTime(int dt_hour, int dt_minute, int dt_second, int dt_millisecond);

    // Get timezone information from TZDB (C++20)
    static std::optional<RegionTime> getRegionFromTZDB(const std::string& dt_tzName);
    
    // Parse datetime from string (enhanced error handling)
    static std::optional<DateTime> parse(const std::string& dt_dateString, 
                                       const std::string& dt_format = "%Y-%m-%d %H:%M:%S");

private:
    std::chrono::system_clock::time_point dt_clockPoint;
    RegionTime dt_timeRegion{WorldTime};  // Default is world time (UTC)
    
    // Static mutex for thread-safe processing (moved from global variable)
    static std::mutex dt_timeMutex;
    
    // Helper function to get time adjusted for region
    std::tm getRegionAdjustedTime() const;

    // Thread-safe time retrieval function
    std::tm getThreadSafeTime(const std::time_t& dt_timeValue, bool dt_useLocalTime) const;
    
    // Helper function for timezone offset calculation (eliminate code duplication)
    std::time_t getLocalTimeOffset() const;
};