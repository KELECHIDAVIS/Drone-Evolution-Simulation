cmake_minimum_required(VERSION 3.28)
project(DroneEvo LANGUAGES CXX)

include(FetchContent)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Enable C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# -------------------------------
# Eigen: prefer system package, fallback to FetchContent
# -------------------------------
# Try to find an installed Eigen first
find_package(Eigen3 3.4 QUIET NO_MODULE)

if (NOT Eigen3_FOUND)
  message(STATUS "Eigen3 not found on the system — fetching via FetchContent")
  FetchContent_Declare(
    eigen
    GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
    GIT_TAG 3.4.0    # or change to the specific tag you want
    GIT_SHALLOW ON
  )
  FetchContent_MakeAvailable(eigen)
  # After adding Eigen via FetchContent, the target is Eigen3::Eigen as well.
endif()

# Grab all source files in src/
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    src/*.cpp
)

# Remove main.cpp from library sources
list(FILTER SOURCES EXCLUDE REGEX ".*main\\.cpp$")

# Create a library from your code (without main)
add_library(DroneEvoLib STATIC ${SOURCES})
target_include_directories(DroneEvoLib PUBLIC include)
target_link_libraries(DroneEvoLib PUBLIC  Eigen3::Eigen)

# Define the main executable (just contains main.cpp)
add_executable(DroneEvo src/main.cpp)
target_link_libraries(DroneEvo PRIVATE DroneEvoLib)

# -------------------------------
# Testing setup 
# -------------------------------
include(CTest) 
enable_testing()

# Grab all test source files in tests/
file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS
    tests/*.cpp
)

# Build a separate executable for each test file
foreach(test_source ${TEST_SOURCES})
    get_filename_component(test_name ${test_source} NAME_WE)
    add_executable(${test_name} ${test_source})
    target_link_libraries(${test_name} PRIVATE DroneEvoLib)
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()