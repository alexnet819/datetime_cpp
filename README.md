# Datetime_cpp

A modern C++20 date and time library with timezone support, formatting capabilities, and thread safety.

## Features

- Thread-safe datetime operations
- Timezone support and conversion
- Date and time arithmetic
- Various formatting options
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
    
    // Create a specific datetime
    DateTime christmas(2023, 12, 25);
    std::cout << "Christmas: " << christmas.toString() << std::endl;
    
    // Date arithmetic
    auto tomorrow = now->plusDays(1);
    std::cout << "Tomorrow: " << tomorrow.toString() << std::endl;
    
    // Comparison
    if (tomorrow > *now) {
        std::cout << "Tomorrow is after today" << std::endl;
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
    
    std::cout << "UTC time: " << utcNow->toStringWithRegion() << std::endl;
    std::cout << "Japan time: " << jstNow->toStringWithRegion() << std::endl;
    
    // Convert between timezones
    auto utcToJst = utcNow->convertToRegion(DateTime::JapanTime);
    std::cout << "UTC converted to JST: " << utcToJst.toStringWithRegion() << std::endl;
    
    return 0;
}
```

### Date Formatting

```cpp
#include "datetime.hpp"
#include <iostream>

int main() {
    DateTime dt(2023, 10, 15, 14, 30, 45);
    
    // Standard format
    std::cout << "Default format: " << dt.toString() << std::endl;
    // Output: 2023-10-15 14:30:45
    
    // Custom format
    std::cout << "Custom format: " << dt.toString("%Y/%m/%d %H:%M") << std::endl;
    // Output: 2023/10/15 14:30
    
    // C++20 format style
    std::cout << "C++20 format: " << dt.formatString("{:%Y-%m-%d}") << std::endl;
    // Output: 2023-10-15
    
    // With timezone
    std::cout << "With timezone: " << dt.toStringWithRegion() << std::endl;
    // Output: 2023-10-15 14:30:45 UTC
    
    return 0;
}
```

## API Overview

### Core Classes

- `DateTime`: Main class for date and time operations
- `DateTime::RegionTime`: Represents timezone information

### Key Methods

- Creation: `DateTime()`, `DateTime(year, month, day, ...)`, `DateTime::current()`
- Time access: `getYear()`, `getMonth()`, `getDay()`, `getHour()`, ...
- Date arithmetic: `plusYears()`, `plusMonths()`, `plusDays()`, `plusHours()`, ...
- Comparison: `operator==`, `operator!=`, `operator<`, `operator>`, ...
- Formatting: `toString()`, `toStringWithRegion()`, `formatString()`
- Timezone: `convertToRegion()`, `getRegion()`, `setRegion()`
- Validation: `isValidDate()`, `isValidTime()`
- Parsing: `parse()`

## License

This library is released under the MIT License. See the LICENSE file for details.