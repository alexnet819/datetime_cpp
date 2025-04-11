#include "datetime.hpp"
#include <gtest/gtest.h>
#include <memory>
#include <chrono>
#include <thread>

// Basic constructor test
TEST(DateTimeTest, BasicConstructor) {
    // Default constructor
    DateTime dt1;
    EXPECT_EQ(dt1.getRegion().identifier, "UTC");

    // Constructor with year, month, day, hour, minute, second
    DateTime dt2(2023, 10, 15, 14, 30, 45);
    EXPECT_EQ(dt2.getYear(), 2023);
    EXPECT_EQ(dt2.getMonth(), 10);
    EXPECT_EQ(dt2.getDay(), 15);
    EXPECT_EQ(dt2.getHour(), 14);
    EXPECT_EQ(dt2.getMinute(), 30);
    EXPECT_EQ(dt2.getSecond(), 45);
    EXPECT_EQ(dt2.getRegion().identifier, "UTC");
    
    // Constructor with region time
    DateTime dt3(2023, 10, 15, 14, 30, 45, 0, DateTime::JapanTime);
    EXPECT_EQ(dt3.getRegion().identifier, "JST");
    EXPECT_EQ(dt3.getHour(), 14);  // 14:00 in Japan time
}

// Current time retrieval test
TEST(DateTimeTest, CurrentTime) {
    auto now1 = DateTime::current();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    auto now2 = DateTime::current();
    
    // Current time should be different (advancing) with each call
    EXPECT_TRUE(*now1 < *now2);
    
    // Get current time with specific region time
    auto utcNow = DateTime::current(DateTime::WorldTime);
    auto jstNow = DateTime::current(DateTime::JapanTime);
    
    // UTC and JST have a 9-hour time difference
    EXPECT_EQ(utcNow->getRegion().identifier, "UTC");
    EXPECT_EQ(jstNow->getRegion().identifier, "JST");
    
    // Confirm they represent the same moment in different time zones
    auto utcToJst = utcNow->convertToRegion(DateTime::JapanTime);
    EXPECT_EQ(utcToJst.getHour(), jstNow->getHour());
}

// Date/time setter test
TEST(DateTimeTest, SetDateTime) {
    DateTime dt;
    dt.setDate(2025, 12, 31);
    dt.setTime(23, 59, 59);
    
    EXPECT_EQ(dt.getYear(), 2025);
    EXPECT_EQ(dt.getMonth(), 12);
    EXPECT_EQ(dt.getDay(), 31);
    EXPECT_EQ(dt.getHour(), 23);
    EXPECT_EQ(dt.getMinute(), 59);
    EXPECT_EQ(dt.getSecond(), 59);
    
    // Invalid dates should cause an error
    EXPECT_THROW(dt.setDate(2025, 13, 31), DateTimeException);
    EXPECT_THROW(dt.setDate(2025, 2, 30), DateTimeException);
    
    // Invalid times should cause an error
    EXPECT_THROW(dt.setTime(24, 0, 0), DateTimeException);
    EXPECT_THROW(dt.setTime(23, 60, 0), DateTimeException);
}

// Date manipulation test
TEST(DateTimeTest, DateTimeManipulation) {
    DateTime dt(2023, 1, 1, 12, 0, 0);
    
    // Date/time addition
    DateTime dt2 = dt.plusYears(1);
    EXPECT_EQ(dt2.getYear(), 2024);
    
    dt2 = dt.plusMonths(1);
    EXPECT_EQ(dt2.getMonth(), 2);
    
    dt2 = dt.plusDays(1);
    EXPECT_EQ(dt2.getDay(), 2);
    
    dt2 = dt.plusHours(1);
    EXPECT_EQ(dt2.getHour(), 13);
    
    dt2 = dt.plusMinutes(30);
    EXPECT_EQ(dt2.getMinute(), 30);
    
    dt2 = dt.plusSeconds(30);
    EXPECT_EQ(dt2.getSecond(), 30);
    
    // Month-end handling test
    DateTime jan31(2023, 1, 31);
    DateTime feb28 = jan31.plusMonths(1);
    EXPECT_EQ(feb28.getMonth(), 2);
    EXPECT_EQ(feb28.getDay(), 28);  // Jan 31 + 1 month = Feb 28
    
    // Leap year test
    DateTime leapFeb29(2024, 2, 29);
    EXPECT_TRUE(DateTime::isValidDate(2024, 2, 29));  // 2024 is a leap year
    EXPECT_FALSE(DateTime::isValidDate(2023, 2, 29));  // 2023 is not a leap year
}

// Comparison operator test
TEST(DateTimeTest, ComparisonOperators) {
    DateTime dt1(2023, 1, 1);
    DateTime dt2(2023, 1, 2);
    DateTime dt3(2023, 1, 1);
    
    EXPECT_TRUE(dt1 < dt2);
    EXPECT_TRUE(dt2 > dt1);
    EXPECT_TRUE(dt1 <= dt3);
    EXPECT_TRUE(dt1 >= dt3);
    EXPECT_TRUE(dt1 == dt3);
    EXPECT_TRUE(dt1 != dt2);
    
    // Even with different time zones, the same moment should be equivalent
    DateTime utc(2023, 1, 1, 0, 0, 0, 0, DateTime::WorldTime);
    DateTime jst(2023, 1, 1, 9, 0, 0, 0, DateTime::JapanTime);
    
    // 0:00 UTC and 9:00 JST are the same moment
    EXPECT_TRUE(utc.convertToRegion(DateTime::JapanTime).getHour() == 9);
}

// Validation test
TEST(DateTimeTest, Validation) {
    EXPECT_TRUE(DateTime::isValidDate(2023, 1, 31));
    EXPECT_FALSE(DateTime::isValidDate(2023, 2, 29));
    EXPECT_TRUE(DateTime::isValidDate(2024, 2, 29));
    EXPECT_FALSE(DateTime::isValidDate(2023, 4, 31));
    
    EXPECT_TRUE(DateTime::isValidTime(23, 59, 59, 0));
    EXPECT_FALSE(DateTime::isValidTime(24, 0, 0, 0));
    EXPECT_FALSE(DateTime::isValidTime(23, 60, 0, 0));
    EXPECT_FALSE(DateTime::isValidTime(23, 59, 60, 0));
    
    // Add millisecond tests
    EXPECT_TRUE(DateTime::isValidTime(23, 59, 59, 999));
    EXPECT_FALSE(DateTime::isValidTime(23, 59, 59, 1000));
}

// Time interval calculation test
TEST(DateTimeTest, TimeBetween) {
    DateTime dt1(2023, 1, 1, 0, 0, 0);
    DateTime dt2(2023, 1, 1, 1, 0, 0);
    
    // 1 hour = 3600 seconds
    auto diff = DateTime::timeBetween(dt1, dt2);
    EXPECT_EQ(diff.count(), 3600);
    
    // Confirm negative value
    diff = DateTime::timeBetween(dt2, dt1);
    EXPECT_EQ(diff.count(), -3600);
}

// Test for getDayOfWeek
TEST(DateTimeTest, DayOfWeek) {
    DateTime sunday(2023, 1, 1);  // January 1, 2023 was a Sunday
    EXPECT_EQ(sunday.getDayOfWeek(), 0);  // Sunday = 0
    
    DateTime wednesday(2023, 1, 4);  // January 4, 2023 was a Wednesday
    EXPECT_EQ(wednesday.getDayOfWeek(), 3);  // Wednesday = 3
    
    DateTime saturday(2023, 1, 7);  // January 7, 2023 was a Saturday
    EXPECT_EQ(saturday.getDayOfWeek(), 6);  // Saturday = 6
}

// Test for millisecond functionality
TEST(DateTimeTest, MillisecondTest) {
    // Create DateTime with milliseconds
    DateTime dt1(2023, 10, 15, 14, 30, 45, 789);
    EXPECT_EQ(dt1.getMillisecond(), 789);
    
    // Test setTime with milliseconds
    DateTime dt2;
    dt2.setTime(12, 34, 56, 321);
    EXPECT_EQ(dt2.getHour(), 12);
    EXPECT_EQ(dt2.getMinute(), 34);
    EXPECT_EQ(dt2.getSecond(), 56);
    EXPECT_EQ(dt2.getMillisecond(), 321);
    
    // Test plusMilliseconds
    DateTime dt3(2023, 1, 1, 0, 0, 0, 0);
    auto dt4 = dt3.plusMilliseconds(500);
    EXPECT_EQ(dt4.getMillisecond(), 500);
    
    // Test millisecond overflow
    auto dt5 = dt3.plusMilliseconds(1500);
    EXPECT_EQ(dt5.getSecond(), 1);
    EXPECT_EQ(dt5.getMillisecond(), 500);
    
    // Test millisecond validation
    EXPECT_TRUE(DateTime::isValidTime(12, 30, 45, 999));
    EXPECT_FALSE(DateTime::isValidTime(12, 30, 45, 1000));
    EXPECT_FALSE(DateTime::isValidTime(12, 30, 45, -1));
    
    // Exception test for invalid milliseconds
    EXPECT_THROW(dt2.setTime(12, 34, 56, 1000), DateTimeException);
    EXPECT_THROW(dt2.setTime(12, 34, 56, -1), DateTimeException);
}
