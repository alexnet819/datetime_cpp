# CMake file for datetime utility

# Library type option (static or dynamic)
option(DATETIME_BUILD_SHARED "Build datetime as a shared library" OFF)

# Source and header file settings
set(DATETIME_SOURCES 
    src/datetime.cpp
)

set(DATETIME_HEADERS
    inc/datetime.hpp
)

# Create library (static or dynamic)
if(DATETIME_BUILD_SHARED)
    add_library(datetime SHARED ${DATETIME_SOURCES})
    set_target_properties(datetime PROPERTIES 
        POSITION_INDEPENDENT_CODE ON
    )
else()
    add_library(datetime STATIC ${DATETIME_SOURCES})
endif()

# Specify include directories
target_include_directories(datetime
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
        $<INSTALL_INTERFACE:include>
)

# Export settings
target_compile_definitions(datetime
    PUBLIC 
        $<$<BOOL:${DATETIME_BUILD_SHARED}>:DATETIME_SHARED>
)

# Installation settings (information needed for installation in the root project)
set(DATETIME_INSTALL_HEADERS ${DATETIME_HEADERS} PARENT_SCOPE)
set(DATETIME_TARGET datetime PARENT_SCOPE)
