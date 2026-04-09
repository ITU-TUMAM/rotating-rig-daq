include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.5.3
)
FetchContent_MakeAvailable(Catch2)

add_executable(scanivalve_tests
    test_frame_parsing.cpp
    test_client_commands.cpp
    test_transport_errors.cpp
)
target_link_libraries(scanivalve_tests PRIVATE
    scanivalve::scanivalve
    Catch2::Catch2WithMain
)
target_include_directories(scanivalve_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(Catch)
catch_discover_tests(scanivalve_tests)
