# Datetime_cpp

A modern C++20 date and time library with timezone support, formatting capabilities, and thread safety.

## Features

- Thread-safe datetime operations
- Timezone support and conversion
- Date and time arithmetic with millisecond precision
- Various formatting options including millisecond support
- C++20 format support
- Leap year handling
- Thread safety tests
- Performance benchmarks

## Requirements

- C++20 compatible compiler
- CMake 3.16 or higher
- Google Test (for running tests)

## Building the Library

### Using CMake

1. Clone the repository:
```bash
git clone https://github.com/yourusername/datetime_cpp.git
cd datetime_cpp
```

2. Create a build directory:
```bash
mkdir build && cd build
```

3. Run CMake and build:
```bash
cmake ..
make
```

### Build Options

- Static library (default)
```bash
cmake ..
```

- Shared library
```bash
cmake -DDATETIME_BUILD_SHARED=ON ..
```

## Running Tests

After building the library:

```bash
cd build
ctest
```

Or run specific test categories:

```bash
./tests_bin/datetime_tests
./tests_bin/performance_tests
```

## Usage Examples

### Basic Usage

```cpp
#include "datetime.hpp"
#include <iostream>

int main() {
    // Get current time
    auto now = DateTime::current();
    std::cout << "Current time: " << now->toString() << std::endl;
    std::cout << "Current time with milliseconds: " << now->toString() << "." << now->getMillisecond() << std::endl;
    
    // Create a specific datetime with milliseconds
    DateTime christmas(2023, 12, 25, 12, 0, 0, 500);
    std::cout << "Christmas (with 500ms): " << christmas.toString() << "." << christmas.getMillisecond() << std::endl;
    
    // Date arithmetic with millisecond precision
    auto plusMs = now->plusMilliseconds(750);
    std::cout << "750ms later: " << plusMs.toString() << "." << plusMs.getMillisecond() << std::endl;
    
    // Comparison
    if (plusMs > *now) {
        std::cout << "The time with added milliseconds is after the original time" << std::endl;
    }
    
    return 0;
}
```

### Timezone Handling

```cpp
#include "datetime.hpp"
#include <iostream>

int main() {
    // Current time in different timezones
    auto utcNow = DateTime::current(DateTime::WorldTime);
    auto jstNow = DateTime::current(DateTime::JapanTime);
    
    std::cout << "UTC time: " << utcNow->toStringWithRegion() << "." << utcNow->getMillisecond() << std::endl;
    std::cout << "Japan time: " << jstNow->toStringWithRegion() << "." << jstNow->getMillisecond() << std::endl;
    
    // Convert between timezones (milliseconds are preserved)
    auto utcToJst = utcNow->convertToRegion(DateTime::JapanTime);
    std::cout << "UTC converted to JST: " << utcToJst.toStringWithRegion() << "." << utcToJst.getMillisecond() << std::endl;
    
    // Create datetime with specific milliseconds in a timezone
    DateTime tokyoEvent(2023, 10, 15, 14, 30, 45, 250, DateTime::JapanTime);
    std::cout << "Tokyo event: " << tokyoEvent.toStringWithRegion() << "." << tokyoEvent.getMillisecond() << std::endl;
    
    return 0;
}
```

### Date Formatting

```cpp
#include "datetime.hpp"
#include <iostream>

int main() {
    DateTime dt(2023, 10, 15, 14, 30, 45, 789);
    
    // Standard format
    std::cout << "Default format: " << dt.toString() << std::endl;
    // Output: 2023-10-15 14:30:45
    
    // With milliseconds
    std::cout << "With milliseconds: " << dt.toString() << "." << dt.getMillisecond() << std::endl;
    // Output: 2023-10-15 14:30:45.789
    
    // Custom format
    std::cout << "Custom format: " << dt.toString("%Y/%m/%d %H:%M") << std::endl;
    // Output: 2023/10/15 14:30
    
    // Custom format with milliseconds
    std::cout << "Custom with ms: " << dt.toString("%Y/%m/%d %H:%M:%S") << "." << dt.getMillisecond() << std::endl;
    // Output: 2023/10/15 14:30:45.789
    
    // C++20 format style
    std::cout << "C++20 format: " << dt.formatString("{:%Y-%m-%d}") << std::endl;
    // Output: 2023-10-15
    
    // With timezone
    std::cout << "With timezone: " << dt.toStringWithRegion() << "." << dt.getMillisecond() << std::endl;
    // Output: 2023-10-15 14:30:45 UTC.789
    
    return 0;
}
```

## API Overview

### Core Classes

- `DateTime`: Main class for date and time operations
- `DateTime::RegionTime`: Represents timezone information

### Key Methods

- Creation: `DateTime()`, `DateTime(year, month, day, ...)`, `DateTime::current()`
- Time access: `getYear()`, `getMonth()`, `getDay()`, `getHour()`, `getMinute()`, `getSecond()`, `getMillisecond()`
- Date arithmetic: `plusYears()`, `plusMonths()`, `plusDays()`, `plusHours()`, `plusMinutes()`, `plusSeconds()`, `plusMilliseconds()`
- Comparison: `operator==`, `operator!=`, `operator<`, `operator>`, ...
- Formatting: `toString()`, `toStringWithRegion()`, `formatString()`
- Timezone: `convertToRegion()`, `getRegion()`, `setRegion()`
- Validation: `isValidDate()`, `isValidTime()`
- Parsing: `parse()`

## License

This library is released under the MIT License. See the LICENSE file for details.