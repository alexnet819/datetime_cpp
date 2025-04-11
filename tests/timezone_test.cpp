#include "datetime.hpp"
#include <gtest/gtest.h>
#include <memory>

TEST(TimeZoneTest, RegionTimeBasic) {
    // Basic test of region time constantsime constants test
    EXPECT_EQ(DateTime::WorldTime.identifier, "UTC");
    EXPECT_EQ(DateTime::WorldTime.hourOffset, 0);
    
    EXPECT_EQ(DateTime::JapanTime.identifier, "JST");
    EXPECT_EQ(DateTime::JapanTime.hourOffset, 9);
    
    EXPECT_EQ(DateTime::EasternTime.identifier, "EST");
    EXPECT_EQ(DateTime::EasternTime.hourOffset, -5);
    
    EXPECT_EQ(DateTime::PacificTime.identifier, "PST");
    EXPECT_EQ(DateTime::PacificTime.hourOffset, -8);
}

TEST(TimeZoneTest, RegionConversion) {
    // Create UTC datetime
    DateTime utcTime(2023, 1, 1, 0, 0, 0, 0, DateTime::WorldTime);
    
    // Convert to different time zones
    auto jstTime = utcTime.convertToRegion(DateTime::JapanTime);
    EXPECT_EQ(jstTime.getHour(), 9);  // UTC 0:00 = JST 9:00
    EXPECT_EQ(jstTime.getRegion().identifier, "JST");
    
    auto estTime = utcTime.convertToRegion(DateTime::EasternTime);
    EXPECT_EQ(estTime.getHour(), 19);  // UTC 0:00 = Previous day EST 19:00
    EXPECT_EQ(estTime.getDay(), 31);   // Should be previous day
    EXPECT_EQ(estTime.getMonth(), 12);  // Previous month
    EXPECT_EQ(estTime.getYear(), 2022);  // Previous year
    
    // Create JST datetime
    DateTime jstNoon(2023, 1, 1, 12, 0, 0, 0, DateTime::JapanTime);
    
    // Convert to other time zones
    auto jstToUtc = jstNoon.convertToRegion(DateTime::WorldTime);
    EXPECT_EQ(jstToUtc.getHour(), 3);  // JST 12:00 = UTC 3:00
    
    auto jstToEst = jstNoon.convertToRegion(DateTime::EasternTime);
    EXPECT_EQ(jstToEst.getHour(), 22);  // JST 12:00 = Previous day EST 22:00
    EXPECT_EQ(jstToEst.getDay(), 31);
}

TEST(TimeZoneTest, TimeZoneStringFormat) {
    // Create string representation including time zone information
    DateTime jstTime(2023, 1, 1, 12, 0, 0, 0, DateTime::JapanTime);
    
    // Standard format
    std::string stdFormat = jstTime.toString();
    EXPECT_EQ(stdFormat, "2023-01-01 12:00:00");
    
    // Format with region information
    std::string tzFormat = jstTime.toStringWithRegion();
    EXPECT_TRUE(tzFormat.find("JST") != std::string::npos);
    
    // Custom format
    std::string customFormat = jstTime.toStringWithRegion("%Y/%m/%d %H:%M:%S %Z");
    EXPECT_EQ(customFormat, "2023/01/01 12:00:00 JST");
}

TEST(TimeZoneTest, CreateWithDifferentTimeZones) {
    // Create objects with same datetime but different time zones
    DateTime utcNewYear(2023, 1, 1, 0, 0, 0, 0, DateTime::WorldTime);
    DateTime jstNewYear(2023, 1, 1, 0, 0, 0, 0, DateTime::JapanTime);
    
    // The actual times represented are different
    EXPECT_NE(utcNewYear, jstNewYear);
    
    // Convert and compare
    auto jstAsUtc = jstNewYear.convertToRegion(DateTime::WorldTime);
    EXPECT_EQ(jstAsUtc.getHour(), 15);  // JST 0:00 = Previous day UTC 15:00
    EXPECT_EQ(jstAsUtc.getDay(), 31);
    EXPECT_EQ(jstAsUtc.getMonth(), 12);
    EXPECT_EQ(jstAsUtc.getYear(), 2022);
}
