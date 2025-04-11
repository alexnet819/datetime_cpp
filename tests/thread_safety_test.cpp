#include "datetime.hpp"
#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <atomic>
#include <mutex>

// Test calling DateTime::current from multiple threads
TEST(ThreadSafetyTest, ConcurrentDateTimeCreation) {
    std::atomic<int> successCount(0);
    std::vector<std::thread> threads;
    
    // Create 10 threads
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&successCount]() {
            try {
                // Call DateTime::current 100 times in each thread
                for (int j = 0; j < 100; ++j) {
                    auto dt = DateTime::current();
                    auto formatted = dt->toString();
                    if (!formatted.empty()) {
                        successCount++;
                    }
                }
            } catch (const std::exception& e) {
                // Fail the test if an exception occurs
                ADD_FAILURE() << "Exception caught: " << e.what();
            }
        });
    }
    
    // Wait for all threads to finish
    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }
    
    // Verify all calls succeeded
    EXPECT_EQ(successCount, 10 * 100);
}

// Test manipulating the same DateTime instance from multiple threads
TEST(ThreadSafetyTest, ConcurrentDateTimeManipulation) {
    std::vector<std::thread> threads;
    std::mutex resultMutex;
    std::vector<DateTime> results;
    
    // Shared DateTime instance
    DateTime sharedDateTime(2023, 1, 1);
    
    // Create 10 threads
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([i, &sharedDateTime, &resultMutex, &results]() {
            // Each thread performs a different operation
            DateTime localCopy = sharedDateTime;
            DateTime modified;
            
            switch (i % 5) {
                case 0:
                    modified = localCopy.plusDays(i);
                    break;
                case 1:
                    modified = localCopy.plusMonths(i);
                    break;
                case 2:
                    modified = localCopy.plusYears(i);
                    break;
                case 3:
                    modified = localCopy.plusHours(i * 24);
                    break;
                case 4:
                    modified = localCopy.convertToRegion(DateTime::JapanTime);
                    break;
            }
            
            // Safely store the result
            std::lock_guard<std::mutex> lock(resultMutex);
            results.push_back(modified);
        });
    }
    
    // Wait for all threads to finish
    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }
    
    // Check the size of results array
    EXPECT_EQ(results.size(), 10);
    
    // Verify the original DateTime instance is unchanged
    EXPECT_EQ(sharedDateTime.getYear(), 2023);
    EXPECT_EQ(sharedDateTime.getMonth(), 1);
    EXPECT_EQ(sharedDateTime.getDay(), 1);
}
