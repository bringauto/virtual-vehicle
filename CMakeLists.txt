CMAKE_MINIMUM_REQUIRED(VERSION 3.20)
PROJECT(VirtualVehicle)

SET(CMAKE_CXX_STANDARD 17)

INCLUDE(CheckPIESupported)
CHECK_PIE_SUPPORTED()
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

OPTION(BRINGAUTO_TESTS "Disable tests" OFF)
OPTION(BRINGAUTO_PACKAGE "Package creation" OFF)
OPTION(BRINGAUTO_INSTALL "Disable install" OFF)
OPTION(BRINGAUTO_SYSTEM_DEP "Enable system dependencies" OFF)
OPTION(BRINGAUTO_SAMPLES "Enable build of sample app, not used in project" OFF)
OPTION(STATE_SMURF "Enables state smurf" OFF)


IF (NOT BRINGAUTO_SYSTEM_DEP)
    FIND_PACKAGE(CMLIB REQUIRED)
    INCLUDE(cmake/Dependencies.cmake)
ENDIF ()

FIND_PACKAGE(cxxopts 3.0.0 REQUIRED)

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
FIND_PACKAGE(Osmium 2.15.4 REQUIRED COMPONENTS XML)

FIND_PACKAGE(libbringauto_logger 1.0.0 REQUIRED)

IF (NOT OSMIUM_FOUND)
    MESSAGE(WARNING "Libosmium not found!\n")
ENDIF ()

INCLUDE_DIRECTORIES(${OSMIUM_INCLUDE_DIRS})

#------------protobuf----------------#
SET(Protobuf_USE_STATIC_LIBS ON)
FIND_PACKAGE(Protobuf 3.17.3 REQUIRED)
ADD_LIBRARY(protobuff_lib STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/proto/CarStateProtocol.pb.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/proto/CarStateProtocol.pb.h)
TARGET_LINK_LIBRARIES(protobuff_lib PUBLIC protobuf::libprotobuf)
TARGET_INCLUDE_DIRECTORIES(protobuff_lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/libs/proto/")


IF (BRINGAUTO_TESTS)
    ENABLE_TESTING()
    INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/test/CMakeLists.txt)
ENDIF (BRINGAUTO_TESTS)

FILE(GLOB_RECURSE source_files "source/*")
ADD_LIBRARY(bringautolib STATIC ${source_files})
TARGET_INCLUDE_DIRECTORIES(bringautolib PUBLIC "lib/proto/" "include/")
TARGET_LINK_LIBRARIES(bringautolib PUBLIC cxxopts::cxxopts protobuff_lib ${OSMIUM_XML_LIBRARIES} bringauto_logger::bringauto_logger)

IF (STATE_SMURF)
    ADD_SUBDIRECTORY(lib/StateSmurf/)
    ADD_COMPILE_DEFINITIONS(STATE_SMURF)
    TARGET_LINK_LIBRARIES(bringautolib PUBLIC
            diagram_smurf_lib
            transition_smurf_lib)
ENDIF ()

ADD_EXECUTABLE(VirtualVehicle main.cpp)
TARGET_INCLUDE_DIRECTORIES(VirtualVehicle PUBLIC "include/")
TARGET_LINK_LIBRARIES(VirtualVehicle PUBLIC bringautolib)