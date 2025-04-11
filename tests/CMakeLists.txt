cmake_minimum_required(VERSION 3.16)

# Create test executable
add_executable(datetime_tests
    datetime_test.cpp
    timezone_test.cpp
    format_test.cpp
    edge_case_test.cpp
    thread_safety_test.cpp
    performance_test.cpp
    millisecond_test.cpp
)

# Set include directories
target_include_directories(datetime_tests
    PRIVATE
        ${CMAKE_SOURCE_DIR}/datetime/inc
)

# Link settings
target_link_libraries(datetime_tests
    PRIVATE
        datetime
        GTest::GTest
        GTest::Main
)

# Link thread library (for thread tests)
find_package(Threads REQUIRED)
target_link_libraries(datetime_tests PRIVATE Threads::Threads)

# Register tests
add_test(NAME DatetimeTests COMMAND datetime_tests)

# Make performance tests separately runnable
add_executable(performance_tests performance_test.cpp)
target_include_directories(performance_tests PRIVATE ${CMAKE_SOURCE_DIR}/datetime/inc)
target_link_libraries(performance_tests PRIVATE datetime GTest::GTest GTest::Main)
add_test(NAME PerformanceTests COMMAND performance_tests)
