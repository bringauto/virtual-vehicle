CMLIB_DEPENDENCY(
        URI "https://github.com/bringauto/cxxopts-package.git"
        URI_TYPE GIT
        GIT_TAG v2.2.1
        TYPE MODULE
)
FIND_PACKAGE(cxxopts_package REQUIRED)

CMLIB_DEPENDENCY(
        URI "https://github.com/bringauto/protobuf-package.git"
        URI_TYPE GIT
        GIT_TAG v3.17.3
        TYPE MODULE
)
FIND_PACKAGE(protobuf_package REQUIRED)

CMLIB_DEPENDENCY(
        URI "https://github.com/bringauto/balogger-package.git"
        URI_TYPE GIT
        GIT_TAG v1.1.0
        TYPE MODULE
)
FIND_PACKAGE(balogger_package REQUIRED)
