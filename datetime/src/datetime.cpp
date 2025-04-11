#include "datetime.hpp"
#include <chrono>
#include <ctime>
#include <time.h>  // Added for timegm
#include <iomanip>
#include <sstream>
#include <format>
#include <memory>
#include <mutex>
#include <stdexcept>

// Initialize static region time constants
const DateTime::RegionTime DateTime::WorldTime{"UTC", 0};
const DateTime::RegionTime DateTime::JapanTime{"JST", 9};
const DateTime::RegionTime DateTime::EasternTime{"EST", -5};
const DateTime::RegionTime DateTime::PacificTime{"PST", -8};

// Initialize static mutex (as a class member rather than a global variable)
std::mutex DateTime::dt_timeMutex;

// Use common prefix (dt_) for all variable names and methods
DateTime::DateTime() : dt_clockPoint(std::chrono::system_clock::now()), dt_timeRegion(WorldTime) {}

DateTime::DateTime(const std::chrono::system_clock::time_point& dt_time) : dt_clockPoint(dt_time), dt_timeRegion(WorldTime) {}


// Constructor with region time specification
DateTime::DateTime(int dt_year, int dt_month, int dt_day, 
                    int dt_hour, int dt_minute, int dt_second, int dt_millisecond, 
                    const RegionTime& dt_region) : dt_timeRegion(dt_region) 
{
    std::tm dt_timeStruct{};
    dt_timeStruct.tm_year = dt_year - 1900;
    dt_timeStruct.tm_mon = dt_month - 1;
    dt_timeStruct.tm_mday = dt_day;
    dt_timeStruct.tm_hour = dt_hour;
    dt_timeStruct.tm_min = dt_minute;
    dt_timeStruct.tm_sec = dt_second;
    
    // Interpret the specified time as region time
    std::time_t dt_timeValue = timegm(&dt_timeStruct);
    
    // Reverse apply the region time offset to convert to UTC-based time point
    std::time_t dt_offsetSeconds = -1 * (dt_region.hourOffset * 3600 + dt_region.minuteOffset * 60);
    dt_timeValue += dt_offsetSeconds;
    
    // Convert adjusted time to time point
    dt_clockPoint = std::chrono::system_clock::from_time_t(dt_timeValue);
    dt_clockPoint += std::chrono::milliseconds(dt_millisecond);
}

DateTime::DateTime(const std::chrono::system_clock::time_point& dt_time, const RegionTime& dt_region) 
    : dt_clockPoint(dt_time), dt_timeRegion(dt_region) {}

std::shared_ptr<DateTime> DateTime::current() {
    return std::make_shared<DateTime>(std::chrono::system_clock::now(), WorldTime);
}

std::shared_ptr<DateTime> DateTime::current(const RegionTime& dt_region) {
    return std::make_shared<DateTime>(std::chrono::system_clock::now(), dt_region);
}

// Thread-safe tm struct retrieval function
std::tm DateTime::getThreadSafeTime(const std::time_t& dt_timeValue, bool dt_useLocalTime) const {
    std::tm dt_timeStruct{};
    std::lock_guard<std::mutex> dt_lock(dt_timeMutex);  // Using static member instead of global
    
    #if defined(__linux__) || defined(__APPLE__)
        // Use thread-safe version on Linux/Mac
        if (dt_useLocalTime) {
            localtime_r(&dt_timeValue, &dt_timeStruct);
        } else {
            gmtime_r(&dt_timeValue, &dt_timeStruct);
        }
    #else
        // On other platforms like Windows, protect with mutex
        if (dt_useLocalTime) {
            dt_timeStruct = *std::localtime(&dt_timeValue);
        } else {
            dt_timeStruct = *std::gmtime(&dt_timeValue);
        }
    #endif
    
    return dt_timeStruct;
}

// Timezone offset retrieval function (eliminate code duplication)
std::time_t DateTime::getLocalTimeOffset() const {
    std::time_t dt_nowValue = std::time(nullptr);
    std::tm dt_localTimeStruct = getThreadSafeTime(dt_nowValue, true);
    
    #if defined(_WIN32) || defined(_WIN64)
        // On Windows, tm_gmtoff doesn't exist, so calculate separately
        std::tm dt_gmTimeStruct = getThreadSafeTime(dt_nowValue, false);
        std::time_t dt_localTime = std::mktime(&dt_localTimeStruct);
        std::time_t dt_gmTime = std::mktime(&dt_gmTimeStruct);
        return dt_localTime - dt_gmTime;
    #else
        // On Linux/Mac, tm_gmtoff can be used directly
        return dt_localTimeStruct.tm_gmtoff;
    #endif
}

// Date validity verification function
bool DateTime::isValidDate(int dt_year, int dt_month, int dt_day) {
    if (dt_month < 1 || dt_month > 12 || dt_day < 1)
        return false;
        
    // Date validation using C++20 calendar type
    try {
        if (dt_year < 1900 || dt_year > 9999) return false;
        
        // Calculate the last day of each month
        const int dt_daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        int dt_maxDay = dt_daysInMonth[dt_month];
        
        // February has 29 days in leap years
        if (dt_month == 2) {
            bool dt_isLeap = (dt_year % 4 == 0) && ((dt_year % 100 != 0) || (dt_year % 400 == 0));
            if (dt_isLeap) dt_maxDay = 29;
        }
        
        return dt_day <= dt_maxDay;
    }
    catch (const std::exception&) {
        return false;
    }
}

bool DateTime::isValidTime(int dt_hour, int dt_minute, int dt_second, int dt_millisecond) {
    return dt_hour >= 0 && dt_hour < 24 &&
           dt_minute >= 0 && dt_minute < 60 &&
           dt_second >= 0 && dt_second < 60 &&
           dt_millisecond >= 0 && dt_millisecond < 1000;
}

// Add validation when setting date/time
void DateTime::setDate(int dt_year, int dt_month, int dt_day) {
    // Date validity verification
    if (!isValidDate(dt_year, dt_month, dt_day)) {
        throw DateTimeException("Invalid date specified: " + 
                               std::to_string(dt_year) + "-" + 
                               std::to_string(dt_month) + "-" + 
                               std::to_string(dt_day));
    }
    
    std::tm dt_timeStruct = getRegionAdjustedTime();
    dt_timeStruct.tm_year = dt_year - 1900;
    dt_timeStruct.tm_mon = dt_month - 1;
    dt_timeStruct.tm_mday = dt_day;
    
    // Interpret the specified time as region time
    std::time_t dt_timeValue = timegm(&dt_timeStruct);
    
    // Reverse apply the region time offset to convert to UTC-based time point
    std::time_t dt_offsetSeconds = -1 * (dt_timeRegion.hourOffset * 3600 + dt_timeRegion.minuteOffset * 60);
    dt_timeValue += dt_offsetSeconds;
    
    // Convert adjusted time to time point
    dt_clockPoint = std::chrono::system_clock::from_time_t(dt_timeValue);
}

void DateTime::setTime(int dt_hour, int dt_minute, int dt_second, int dt_millisecond) {
    // Time validity verification (including milliseconds)
    if (!isValidTime(dt_hour, dt_minute, dt_second, dt_millisecond)) {
        throw DateTimeException("Invalid time specified: " + 
                               std::to_string(dt_hour) + ":" + 
                               std::to_string(dt_minute) + ":" + 
                               std::to_string(dt_second) + "." +
                               std::to_string(dt_millisecond));
    }
    
    std::tm dt_timeStruct = getRegionAdjustedTime();
    dt_timeStruct.tm_hour = dt_hour;
    dt_timeStruct.tm_min = dt_minute;
    dt_timeStruct.tm_sec = dt_second;
    
    // Interpret the specified time as region time
    std::time_t dt_timeValue = timegm(&dt_timeStruct);
    
    // Reverse apply the region time offset to convert to UTC-based time point
    std::time_t dt_offsetSeconds = -1 * (dt_timeRegion.hourOffset * 3600 + dt_timeRegion.minuteOffset * 60);
    dt_timeValue += dt_offsetSeconds;
    
    // Convert adjusted time to time point
    dt_clockPoint = std::chrono::system_clock::from_time_t(dt_timeValue);
    
    // Add milliseconds
    dt_clockPoint += std::chrono::milliseconds(dt_millisecond);
}

void DateTime::setRegion(const RegionTime& dt_region) {
    dt_timeRegion = dt_region;
}

const DateTime::RegionTime& DateTime::getRegion() const {
    return dt_timeRegion;
}

DateTime DateTime::convertToRegion(const RegionTime& dt_targetRegion) const {
    return DateTime(dt_clockPoint, dt_targetRegion);
}

// Thread-safe RegionAdjustedTime implementation
std::tm DateTime::getRegionAdjustedTime() const {
    std::time_t dt_timeValue = std::chrono::system_clock::to_time_t(dt_clockPoint);
    std::tm dt_timeStruct = getThreadSafeTime(dt_timeValue, false); // Get based on UTC
    
    // Apply region time offset
    std::time_t dt_regionOffsetValue = dt_timeRegion.hourOffset * 3600 + dt_timeRegion.minuteOffset * 60;
    dt_timeValue += dt_regionOffsetValue;
    dt_timeStruct = getThreadSafeTime(dt_timeValue, false);
    
    return dt_timeStruct;
}

int DateTime::getYear() const {
    std::tm dt_timeStruct = getRegionAdjustedTime();
    return dt_timeStruct.tm_year + 1900;
}

int DateTime::getMonth() const {
    std::tm dt_timeStruct = getRegionAdjustedTime();
    return dt_timeStruct.tm_mon + 1;
}

int DateTime::getDay() const {
    std::tm dt_timeStruct = getRegionAdjustedTime();
    return dt_timeStruct.tm_mday;
}

int DateTime::getHour() const {
    std::tm dt_timeStruct = getRegionAdjustedTime();
    return dt_timeStruct.tm_hour;
}

int DateTime::getMinute() const {
    std::tm dt_timeStruct = getRegionAdjustedTime();
    return dt_timeStruct.tm_min;
}

int DateTime::getSecond() const {
    std::tm dt_timeStruct = getRegionAdjustedTime();
    return dt_timeStruct.tm_sec;
}

int DateTime::getMillisecond() const {
    // Milliseconds cannot be directly retrieved from the standard library, so calculate them
    auto dt_duration = dt_clockPoint.time_since_epoch();
    auto dt_milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(dt_duration);
    return static_cast<int>(dt_milliseconds.count() % 1000);
}
int DateTime::getDayOfWeek() const {
    std::tm dt_timeStruct = getRegionAdjustedTime();
    return dt_timeStruct.tm_wday; // 0=Sunday, 6=Saturday
}

// Modified version of plusYears method
DateTime DateTime::plusYears(int dt_years) const {
    // Get time structure adjusted for region
    std::tm dt_timeStruct = getRegionAdjustedTime();
    
    // Add years
    dt_timeStruct.tm_year += dt_years;
    
    // Convert to UTC-based time (using timegm instead of mktime)
    std::time_t dt_timeValue = timegm(&dt_timeStruct);
    
    // Reverse apply the region time offset to convert to UTC-based time point
    std::time_t dt_offsetSeconds = -1 * (dt_timeRegion.hourOffset * 3600 + dt_timeRegion.minuteOffset * 60);
    dt_timeValue += dt_offsetSeconds;
    
    // Convert adjusted time to time point
    std::chrono::system_clock::time_point dt_newTimePoint = 
        std::chrono::system_clock::from_time_t(dt_timeValue);
    
    // Return new DateTime object (preserving region time info)
    return DateTime(dt_newTimePoint, dt_timeRegion);
}

// Modified version of plusMonths method
DateTime DateTime::plusMonths(int dt_months) const {
    // Get time structure adjusted for region
    std::tm dt_timeStruct = getRegionAdjustedTime();
    
    // Save the original day
    int dt_originalDay = dt_timeStruct.tm_mday;
    
    // Store year and month temporarily for calculation
    int dt_newYear = dt_timeStruct.tm_year;
    int dt_newMonth = dt_timeStruct.tm_mon + dt_months;
    
    // Handle month overflow (addition of 12+ months)
    if (dt_newMonth >= 12) {
        dt_newYear += dt_newMonth / 12;
        dt_newMonth %= 12;
    }
    // Handle negative case as well
    else if (dt_newMonth < 0) {
        int dt_yearAdjust = (dt_newMonth / 12) - 1;
        dt_newYear += dt_yearAdjust;
        dt_newMonth = 12 + (dt_newMonth % 12);
    }
    
    // Calculate the last day of the new month
    int dt_maxDay = 31; // Default value
    
    // Set days for each month
    if (dt_newMonth == 1) { // February
        dt_maxDay = 28;
        // Leap year check
        int dt_year = dt_newYear + 1900;
        if ((dt_year % 4 == 0 && dt_year % 100 != 0) || dt_year % 400 == 0) {
            dt_maxDay = 29;
        }
    } else if (dt_newMonth == 3 || dt_newMonth == 5 || 
              dt_newMonth == 8 || dt_newMonth == 10) {
        dt_maxDay = 30; // April, June, September, November have 30 days
    }
    
    // Set the new date
    dt_timeStruct.tm_year = dt_newYear;
    dt_timeStruct.tm_mon = dt_newMonth;
    dt_timeStruct.tm_mday = std::min(dt_originalDay, dt_maxDay);
    
    // Convert to UTC-based time
    std::time_t dt_timeValue = timegm(&dt_timeStruct);
    
    // Reverse apply the region time offset to convert to UTC-based time point
    std::time_t dt_offsetSeconds = -1 * (dt_timeRegion.hourOffset * 3600 + dt_timeRegion.minuteOffset * 60);
    dt_timeValue += dt_offsetSeconds;
    
    // Convert adjusted time to time point
    std::chrono::system_clock::time_point dt_newTimePoint = 
        std::chrono::system_clock::from_time_t(dt_timeValue);
    
    // Return new DateTime object
    return DateTime(dt_newTimePoint, dt_timeRegion);
}

DateTime DateTime::plusDays(int dt_days) const {
    std::chrono::system_clock::time_point dt_newTime = dt_clockPoint + std::chrono::hours(24 * dt_days);
    return DateTime(dt_newTime, dt_timeRegion);
}

DateTime DateTime::plusHours(int dt_hours) const {
    std::chrono::system_clock::time_point dt_newTime = dt_clockPoint + std::chrono::hours(dt_hours);
    return DateTime(dt_newTime, dt_timeRegion);
}

DateTime DateTime::plusMinutes(int dt_minutes) const {
    std::chrono::system_clock::time_point dt_newTime = dt_clockPoint + std::chrono::minutes(dt_minutes);
    return DateTime(dt_newTime, dt_timeRegion);
}

DateTime DateTime::plusSeconds(int dt_seconds) const {
    std::chrono::system_clock::time_point dt_newTime = dt_clockPoint + std::chrono::seconds(dt_seconds);
    return DateTime(dt_newTime, dt_timeRegion);
}

DateTime DateTime::plusMilliseconds(int dt_milliseconds) const {
    std::chrono::system_clock::time_point dt_newTime = dt_clockPoint + std::chrono::milliseconds(dt_milliseconds);
    return DateTime(dt_newTime, dt_timeRegion);
}
bool DateTime::operator==(const DateTime& dt_other) const {
    return dt_clockPoint == dt_other.dt_clockPoint;
}

bool DateTime::operator!=(const DateTime& dt_other) const {
    return dt_clockPoint != dt_other.dt_clockPoint;
}

bool DateTime::operator<(const DateTime& dt_other) const {
    return dt_clockPoint < dt_other.dt_clockPoint;
}

bool DateTime::operator<=(const DateTime& dt_other) const {
    return dt_clockPoint <= dt_other.dt_clockPoint;
}

bool DateTime::operator>(const DateTime& dt_other) const {
    return dt_clockPoint > dt_other.dt_clockPoint;
}

bool DateTime::operator>=(const DateTime& dt_other) const {
    return dt_clockPoint >= dt_other.dt_clockPoint;
}

std::string DateTime::toString(const std::string& dt_format) const {
    std::tm dt_timeStruct = getRegionAdjustedTime();
    std::stringstream dt_stringStream;
    dt_stringStream << std::put_time(&dt_timeStruct, dt_format.c_str());
    return dt_stringStream.str();
}

std::string DateTime::toStringWithRegion(const std::string& dt_format) const {
    // Check if the format string contains %Z
    bool dt_hasTimezoneFormat = (dt_format.find("%Z") != std::string::npos);
    
    // Create a temporary format without %Z (to be replaced with appropriate timezone later)
    std::string dt_tempFormat = dt_format;
    if (dt_hasTimezoneFormat) {
        // Replace %Z with a temporary placeholder
        size_t dt_pos = dt_tempFormat.find("%Z");
        dt_tempFormat.replace(dt_pos, 2, "##TZ_PLACEHOLDER##");
    }
    
    // Convert to string using the basic format
    std::tm dt_timeStruct = getRegionAdjustedTime();
    std::stringstream dt_stringStream;
    dt_stringStream << std::put_time(&dt_timeStruct, dt_tempFormat.c_str());
    std::string dt_result = dt_stringStream.str();
    
    // Replace placeholder with the correct timezone identifier
    if (dt_hasTimezoneFormat) {
        size_t dt_pos = dt_result.find("##TZ_PLACEHOLDER##");
        if (dt_pos != std::string::npos) {
            dt_result.replace(dt_pos, 18, dt_timeRegion.identifier);
        }
    } else {
        // If timezone info is not included in the format, append to the end
        dt_result += " " + dt_timeRegion.identifier;
    }
    
    return dt_result;
}

// DateTime parsing from string (enhanced error handling)
std::optional<DateTime> DateTime::parse(const std::string& dt_dateString, const std::string& dt_format) {
    std::tm dt_timeStruct{};
    std::istringstream dt_ss(dt_dateString);
    dt_ss >> std::get_time(&dt_timeStruct, dt_format.c_str());
    
    if (dt_ss.fail()) {
        return std::nullopt;
    }
    
    try {
        // Validate parsed values
        if (!isValidDate(dt_timeStruct.tm_year + 1900, dt_timeStruct.tm_mon + 1, dt_timeStruct.tm_mday) ||
            !isValidTime(dt_timeStruct.tm_hour, dt_timeStruct.tm_min, dt_timeStruct.tm_sec, 0)) {
            return std::nullopt;
        }
        
        // Create DateTime object from valid datetime information
        return DateTime(dt_timeStruct.tm_year + 1900, dt_timeStruct.tm_mon + 1, dt_timeStruct.tm_mday,
                       dt_timeStruct.tm_hour, dt_timeStruct.tm_min, dt_timeStruct.tm_sec);
    }
    catch (const DateTimeException&) {
        return std::nullopt;
    }
}

// Enhanced formatting function using C++20 format
std::string DateTime::formatString(std::string_view dt_fmt) const {
    std::tm dt_timeStruct = getRegionAdjustedTime();
    try {
        // Use default format if empty
        if (dt_fmt.empty()) {
            return std::format("{}", dt_timeStruct);
        }
        
        // If format string starts with {: and ends with }, extract the content
        std::string_view dt_pattern = dt_fmt;
        if (dt_pattern.size() >= 4 && dt_pattern[0] == '{' && dt_pattern[1] == ':' && 
            dt_pattern[dt_pattern.size()-1] == '}') {
            // Extract %Y-%m-%d from {:%Y-%m-%d}
            std::string extracted(dt_fmt.substr(2, dt_fmt.size() - 3));
            
            // Process using strftime style
            char dt_buffer[100];
            std::strftime(dt_buffer, sizeof(dt_buffer), extracted.c_str(), &dt_timeStruct);
            return dt_buffer;
        }
        
        // strftime style format
        if (dt_fmt.find("%") != std::string_view::npos) {
            char dt_buffer[100];
            std::strftime(dt_buffer, sizeof(dt_buffer), dt_fmt.data(), &dt_timeStruct);
            return dt_buffer;
        }
        
        // Maintain existing processing for other formats
        // ...other format processing...
        
        // Fall back to toString if failed
        return toString(std::string(dt_fmt));
    } catch (const std::exception& dt_exception) {
        return "Format error: " + std::string(dt_exception.what());
    }
}

std::chrono::seconds DateTime::timeBetween(const DateTime& dt_dt1, const DateTime& dt_dt2) {
    // Take the difference between time_points directly, cast to seconds
    return std::chrono::duration_cast<std::chrono::seconds>(dt_dt2.dt_clockPoint - dt_dt1.dt_clockPoint);
}

std::chrono::system_clock::time_point DateTime::getSystemTime() const {
    return dt_clockPoint;
}
