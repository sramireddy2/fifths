cmake_minimum_required(VERSION 3.20)
project(fifths VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(fifths src/main.cpp src/audio.cpp)
target_include_directories(fifths PRIVATE src third_party)

if (WIN32)
    target_link_libraries(fifths PRIVATE ole32)
endif()

enable_testing()
add_executable(fifths_tests tests/test_fifths.cpp)
target_include_directories(fifths_tests PRIVATE src)
add_test(NAME fifths_tests COMMAND fifths_tests)
