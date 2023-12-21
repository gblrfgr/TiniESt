# Minimum required version of CMake
cmake_minimum_required(VERSION 3.12)

# Project name
project(TiniESt)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add source files
set(SOURCES "tiniest/lex.cpp")

Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.4.0
)

FetchContent_MakeAvailable(Catch2)

set(TEST_SOURCES "tiniest/lex.test.cpp")

add_executable(tests ${TEST_SOURCES} ${SOURCES})
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)
catch_discover_tests(tests)

# Add library target
include_directories(${PROJECT_SOURCE_DIR})
add_library(${PROJECT_NAME} ${SOURCES})

# Set library properties
target_include_directories(${PROJECT_NAME} PUBLIC include)

# Install library
install(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
)

# Install library headers
install(DIRECTORY include/ DESTINATION include/${PROJECT_NAME})
