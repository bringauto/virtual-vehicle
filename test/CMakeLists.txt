CMAKE_MINIMUM_REQUIRED(VERSION 3.20)
PROJECT(virtual-vehicle-utility)

SET(CMAKE_CXX_STANDARD 20)
#------------gtest-------------------#
FIND_PACKAGE(GTest REQUIRED)
FILE(GLOB_RECURSE source_test_files "${CMAKE_CURRENT_LIST_DIR}/source/*")
ADD_EXECUTABLE(vvutests ${source_test_files} ${source_files} ${CMAKE_CURRENT_LIST_DIR}/mainTests.cpp)
TARGET_INCLUDE_DIRECTORIES(vvutests PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include/" "${CMAKE_CURRENT_LIST_DIR}/../include/" ${OSMIUM_INCLUDE_DIRS} ${PROTOZERO_INCLUDE_DIR})

TARGET_LINK_LIBRARIES(vvutests
        PUBLIC
        GTest::gtest
        pthread
        protobuff_lib
        bringauto_logger::bringauto_logger
        nlohmann_json::nlohmann_json
        cxxopts::cxxopts
        internal-client-shared::internal-client-shared
        fleet-protocol-interface::internal-client-interface
        ${OSMIUM_XML_LIBRARIES}
        modbuspp_lib
        Boost::system
)

IF (STATE_SMURF)
    TARGET_LINK_LIBRARIES(vvutests PUBLIC
            diagram_smurf_lib
            transition_smurf_lib)
ENDIF ()

INCLUDE(GoogleTest)
GTEST_DISCOVER_TESTS(vvutests)

ADD_CUSTOM_COMMAND(TARGET vvutests PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_LIST_DIR}/maps ${CMAKE_CURRENT_BINARY_DIR}/maps)
