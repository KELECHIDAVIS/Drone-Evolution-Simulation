cmake_minimum_required(VERSION 3.28)
project(DroneEvo LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Enable C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fetch SFML from GitHub
include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM
)
FetchContent_MakeAvailable(SFML)

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

# Define the executable
add_executable(DroneEvo ${SOURCES})

# Include header directory
target_include_directories(DroneEvo PRIVATE include)

# Link SFML
target_link_libraries(DroneEvo PRIVATE SFML::Graphics)

# Link Eigen (header-only) — this exposes include dirs
target_link_libraries(DroneEvo PRIVATE Eigen3::Eigen)


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
    target_include_directories(${test_name} PRIVATE include)
    target_link_libraries(${test_name} PRIVATE Eigen3::Eigen SFML::Graphics)
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()