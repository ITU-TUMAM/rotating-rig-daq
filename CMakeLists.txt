cmake_minimum_required(VERSION 3.14)
project(scanivalve_client
    VERSION 0.1.0
    DESCRIPTION "C++ client for Scanivalve pressure scanners"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

include(cmake/CompilerWarnings.cmake)

# --- Library ---
add_library(scanivalve
    src/scanivalve_client.cpp
    src/tcp_transport.cpp
)
target_include_directories(scanivalve PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
set_project_warnings(scanivalve)
add_library(scanivalve::scanivalve ALIAS scanivalve)

# --- Demo executable ---
add_executable(scan_demo apps/scan_demo.cpp)
target_link_libraries(scan_demo PRIVATE scanivalve::scanivalve)

# --- Tests ---
option(SCANIVALVE_BUILD_TESTS "Build unit tests" ON)
if(SCANIVALVE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
