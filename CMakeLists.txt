cmake_minimum_required(VERSION 2.8)

# ALL OF THE SPECIFIED NAMES IN THIS CMAKE ARE REQUIRED.
# Set the project name variable to assignment3.
# THIS IS REQUIRED
project(hw2)

# Add your CMAKE_C_FLAGS AND CMAKE_CXX_FLAGS here
# Make SURE to use -std=c11 and -std=c++11 where relevant.
# The options specified below are required, but additional options
#  may be used.
set(CMAKE_C_FLAGS "-std=c11 -Wall -Wextra -Wshadow -Werror")
set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra -Wshadow -Werror")

# Add our include directory to CMake's search paths.
# THIS IS REQUIRED
# REFERENCE: https://cmake.org/cmake/help/latest/command/include_directories.html
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Create library from dyn_array so we can use it later.
# REFERENCE: https://cmake.org/cmake/help/latest/command/add_library.html
add_library(dyn_array SHARED src/dyn_array.c)
add_library(process_scheduling SHARED src/process_scheduling.c)


# Compile the analysis executable.
# REFERENCE: https://stackoverflow.com/questions/50301919/cmake-add-executable-in-another-directory
# Looked at tester executable for reference as well
add_executable(${PROJECT_NAME}_analysis src/analysis.c)

# link the dyn_array library we compiled against our analysis executable.
# REFERENCE: https://cmake.org/cmake/help/latest/command/target_link_libraries.html
# Can just use the file we created above and dyn_array as the 2 parameters
target_link_libraries(${PROJECT_NAME}_analysis dyn_array)
target_link_libraries(${PROJECT_NAME}_analysis process_scheduling)


# Compile the the tester executable.

# REFERENCED the CMAKE file in HW1, where add_executable(arrays_test test/tests.cpp src/arrays.c)
# gives us the templace
add_executable(${PROJECT_NAME}_test test/tests.cpp src/process_scheduling.c)

target_compile_definitions(${PROJECT_NAME}_test PRIVATE)

# Link ${PROJECT_NAME}_test with dyn_array and gtest and pthread libraries
target_link_libraries(${PROJECT_NAME}_test gtest pthread dyn_array)
