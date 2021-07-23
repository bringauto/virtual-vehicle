CMLIB_DEPENDENCY(
        URI "https://github.com/bringauto/cxxopts-package.git"
        URI_TYPE GIT
        GIT_TAG v2.2.1
        TYPE MODULE
)
FIND_PACKAGE(cxxopts_package REQUIRED)

CMLIB_DEPENDENCY(
        URI "https://github.com/bringauto/spdlog-package.git"
        URI_TYPE GIT
        GIT_TAG v1.8.5
        TYPE MODULE
)
FIND_PACKAGE(spdlog_package REQUIRED)

CMLIB_DEPENDENCY(
        URI "https://github.com/bringauto/protobuf-package.git"
        URI_TYPE GIT
        GIT_TAG v3.17.3
        TYPE MODULE
)
FIND_PACKAGE(protobuf_package REQUIRED)

#[[CMLIB_DEPENDENCY(
        URI "https://github.com/bringauto/boost-package.git"
        URI_TYPE GIT
        GIT_TAG v1.76.0
        TYPE MODULE
)
FIND_PACKAGE(boost REQUIRED)]]
