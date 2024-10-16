
SET(CMAKE_FIND_USE_CMAKE_SYSTEM_PATH FALSE)

BA_PACKAGE_LIBRARY(cxxopts          v3.0.5  PLATFORM_STRING_MODE any_machine NO_DEBUG ON)
BA_PACKAGE_LIBRARY(ba-logger        v2.0.0)
BA_PACKAGE_LIBRARY(statesmurf       v2.2.0)
BA_PACKAGE_LIBRARY(modbuspp         v0.3.1  PLATFORM_STRING_MODE any_machine NO_DEBUG ON)
BA_PACKAGE_LIBRARY(nlohmann-json    v3.10.5 PLATFORM_STRING_MODE any_machine NO_DEBUG ON)
BA_PACKAGE_LIBRARY(internal-client  v1.1.1)
BA_PACKAGE_LIBRARY(fleet-protocol-interface  v2.0.0 PLATFORM_STRING_MODE any_machine NO_DEBUG ON)
BA_PACKAGE_LIBRARY(boost            v1.86.0)
# All packages needed by libosmium for xml parsing
BA_PACKAGE_LIBRARY(zlib             v1.2.11)
BA_PACKAGE_LIBRARY(bzip2            v1.0.8)
BA_PACKAGE_LIBRARY(expat            v2.4.8)
BA_PACKAGE_LIBRARY(protozero        v1.7.1)
BA_PACKAGE_LIBRARY(osmium           v2.17.3)

IF (BRINGAUTO_TESTS)
    BA_PACKAGE_LIBRARY(gtest         v1.12.1)
ENDIF ()
