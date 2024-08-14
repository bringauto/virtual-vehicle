CMAKE_MINIMUM_REQUIRED(VERSION 3.23)
PROJECT(virtual-vehicle-utility)

SET(CMAKE_CXX_STANDARD 17)

SET(VIRTUAL_VEHICLE_UTILITY_VERSION 3.2.0)
ADD_DEFINITIONS(-DVIRTUAL_VEHICLE_UTILITY_VERSION="${VIRTUAL_VEHICLE_UTILITY_VERSION}")

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
OPTION(BRINGAUTO_GET_PACKAGES_ONLY "Only download packages for this project" OFF)
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
    IF (BRINGAUTO_GET_PACKAGES_ONLY)
        RETURN()
    ENDIF ()
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
FIND_PACKAGE(internal-client-shared 1.1.1 REQUIRED)
FIND_PACKAGE(fleet-protocol-interface 2.0.0 REQUIRED)
FIND_PACKAGE(Boost REQUIRED COMPONENTS system)
FIND_PACKAGE(Protozero REQUIRED)

FILE(GLOB_RECURSE source_files "source/*")

CMDEF_ADD_EXECUTABLE(
        TARGET virtual-vehicle-utility
        SOURCES main.cpp ${source_files}
        INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include/" ${OSMIUM_INCLUDE_DIRS} ${PROTOZERO_INCLUDE_DIR}
        VERSION ${VIRTUAL_VEHICLE_UTILITY_VERSION}
)
TARGET_LINK_LIBRARIES(virtual-vehicle-utility
        PRIVATE
        bringauto_logger::bringauto_logger
        PUBLIC
        nlohmann_json::nlohmann_json
        cxxopts::cxxopts
        internal-client-shared::internal-client-shared
        fleet-protocol-interface::internal-client-interface
        ${OSMIUM_XML_LIBRARIES}
        modbuspp_lib
        Boost::system
)
IF (STATE_SMURF)
    FIND_PACKAGE(state_smurf 2.0.0 REQUIRED)
    ADD_COMPILE_DEFINITIONS(STATE_SMURF)
    TARGET_LINK_LIBRARIES(virtual-vehicle-utility PUBLIC
            diagram_smurf_lib
            transition_smurf_lib)
ENDIF ()

IF (BRINGAUTO_TESTS)
    ENABLE_TESTING()
    INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/test/CMakeLists.txt)
ENDIF (BRINGAUTO_TESTS)

IF (BRINGAUTO_INSTALL)
    CONFIGURE_FILE(resources/systemd/virtual-vehicle-utility.service.in virtual-vehicle-utility.service @ONLY)
    CMDEF_INSTALL(
            TARGET virtual-vehicle-utility
            NAMESPACE virtual-vehicle-utility::
    )
    BA_PACKAGE_DEPS_IMPORTED(virtual-vehicle-utility)

    INCLUDE(GNUInstallDirs)
    INSTALL(FILES "${CMAKE_CURRENT_LIST_DIR}/resources/config/default.json"
            DESTINATION
            "${CMAKE_INSTALL_SYSCONFDIR}/bringauto/virtual-vehicle-utility/"
            RENAME "config.json"
    )
    INSTALL(FILES "${CMAKE_CURRENT_BINARY_DIR}/virtual-vehicle-utility.service"
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
    INCLUDE(CPack)
ENDIF ()
