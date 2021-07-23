CMAKE_MINIMUM_REQUIRED(VERSION 3.18)
PROJECT(VirtualVehicle)

SET(CMAKE_CXX_STANDARD 17)

OPTION(BRINGAUTO_PACKAGE "Package creation" OFF)
OPTION(BRINGAUTO_INSTALL "Disable install" OFF)

#------------cmlib----------------#
FIND_PACKAGE(CMLIB REQUIRED)
INCLUDE(cmake/Dependencies.cmake)

#------------cxxopts----------------#
FIND_PACKAGE(cxxopts 2.2.0 REQUIRED)

#------------spdlog----------------#
IF (NOT TARGET spdlog)
    # Stand-alone build
    FIND_PACKAGE(spdlog 1.8.5 REQUIRED)
ENDIF ()

#------------osmium----------------#
LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
FIND_PACKAGE(Osmium REQUIRED COMPONENTS XML)

IF (NOT OSMIUM_FOUND)
    MESSAGE(WARNING "Libosmium not found!\n")
ENDIF ()

INCLUDE_DIRECTORIES(${OSMIUM_INCLUDE_DIRS})

#------------protobuf----------------#
FIND_PACKAGE(Protobuf 3.17.3 REQUIRED)
SET(PROTO_DIR "${CMAKE_CURRENT_BINARY_DIR}/proto_compiled/")
SET(ORIG_PROTO_DIR "${CMAKE_CURRENT_LIST_DIR}/proto")

MESSAGE(STATUS "${Protobuf_PROTOC_EXECUTABLE}")

FILE(MAKE_DIRECTORY "${PROTO_DIR}")
EXECUTE_PROCESS(
        COMMAND "${Protobuf_PROTOC_EXECUTABLE}"
        --cpp_out=${PROTO_DIR}  --proto_path=${ORIG_PROTO_DIR} ${CMAKE_CURRENT_LIST_DIR}/proto/BringAutoDaemon.proto
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)

ADD_LIBRARY(protobuff_lib STATIC "${PROTO_DIR}/BringAutoDaemon.pb.cc")
TARGET_LINK_LIBRARIES(protobuff_lib PUBLIC protobuf::libprotobuf)
TARGET_INCLUDE_DIRECTORIES(protobuff_lib PUBLIC "${PROTO_DIR}")

#------------tests----------------------#

OPTION(BRINGAUTO_TESTS "Disable tests" OFF)

IF(BRINGAUTO_TESTS)
    INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt)
ENDIF(BRINGAUTO_TESTS)


#------------binaries-------------------#

FILE(GLOB_RECURSE source_files "src/*")
ADD_LIBRARY(bringautolib STATIC ${source_files})
TARGET_INCLUDE_DIRECTORIES(bringautolib PUBLIC "${PROTO_DIR}" "include/")
TARGET_LINK_LIBRARIES(bringautolib PUBLIC spdlog::spdlog_header_only cxxopts::cxxopts protobuff_lib ${OSMIUM_XML_LIBRARIES})


ADD_EXECUTABLE(VirtualVehicle main.cpp src/bringauto/osm/Point.cpp include/bringauto/osm/Point.hpp src/bringauto/osm/Route.cpp include/bringauto/osm/Route.hpp)
TARGET_INCLUDE_DIRECTORIES(VirtualVehicle PUBLIC "include/")
TARGET_LINK_LIBRARIES(VirtualVehicle PUBLIC bringautolib)






