cmake_minimum_required(VERSION 3.16)
project(VirtualVehicle)

set(CMAKE_CXX_STANDARD 17)
#------------gtest-------------------#

enable_testing()
file(GLOB_RECURSE source_test_files "${CMAKE_CURRENT_LIST_DIR}/src/*")
add_executable(vvutests ${source_test_files} ${CMAKE_CURRENT_LIST_DIR}/mainTests.cpp)
target_include_directories(vvutests PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include/")
target_link_libraries(vvutests PUBLIC gmock gtest pthread gtest_main bringautolib)
include(GoogleTest)
gtest_discover_tests(vvutests)

add_custom_command(TARGET vvutests PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_LIST_DIR}/maps ${CURRENT_BINARY_DIR}/maps)