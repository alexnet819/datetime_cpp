#include "datetime.hpp"
#include <iostream>
#include <memory>

int main() {
    // Get current datetime
    auto now = DateTime::current();
    std::cout << "Current datetime: " << now->toString() << std::endl;
    std::cout << "Current milliseconds: " << now->getMillisecond() << std::endl;
    
    // Create a specific datetime with milliseconds
    DateTime christmas(2023, 12, 25, 12, 0, 0, 500);
    std::cout << "Christmas (with 500ms): " << christmas.toString() << std::endl;
    std::cout << "Christmas milliseconds: " << christmas.getMillisecond() << std::endl;
    
    // Datetime operations
    auto tomorrow = now->plusDays(1);
    std::cout << "Tomorrow: " << tomorrow.toString() << std::endl;
    
    // Millisecond arithmetic
    auto plusMs = now->plusMilliseconds(750);
    std::cout << "750ms later: " << plusMs.toString() << std::endl;
    std::cout << "Milliseconds: " << plusMs.getMillisecond() << std::endl;
    
    // Format specification
    std::cout << "Custom format: " << now->toString("%Y-%m-%d %H:%M:%S") << std::endl;
    
    // Use C++20 format feature
    try {
        std::cout << "C++20 format: " << now->formatString("{:%Y-%m-%d}") << std::endl;
    } catch(const std::exception& e) {
        std::cerr << "Format error: " << e.what() << std::endl;
    }
    
    // Comparison
    if (tomorrow > *now) {
        std::cout << "Tomorrow is after today" << std::endl;
    }
    
    // Smart pointer usage example
    std::shared_ptr<DateTime> futureDate = std::make_shared<DateTime>(2030, 1, 1, 0, 0, 0, 250);
    std::cout << "Future date: " << futureDate->toString() << std::endl;
    std::cout << "Future date milliseconds: " << futureDate->getMillisecond() << std::endl;
    
    return 0;
}
