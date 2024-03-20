CMAKE_MINIMUM_REQUIRED(VERSION 3.20)
PROJECT(virtual-vehicle-utility)

SET(CMAKE_CXX_STANDARD 17)

SET(VIRTUAL_VEHICLE_UTILITY_VERSION 2.6.3)
ADD_DEFINITIONS( -DVIRTUAL_VEHICLE_UTILITY_VERSION="${VIRTUAL_VEHICLE_UTILITY_VERSION}" )

# for linking .so after make install (actually ba_logger):
SET(CMAKE_SKIP_BUILD_RPATH FALSE)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
SET(CMAKE_INSTALL_RPATH $ORIGIN/../lib/)

SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

INCLUDE(CheckPIESupported)
CHECK_PIE_SUPPORTED()
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

OPTION(BRINGAUTO_TESTS "Disable tests" OFF)
OPTION(BRINGAUTO_PACKAGE "Package creation" OFF)
OPTION(BRINGAUTO_INSTALL "Disable install" OFF)
OPTION(BRINGAUTO_SYSTEM_DEP "Enable system dependencies" OFF)
OPTION(BRINGAUTO_SAMPLES "Enable build of sample app, not used in project" OFF)
OPTION(STATE_SMURF "Enables state smurf" OFF)

IF (BRINGAUTO_PACKAGE)
    IF (NOT BRINGAUTO_INSTALL)
        SET(BRINGAUTO_INSTALL ON CACHE BOOL "Forced install due to BRINGAUTO_PACKAGE=ON" FORCE)
        MESSAGE(WARNING "BRINGAUTO_INSTALL is switched to on because of BRINGAUTO_PACKAGE=ON")
    ENDIF ()
ENDIF ()

FIND_PACKAGE(CMLIB
        COMPONENTS CMDEF CMUTIL STORAGE
        REQUIRED
        )

IF (NOT BRINGAUTO_SYSTEM_DEP)
    INCLUDE(cmake/Dependencies.cmake)
ENDIF ()


LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
FIND_PACKAGE(Osmium 2.15.4 REQUIRED COMPONENTS XML)

IF (NOT OSMIUM_FOUND)
    MESSAGE(WARNING "Libosmium not found!\n")
ENDIF ()

FIND_PACKAGE(cxxopts 3.0.0 REQUIRED)
FIND_PACKAGE(nlohmann_json 3.2.0 REQUIRED)
FIND_PACKAGE(libbringauto_logger 1.2.0 REQUIRED)
FIND_PACKAGE(modbuspp 0.3.1 REQUIRED)

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
TARGET_LINK_LIBRARIES(bringautolib PUBLIC nlohmann_json::nlohmann_json protobuff_lib cxxopts::cxxopts protobuff_lib ${OSMIUM_XML_LIBRARIES} bringauto_logger::bringauto_logger modbuspp_lib)


IF (STATE_SMURF)
    FIND_PACKAGE(state_smurf 2.0.0 REQUIRED)
    ADD_COMPILE_DEFINITIONS(STATE_SMURF)
    TARGET_LINK_LIBRARIES(bringautolib PUBLIC
            diagram_smurf_lib
            transition_smurf_lib)
ENDIF ()


CMDEF_ADD_EXECUTABLE(
        TARGET virtual-vehicle-utility
        SOURCES main.cpp
        VERSION ${VIRTUAL_VEHICLE_UTILITY_VERSION}
)
TARGET_LINK_LIBRARIES(virtual-vehicle-utility PUBLIC bringautolib)

IF (BRINGAUTO_INSTALL)
    INCLUDE(GNUInstallDirs)
	CMDEF_INSTALL(
            TARGET virtual-vehicle-utility
            NAMESPACE virtual-vehicle-utility::
    )
	BA_PACKAGE_DEPS_IMPORTED(virtual-vehicle-utility)
    INSTALL(DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/resources/config/"
            DESTINATION
            "${CMAKE_INSTALL_SYSCONFDIR}/bringauto/virtual-vehicle-utility/config/")
    INSTALL(DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/resources/maps/"
            DESTINATION
            "${CMAKE_INSTALL_SYSCONFDIR}/bringauto/virtual-vehicle-utility/maps/")
    INSTALL(FILES "${CMAKE_CURRENT_LIST_DIR}/resources/systemd/virtual-vehicle-utility.service"
            DESTINATION
            "${CMAKE_INSTALL_SYSCONFDIR}/systemd/system/"
            )
ENDIF ()

IF (BRINGAUTO_PACKAGE)
    CMDEF_PACKAGE(
            MAIN_TARGET virtual-vehicle-utility
            VERSION ${VIRTUAL_VEHICLE_UTILITY_VERSION}
    )
    SET(CPACK_GENERATOR ZIP)
    SET(CPACK_PACKAGE_CONTACT "Bring Auto s.r.o. <maintainers@bringauto.com>")
    SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Bring Auto s.r.o <maintainers@bringauto.com>")
    SET(CPACK_DEBIAN_PACKAGE_GENERATE_SHLIBS ON)
    SET(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
    INCLUDE(CPack)
ENDIF ()