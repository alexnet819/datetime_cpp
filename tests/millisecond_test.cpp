#include "datetime.hpp"
#include <gtest/gtest.h>
#include <memory>

TEST(MillisecondTest, ConstructorWithMilliseconds) {
    // Test constructor with milliseconds
    DateTime dt(2023, 5, 10, 15, 30, 45, 789);
    EXPECT_EQ(dt.getYear(), 2023);
    EXPECT_EQ(dt.getMonth(), 5);
    EXPECT_EQ(dt.getDay(), 10);
    EXPECT_EQ(dt.getHour(), 15);
    EXPECT_EQ(dt.getMinute(), 30);
    EXPECT_EQ(dt.getSecond(), 45);
    EXPECT_EQ(dt.getMillisecond(), 789);
}

TEST(MillisecondTest, GetSetMilliseconds) {
    // Test setting milliseconds
    DateTime dt;
    dt.setTime(12, 34, 56, 321);
    
    EXPECT_EQ(dt.getHour(), 12);
    EXPECT_EQ(dt.getMinute(), 34);
    EXPECT_EQ(dt.getSecond(), 56);
    EXPECT_EQ(dt.getMillisecond(), 321);
    
    // Test invalid milliseconds
    EXPECT_THROW(dt.setTime(12, 34, 56, 1000), DateTimeException);
    EXPECT_THROW(dt.setTime(12, 34, 56, -1), DateTimeException);
}

TEST(MillisecondTest, MillisecondArithmetic) {
    // Test plusMilliseconds
    DateTime dt(2023, 1, 1, 0, 0, 0, 0);
    
    // Add milliseconds
    auto dt1 = dt.plusMilliseconds(500);
    EXPECT_EQ(dt1.getMillisecond(), 500);
    EXPECT_EQ(dt1.getSecond(), 0);
    
    // Add enough milliseconds to roll over to next second
    auto dt2 = dt.plusMilliseconds(1234);
    EXPECT_EQ(dt2.getSecond(), 1);
    EXPECT_EQ(dt2.getMillisecond(), 234);
    
    // Add negative milliseconds
    auto dt3 = dt.plusMilliseconds(-1);
    EXPECT_EQ(dt3.getDay(), 31);
    EXPECT_EQ(dt3.getMonth(), 12);
    EXPECT_EQ(dt3.getYear(), 2022);
    EXPECT_EQ(dt3.getHour(), 23);
    EXPECT_EQ(dt3.getMinute(), 59);
    EXPECT_EQ(dt3.getSecond(), 59);
    EXPECT_EQ(dt3.getMillisecond(), 999);
}

TEST(MillisecondTest, TimezoneConversionWithMilliseconds) {
    // Test timezone conversion preserves milliseconds
    DateTime dt(2023, 1, 1, 12, 0, 0, 500, DateTime::WorldTime);
    auto converted = dt.convertToRegion(DateTime::JapanTime);
    
    EXPECT_EQ(converted.getHour(), 21); // UTC+9
    EXPECT_EQ(converted.getMillisecond(), 500); // Should preserve milliseconds
    
    // Test preservation through multiple conversions
    auto backToUtc = converted.convertToRegion(DateTime::WorldTime);
    EXPECT_EQ(backToUtc.getHour(), 12);
    EXPECT_EQ(backToUtc.getMillisecond(), 500);
}

TEST(MillisecondTest, MillisecondComparisons) {
    // Test that milliseconds are considered in comparisons
    DateTime dt1(2023, 1, 1, 12, 0, 0, 0);
    DateTime dt2(2023, 1, 1, 12, 0, 0, 1);
    
    EXPECT_TRUE(dt1 < dt2);
    EXPECT_FALSE(dt1 == dt2);
    EXPECT_TRUE(dt1 != dt2);
    
    // Equality should require same milliseconds
    DateTime dt3(2023, 1, 1, 12, 0, 0, 0);
    EXPECT_TRUE(dt1 == dt3);
}
