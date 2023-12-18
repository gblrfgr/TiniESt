# Minimum required version of CMake
cmake_minimum_required(VERSION 3.12)

# Project name
project(TiniESt)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add source files
file(GLOB SOURCES
     "tiniest/*.cpp"
)

# Add library target
include_directories(${PROJECT_SOURCE_DIR}/${PROJECT_NAME})
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
