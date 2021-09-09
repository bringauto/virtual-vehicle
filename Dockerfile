FROM eu.gcr.io/bringauto-infrastructure/teamcity-build-images/debian11:latest
WORKDIR /virtual-vehicle-utility
RUN mkdir /virtual-vehicle-utility/tmp
COPY . /virtual-vehicle-utility/tmp

RUN mkdir /virtual-vehicle-utility/tmp/_build && \
    cd /virtual-vehicle-utility/tmp/_build && \
    cmake .. -DCMLIB_DIR=/cmakelib -DCMAKE_BUILD_TYPE=Release -DBRINGAUTO_SYSTEM_DEP=OFF && \
    make -j 8 && \
    mv /virtual-vehicle-utility/tmp/_build/VirtualVehicle /virtual-vehicle-utility/VirtualVehicle && \
    mv /virtual-vehicle-utility/tmp/test/maps /virtual-vehicle-utility/maps && \
    rm -r /virtual-vehicle-utility/tmp

EXPOSE 1536
