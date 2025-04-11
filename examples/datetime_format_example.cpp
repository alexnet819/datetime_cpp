#include "datetime.hpp"
#include <iostream>
#include <memory>

int main() {
    // Get current datetime
    auto now = DateTime::current();
    
    // Create datetime with specific milliseconds
    DateTime dt(2023, 10, 15, 14, 30, 45, 789);
    
    // Various formatting examples
    std::cout << "Default format: " << dt.toString() << std::endl;
    std::cout << "Milliseconds: " << dt.getMillisecond() << std::endl;
    
    // strftime style format
    std::cout << "Custom format (strftime): " << dt.toString("%Y-%m-%d %H:%M:%S") << std::endl;
    
    // Add milliseconds to custom format
    std::string withMs = dt.toString("%Y-%m-%d %H:%M:%S") + "." + 
                         std::to_string(dt.getMillisecond());
    std::cout << "With milliseconds: " << withMs << std::endl;
    
    // C++20 std::format style
    std::cout << "Date only (C++20 format): " << dt.formatString("{:%Y-%m-%d}") << std::endl;
    std::cout << "Time only (C++20 format): " << dt.formatString("{:%H:%M:%S}") << std::endl;
    std::cout << "Date and time (C++20 format): " << dt.formatString("{:%Y-%m-%d %H:%M:%S}") << std::endl;
    
    // With timezone information
    std::cout << "With timezone: " << dt.toStringWithRegion() << std::endl;
    
    // Test millisecond arithmetic
    DateTime start(2023, 1, 1, 0, 0, 0, 0);
    
    for (int i = 0; i < 5; i++) {
        auto time = start.plusMilliseconds(i * 250);
        std::cout << "Plus " << (i * 250) << "ms: " 
                  << time.toString("%H:%M:%S") << "." << time.getMillisecond() << std::endl;
    }
    
    return 0;
}
