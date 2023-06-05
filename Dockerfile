FROM eu.gcr.io/bringauto-infrastructure/teamcity-build-images/debian11:latest
USER root
WORKDIR /virtual-vehicle-utility
RUN mkdir /virtual-vehicle-utility/tmp
COPY . /virtual-vehicle-utility/tmp

RUN mkdir /virtual-vehicle-utility/tmp/_build && \
    mkdir /virtual-vehicle-utility/cmlib_cache && \
    export CMLIB_REQUIRED_ENV_TMP_PATH=/virtual-vehicle-utility/cmlib_cache && \
    cd /virtual-vehicle-utility/tmp/_build && \
    cmake .. -DCMLIB_DIR=/cmakelib -DCMAKE_BUILD_TYPE=Release -DBRINGAUTO_SYSTEM_DEP=OFF && \
    make -j 8 && \
    mv /virtual-vehicle-utility/tmp/_build/virtual-vehicle-utility /virtual-vehicle-utility/virtual-vehicle-utility && \
    mv /virtual-vehicle-utility/tmp/test/maps /virtual-vehicle-utility/maps && \
    rm -r /virtual-vehicle-utility/tmp

EXPOSE 1536
