FROM bringauto/cpp-build-environment:latest
WORKDIR /virtual-vehicle-utility

RUN sudo chown -R bringauto:bringauto /virtual-vehicle-utility
RUN sudo chmod -R 777 /virtual-vehicle-utility

RUN mkdir /virtual-vehicle-utility/tmp
COPY . /virtual-vehicle-utility/tmp

RUN mkdir /virtual-vehicle-utility/tmp/_build && \
    mkdir /virtual-vehicle-utility/cmlib_cache && \
    export CMLIB_REQUIRED_ENV_TMP_PATH=/virtual-vehicle-utility/cmlib_cache && \
    cd /virtual-vehicle-utility/tmp/_build && \
    cmake .. -DCMLIB_DIR=/cmakelib -DCMAKE_BUILD_TYPE=Release -DBRINGAUTO_SYSTEM_DEP=OFF -DBRINGAUTO_INSTALL=ON -DCMAKE_INSTALL_PREFIX=/virtual-vehicle-utility && \
    make install && \
    sudo rm -r /virtual-vehicle-utility/tmp

EXPOSE 1536
