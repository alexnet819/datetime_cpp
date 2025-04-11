cmake_minimum_required(VERSION 3.16)
project(datetime_examples)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Execute only if datetime has been built
if(TARGET datetime)
    # Sample code
    add_executable(datetime_example datetime_example.cpp)
    add_executable(timezone_example timezone_example.cpp)
    add_executable(format_example datetime_format_example.cpp)

    # Set include directories
    target_include_directories(datetime_example PRIVATE ${CMAKE_SOURCE_DIR}/datetime/inc)
    target_include_directories(timezone_example PRIVATE ${CMAKE_SOURCE_DIR}/datetime/inc)
    target_include_directories(format_example PRIVATE ${CMAKE_SOURCE_DIR}/datetime/inc)

    # Link libraries
    target_link_libraries(datetime_example datetime)
    target_link_libraries(timezone_example datetime)
    target_link_libraries(format_example datetime)
endif()
