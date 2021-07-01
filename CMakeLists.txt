cmake_minimum_required(VERSION 3.16)
project(VirtualVehicle)

set(CMAKE_CXX_STANDARD 17)


#------------spdlog----------------#
if (NOT TARGET spdlog)
    # Stand-alone build
    find_package(spdlog REQUIRED)
endif ()

#------------osmium----------------#
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
find_package(Osmium REQUIRED COMPONENTS XML)

if (NOT OSMIUM_FOUND)
    message(WARNING "Libosmium not found!\n")
endif ()

include_directories(${OSMIUM_INCLUDE_DIRS})

#------------protobuf----------------#
SET(PROTO_DIR "${CMAKE_CURRENT_LIST_DIR}/proto_compiled")
SET(ORIG_PROTO_DIR "${CMAKE_CURRENT_LIST_DIR}/proto")

MESSAGE(STATUS "${Protobuf_PROTOC_EXECUTABLE}")

FILE(MAKE_DIRECTORY "${PROTO_DIR}")
EXECUTE_PROCESS(
        COMMAND "${Protobuf_PROTOC_EXECUTABLE}"
        --cpp_out=${PROTO_DIR}  --proto_path=${ORIG_PROTO_DIR} ${CMAKE_CURRENT_LIST_DIR}/proto/BringAutoDaemon.proto
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)

find_package(Protobuf REQUIRED)

ADD_LIBRARY(protobuff_lib STATIC "${PROTO_DIR}/BringAutoDaemon.pb.cc")
TARGET_LINK_LIBRARIES(protobuff_lib PUBLIC protobuf::libprotobuf)
TARGET_INCLUDE_DIRECTORIES(protobuff_lib PUBLIC "${PROTO_DIR}")

#------------gtest-------------------#


add_executable(tests tests/src/osmiumTest.cpp tests/include/osmiumTest.h tests/src/mainTests.cpp)
target_link_libraries(tests gmock gtest pthread gtest_main bringautolib)

add_custom_command(
        TARGET tests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/tests/maps/luzanky.osm
        ${CMAKE_CURRENT_BINARY_DIR}/luzanky.osm)

#------------binaries-------------------#

file(GLOB_RECURSE source_files "src/*")
add_library(bringautolib STATIC ${source_files})
target_include_directories(bringautolib PUBLIC "${PROTO_DIR}" "include/")
target_link_libraries(bringautolib PUBLIC spdlog::spdlog_header_only protobuff_lib ${OSMIUM_XML_LIBRARIES})


add_executable(VirtualVehicle main.cpp)
target_include_directories(VirtualVehicle PUBLIC "include/")
target_link_libraries(VirtualVehicle PUBLIC bringautolib)


