#include "datetime.hpp"
#include <gtest/gtest.h>
#include <chrono>
#include <thread>

// Leap year special case tests
TEST(EdgeCaseTest, LeapYearTests) {
    // Leap year tests
    EXPECT_TRUE(DateTime::isValidDate(2000, 2, 29));  // Years divisible by 400 are leap years
    EXPECT_TRUE(DateTime::isValidDate(2004, 2, 29));  // Years divisible by 4 are normal leap years
    EXPECT_FALSE(DateTime::isValidDate(1900, 2, 29)); // Years divisible by 100 but not by 400 are not leap years
    EXPECT_FALSE(DateTime::isValidDate(2100, 2, 29)); // Same as above
    
    // Date calculation across leap years
    DateTime feb28_2020(2020, 2, 28);
    auto feb29_2020 = feb28_2020.plusDays(1);
    EXPECT_EQ(feb29_2020.getMonth(), 2);
    EXPECT_EQ(feb29_2020.getDay(), 29);
    
    DateTime feb28_2019(2019, 2, 28);
    auto mar1_2019 = feb28_2019.plusDays(1);
    EXPECT_EQ(mar1_2019.getMonth(), 3);
    EXPECT_EQ(mar1_2019.getDay(), 1);
}

// Month-end handling special case tests
TEST(EdgeCaseTest, MonthEndTests) {
    // From month-end to next month
    DateTime jan31(2023, 1, 31);
    auto feb28 = jan31.plusMonths(1);
    EXPECT_EQ(feb28.getMonth(), 2);
    EXPECT_EQ(feb28.getDay(), 28); // February has 28 days
    
    DateTime jul31(2023, 7, 31);
    auto aug31 = jul31.plusMonths(1);
    EXPECT_EQ(aug31.getMonth(), 8);
    EXPECT_EQ(aug31.getDay(), 31); // August has 31 days
    
    DateTime aug31_2023(2023, 8, 31);
    auto sep30 = aug31_2023.plusMonths(1);
    EXPECT_EQ(sep30.getMonth(), 9);
    EXPECT_EQ(sep30.getDay(), 30); // September has 30 days
}

// Year crossing tests
TEST(EdgeCaseTest, YearCrossingTests) {
    DateTime dec31_2022(2022, 12, 31, 23, 59, 59);
    auto jan1_2023 = dec31_2022.plusSeconds(1);
    
    EXPECT_EQ(jan1_2023.getYear(), 2023);
    EXPECT_EQ(jan1_2023.getMonth(), 1);
    EXPECT_EQ(jan1_2023.getDay(), 1);
    EXPECT_EQ(jan1_2023.getHour(), 0);
    EXPECT_EQ(jan1_2023.getMinute(), 0);
    EXPECT_EQ(jan1_2023.getSecond(), 0);
}

// Time zone date crossing tests
TEST(EdgeCaseTest, TimeZoneDateCrossingTests) {
    // JST 23:55 -> UTC 14:55
    DateTime jst_before_midnight(2023, 1, 1, 23, 55, 0, 0, DateTime::JapanTime);
    auto utc_conversion = jst_before_midnight.convertToRegion(DateTime::WorldTime);
    
    EXPECT_EQ(utc_conversion.getYear(), 2023);
    EXPECT_EQ(utc_conversion.getMonth(), 1);
    EXPECT_EQ(utc_conversion.getDay(), 1);
    EXPECT_EQ(utc_conversion.getHour(), 14);
    EXPECT_EQ(utc_conversion.getMinute(), 55);
    
    // UTC 23:55 -> JST next day 8:55
    DateTime utc_before_midnight(2023, 1, 1, 23, 55, 0, 0, DateTime::WorldTime);
    auto jst_conversion = utc_before_midnight.convertToRegion(DateTime::JapanTime);
    
    EXPECT_EQ(jst_conversion.getYear(), 2023);
    EXPECT_EQ(jst_conversion.getMonth(), 1);
    EXPECT_EQ(jst_conversion.getDay(), 2);
    EXPECT_EQ(jst_conversion.getHour(), 8);
    EXPECT_EQ(jst_conversion.getMinute(), 55);
}

// Millisecond edge cases tests
TEST(EdgeCaseTest, MillisecondEdgeCases) {
    // Test millisecond rollovers
    DateTime dt1(2023, 1, 1, 23, 59, 59, 999);
    auto dt2 = dt1.plusMilliseconds(1);
    
    // Should roll over to next day
    EXPECT_EQ(dt2.getYear(), 2023);
    EXPECT_EQ(dt2.getMonth(), 1);
    EXPECT_EQ(dt2.getDay(), 2);
    EXPECT_EQ(dt2.getHour(), 0);
    EXPECT_EQ(dt2.getMinute(), 0);
    EXPECT_EQ(dt2.getSecond(), 0);
    EXPECT_EQ(dt2.getMillisecond(), 0);
    
    // Test large millisecond values
    DateTime dt3(2023, 1, 1, 0, 0, 0, 0);
    auto dt4 = dt3.plusMilliseconds(3723456); // 1h 2m 3s 456ms
    
    EXPECT_EQ(dt4.getHour(), 1);
    EXPECT_EQ(dt4.getMinute(), 2);
    EXPECT_EQ(dt4.getSecond(), 3);
    EXPECT_EQ(dt4.getMillisecond(), 456);
    
    // Test negative milliseconds
    auto dt5 = dt3.plusMilliseconds(-1500); // -1.5 seconds
    
    EXPECT_EQ(dt5.getDay(), 31);  // Previous day
    EXPECT_EQ(dt5.getMonth(), 12); // Previous month
    EXPECT_EQ(dt5.getYear(), 2022); // Previous year
    EXPECT_EQ(dt5.getHour(), 23);
    EXPECT_EQ(dt5.getMinute(), 59);
    EXPECT_EQ(dt5.getSecond(), 58);
    EXPECT_EQ(dt5.getMillisecond(), 500);
}

// // Tests with extremely large or small values
// TEST(EdgeCaseTest, ExtremeValuesTest) {
//     // Far future date
//     DateTime future(2100, 1, 1);
//     EXPECT_EQ(future.getYear(), 2100);
    
//     // Past date
//     DateTime past(1900, 1, 1);
//     EXPECT_EQ(past.getYear(), 1900);
    
//     // Addition with large values
//     auto farFuture = past.plusYears(1000);
//     EXPECT_EQ(farFuture.getYear(), 2900);
    
//     // Subtraction (negative addition) with large values
//     auto farPast = future.plusYears(-1000);
//     EXPECT_EQ(farPast.getYear(), 1100);
// }
