#include "datetime.hpp"
#include <gtest/gtest.h>
#include <chrono>
#include <vector>

// Helper class for timing performance tests
class Timer {
public:
    Timer() : start(std::chrono::high_resolution_clock::now()) {}
    
    double elapsedMilliseconds() {
        auto now = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::milli>(now - start).count();
    }

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
};

// Performance test for mass datetime operations
TEST(PerformanceTest, MassDateTimeOperations) {
    constexpr int iterations = 10000;
    std::vector<DateTime> dateTimes;
    dateTimes.reserve(iterations);
    
    // Create many DateTime objects
    Timer createTimer;
    for (int i = 0; i < iterations; ++i) {
        dateTimes.emplace_back(2023, 1, 1, 12, 0, 0);
    }
    double createTime = createTimer.elapsedMilliseconds();
    std::cout << "Created " << iterations << " DateTime objects in " << createTime << "ms" << std::endl;
    
    // Many date operations
    Timer operationTimer;
    for (int i = 0; i < iterations; ++i) {
        dateTimes[i] = dateTimes[i].plusDays(i % 100);
    }
    double operationTime = operationTimer.elapsedMilliseconds();
    std::cout << "Performed " << iterations << " plusDays operations in " << operationTime << "ms" << std::endl;
    
    // Many formatting operations
    Timer formatTimer;
    std::string result;
    for (int i = 0; i < iterations; ++i) {
        result = dateTimes[i].toString();
    }
    double formatTime = formatTimer.elapsedMilliseconds();
    std::cout << "Performed " << iterations << " toString operations in " << formatTime << "ms" << std::endl;
    
    // Timezone conversion operations
    Timer tzTimer;
    for (int i = 0; i < iterations; ++i) {
        auto converted = dateTimes[i].convertToRegion(DateTime::JapanTime);
    }
    double tzTime = tzTimer.elapsedMilliseconds();
    std::cout << "Performed " << iterations << " timezone conversions in " << tzTime << "ms" << std::endl;
    
    // Verify performance is within acceptable thresholds
    // Note: these values may vary greatly by environment, so use relaxed constraints
    EXPECT_LT(createTime / iterations, 1.0);  // Less than 1ms per object
    EXPECT_LT(operationTime / iterations, 1.0);  // Less than 1ms per operation
    EXPECT_LT(formatTime / iterations, 1.0);  // Less than 1ms per format
    EXPECT_LT(tzTime / iterations, 1.0);  // Less than 1ms per conversion
}

// Performance test for frequent current time retrieval
TEST(PerformanceTest, CurrentTimePerformance) {
    constexpr int iterations = 1000;
    
    Timer timer;
    for (int i = 0; i < iterations; ++i) {
        auto now = DateTime::current();
        auto formatted = now->toString();
    }
    double totalTime = timer.elapsedMilliseconds();
    
    std::cout << "Performed " << iterations << " current time operations in " << totalTime << "ms" << std::endl;
    std::cout << "Average time per operation: " << (totalTime / iterations) << "ms" << std::endl;
    
    // Less than 5ms per operation
    EXPECT_LT(totalTime / iterations, 5.0);
}

// Performance test for millisecond operations
TEST(PerformanceTest, MillisecondOperations) {
    constexpr int iterations = 10000;
    
    // Creating DateTime objects with milliseconds
    Timer msCreateTimer;
    for (int i = 0; i < iterations; ++i) {
        DateTime dt(2023, 1, 1, 12, 0, 0, i % 1000);
    }
    double msCreateTime = msCreateTimer.elapsedMilliseconds();
    std::cout << "Created " << iterations << " DateTime objects with milliseconds in " 
              << msCreateTime << "ms" << std::endl;
    
    // Millisecond arithmetic performance
    DateTime dt(2023, 1, 1);
    Timer msOpTimer;
    for (int i = 0; i < iterations; ++i) {
        dt.plusMilliseconds(i);
    }
    double msOpTime = msOpTimer.elapsedMilliseconds();
    std::cout << "Performed " << iterations << " millisecond operations in " 
              << msOpTime << "ms" << std::endl;
    
    // Verify performance is acceptable
    EXPECT_LT(msCreateTime / iterations, 1.0);  // Less than 1ms per object
    EXPECT_LT(msOpTime / iterations, 0.1);      // Less than 0.1ms per operation
}
