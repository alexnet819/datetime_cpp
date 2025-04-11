#include "datetime.hpp"
#include <gtest/gtest.h>
#include <memory>

TEST(FormatTest, BasicFormatting) {
    DateTime dt(2023, 10, 15, 14, 30, 45);
    
    // Standard format
    EXPECT_EQ(dt.toString(), "2023-10-15 14:30:45");
    
    // Custom format
    EXPECT_EQ(dt.toString("%Y/%m/%d"), "2023/10/15");
    EXPECT_EQ(dt.toString("%H:%M"), "14:30");
    EXPECT_EQ(dt.toString("%Y-%m-%d %H:%M:%S"), "2023-10-15 14:30:45");
    
    // Format with region information
    DateTime jst(2023, 10, 15, 14, 30, 45, 0, DateTime::JapanTime);
    std::string jstString = jst.toStringWithRegion();
    EXPECT_TRUE(jstString.find("JST") != std::string::npos);
}

TEST(FormatTest, FormatString) {
    DateTime dt(2023, 10, 15, 14, 30, 45);
    
    // Format using C++20 format functionality
    EXPECT_EQ(dt.formatString("{:%Y-%m-%d}"), "2023-10-15");
    EXPECT_EQ(dt.formatString("{:%H:%M:%S}"), "14:30:45");
    EXPECT_EQ(dt.formatString("{:%Y-%m-%d %H:%M:%S}"), "2023-10-15 14:30:45");
}

TEST(FormatTest, ParseDateTime) {
    // Parse from string
    auto dt1 = DateTime::parse("2023-10-15 14:30:45");
    ASSERT_TRUE(dt1.has_value());
    EXPECT_EQ(dt1->getYear(), 2023);
    EXPECT_EQ(dt1->getMonth(), 10);
    EXPECT_EQ(dt1->getDay(), 15);
    EXPECT_EQ(dt1->getHour(), 14);
    EXPECT_EQ(dt1->getMinute(), 30);
    EXPECT_EQ(dt1->getSecond(), 45);
    
    // Parse with custom format
    auto dt2 = DateTime::parse("2023/10/15 14:30", "%Y/%m/%d %H:%M");
    ASSERT_TRUE(dt2.has_value());
    EXPECT_EQ(dt2->getYear(), 2023);
    EXPECT_EQ(dt2->getMonth(), 10);
    EXPECT_EQ(dt2->getDay(), 15);
    EXPECT_EQ(dt2->getHour(), 14);
    EXPECT_EQ(dt2->getMinute(), 30);
    EXPECT_EQ(dt2->getSecond(), 0);  // Second not specified, so default value
    
    // Invalid format
    auto dt3 = DateTime::parse("invalid-date");
    EXPECT_FALSE(dt3.has_value());
    
    // Invalid date
    auto dt4 = DateTime::parse("2023-02-30 12:00:00");
    EXPECT_FALSE(dt4.has_value());
}

TEST(FormatTest, FormatWithLocale) {
    DateTime dt(2023, 10, 15, 14, 30, 45);
    
    // Locale-dependent format for day of week and month name
    std::string dayName = dt.toString("%A");  // Day of week (locale dependent)
    std::string monthName = dt.toString("%B"); // Month name (locale dependent)
    
    // Just check they're not empty since results depend on locale
    EXPECT_FALSE(dayName.empty());
    EXPECT_FALSE(monthName.empty());
}

TEST(FormatTest, TimeZoneFormatting) {
    // Format in different time zones
    DateTime utc(2023, 10, 15, 12, 0, 0, 0, DateTime::WorldTime);
    DateTime jst(2023, 10, 15, 21, 0, 0, 0, DateTime::JapanTime);
    
    // Display with UTC+JST timezone
    std::string utcTZFormat = utc.toStringWithRegion("%Y-%m-%d %H:%M:%S %Z");
    std::string jstTZFormat = jst.toStringWithRegion("%Y-%m-%d %H:%M:%S %Z");
    
    EXPECT_TRUE(utcTZFormat.find("UTC") != std::string::npos);
    EXPECT_TRUE(jstTZFormat.find("JST") != std::string::npos);
    
    // Verify they represent the same moment (UTC 12:00 = JST 21:00)
    auto utc_as_jst = utc.convertToRegion(DateTime::JapanTime);
    EXPECT_EQ(utc_as_jst.getHour(), 21);
    EXPECT_EQ(utc_as_jst.toString("%H:%M"), jst.toString("%H:%M"));
}

TEST(FormatTest, ErrorHandlingInFormat) {
    DateTime dt(2023, 10, 15, 14, 30, 45);
    
    // Verify no crash with invalid format specification
    std::string result = dt.toString("invalid-format%Ztest");
    EXPECT_FALSE(result.empty());
    
    // Invalid format string with C++20 format
    std::string formatResult = dt.formatString("invalid-format");
    EXPECT_FALSE(formatResult.empty());
}
