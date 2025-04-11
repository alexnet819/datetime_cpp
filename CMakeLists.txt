cmake_minimum_required(VERSION 3.16)
project(datetime_cpp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# サブディレクトリの追加
add_subdirectory(datetime)
add_subdirectory(examples)
# add_subdirectory(tests) を削除

# GoogleTestとGoogleMockのリンク
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# テストの追加
enable_testing()
add_subdirectory(tests build/tests_bin)