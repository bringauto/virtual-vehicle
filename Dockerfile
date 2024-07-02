FROM bringauto/cpp-build-environment:latest

ARG CMLIB_REQUIRED_ENV_TMP_PATH=/virtual-vehicle-utility/cmlib_cache
WORKDIR /virtual-vehicle-utility/cmlib_build
COPY CMakeLists.txt /virtual-vehicle-utility/CMakeLists.txt
COPY CMLibStorage.cmake /virtual-vehicle-utility/CMLibStorage.cmake
COPY cmake/Dependencies.cmake /virtual-vehicle-utility/cmake/Dependencies.cmake
RUN cmake .. -DCMAKE_BUILD_TYPE=Release -DBRINGAUTO_GET_PACKAGES_ONLY=ON

COPY --chown=bringauto:bringauto . /virtual-vehicle-utility/tmp

WORKDIR /virtual-vehicle-utility/tmp/_build
RUN cmake .. -DCMLIB_DIR=/cmakelib -DCMAKE_BUILD_TYPE=Release -DBRINGAUTO_SYSTEM_DEP=OFF -DBRINGAUTO_INSTALL=ON -DCMAKE_INSTALL_PREFIX=/virtual-vehicle-utility && \
    make -j 8 install && \
    rm -rf /virtual-vehicle-utility/tmp

EXPOSE 1536
