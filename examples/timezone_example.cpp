#include "datetime.hpp"
#include <iostream>
#include <memory>

int main() {
    // Current time in various time zones with milliseconds
    auto utcNow = DateTime::current(DateTime::WorldTime);
    auto jstNow = DateTime::current(DateTime::JapanTime);
    auto estNow = DateTime::current(DateTime::EasternTime);
    
    std::cout << "Current time (UTC): " << utcNow->toString() << "." << utcNow->getMillisecond() 
              << " " << utcNow->getRegion().identifier << std::endl;
    std::cout << "Current time (Japan): " << jstNow->toString() << "." << jstNow->getMillisecond() 
              << " " << jstNow->getRegion().identifier << std::endl;
    std::cout << "Current time (US Eastern): " << estNow->toString() << "." << estNow->getMillisecond() 
              << " " << estNow->getRegion().identifier << std::endl;
    
    // Time zone conversion
    std::cout << "\n--- Time Zone Conversion ---" << std::endl;
    auto jstToUtc = jstNow->convertToRegion(DateTime::WorldTime);
    auto utcToJst = utcNow->convertToRegion(DateTime::JapanTime);
    
    std::cout << "Japan time to UTC: " << jstToUtc.toString() << "." << jstToUtc.getMillisecond() 
              << " " << jstToUtc.getRegion().identifier << std::endl;
    std::cout << "UTC to Japan time: " << utcToJst.toString() << "." << utcToJst.getMillisecond() 
              << " " << utcToJst.getRegion().identifier << std::endl;
    
    // Format with time zone
    std::cout << "\n--- Format with Time Zone ---" << std::endl;
    std::cout << "Japan time (standard format): " << jstNow->toString() << std::endl;
    std::cout << "Japan time (with milliseconds): " << jstNow->toString() << "." << jstNow->getMillisecond() << std::endl;
    std::cout << "Japan time (with TZ): " << jstNow->toStringWithRegion() << std::endl;
    std::cout << "Japan time (custom format with TZ): " << jstNow->toStringWithRegion("%Y-%m-%d %H:%M:%S %Z") << std::endl;
    
    // Create datetime in specific time zones with milliseconds
    std::cout << "\n--- Creating Date/Time with Milliseconds in Specific Time Zone ---" << std::endl;
    DateTime newYorkNewYear(2023, 1, 1, 0, 0, 0, 500, DateTime::EasternTime);
    DateTime tokyoNewYear(2023, 1, 1, 0, 0, 0, 500, DateTime::JapanTime);
    
    std::cout << "New York New Year: " << newYorkNewYear.toString() << "." << newYorkNewYear.getMillisecond() 
              << " " << newYorkNewYear.getRegion().identifier << std::endl;
    std::cout << "Tokyo New Year: " << tokyoNewYear.toString() << "." << tokyoNewYear.getMillisecond() 
              << " " << tokyoNewYear.getRegion().identifier << std::endl;
    
    // Convert New York New Year to Tokyo time (should preserve milliseconds)
    auto newYorkInTokyo = newYorkNewYear.convertToRegion(DateTime::JapanTime);
    std::cout << "New York New Year (Tokyo time): " << newYorkInTokyo.toString() << "." << newYorkInTokyo.getMillisecond() 
              << " " << newYorkInTokyo.getRegion().identifier << std::endl;
    
    return 0;
}
