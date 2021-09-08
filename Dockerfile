FROM debian:bullseye
WORKDIR /virtual-vehicle-utility
RUN mkdir /virtual-vehicle-utility/tmp
COPY . /virtual-vehicle-utility/tmp

RUN apt update && \
    apt install -y \
    gcc-9 \
    g++-9 \
    graphviz \
    libgdal-dev \
    libgeos++-dev \
    liblz4-dev \
    libproj-dev \
    libsparsehash-dev \
    ruby \
    ruby-json \
    zlib1g-dev \
    libosmium2-dev \
    apt-utils \
    autoconf \
    make wget git \
    libssl-dev \
    lsb-release \
    zlib1g-dev \
    libprotozero-dev \
    libbz2-dev \
    libboost-dev \
    libexpat1-dev \
    libbz2-dev \
    nlohmann-json3-dev \
    libcurl4-openssl-dev \
    software-properties-common \
  && rm -rf /var/lib/apt/lists/*

RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-9 60 --slave /usr/bin/g++ g++ /usr/bin/g++-9

RUN wget https://github.com/Kitware/CMake/releases/download/v3.21.2/cmake-3.21.2-linux-x86_64.sh && \
    chmod u+x ./cmake-3.21.2-linux-x86_64.sh && \
    ./cmake-3.21.2-linux-x86_64.sh --skip-license --prefix=/usr/local/ && \
    rm -r /virtual-vehicle-utility/cmake-3.21.2-linux-x86_64.sh

RUN git clone https://github.com/google/googletest.git && \
    cd googletest && \
    mkdir build && cd build && \
    cmake .. -DINSTALL_GTEST=ON  && \
    make -j 8 && \
    make install && \
    rm -r /virtual-vehicle-utility/googletest

RUN git clone https://github.com/protocolbuffers/protobuf.git && \
    cd protobuf && \
    git checkout v3.17.3 && \
    git submodule update --init --recursive && \
    ./autogen.sh && \
    ./configure && \
    make -j 8 && \
    make install && \
    ldconfig && \
    rm -r /virtual-vehicle-utility/protobuf

RUN git clone https://github.com/jarro2783/cxxopts.git && \
    cd cxxopts && \
    cmake . && \
    make -j 8 && \
    make install && \
    rm -r /virtual-vehicle-utility/cxxopts

RUN git clone https://github.com/cmakelib/cmakelib.git /cmakelib

RUN git clone https://github.com/gabime/spdlog.git && \
    cd spdlog && \
    mkdir build && \
    cd build && \
    cmake .. && \
    make -j 8 && \
    make install && \
    rm -r /virtual-vehicle-utility/spdlog

RUN mkdir /virtual-vehicle-utility/tmp/_build && \
    cd /virtual-vehicle-utility/tmp/_build && \
    cmake .. -DCMLIB_DIR=/cmakelib -DCMAKE_BUILD_TYPE=Release -DBRINGAUTO_SYSTEM_DEP=OFF && \
    make -j 8 && \
    mv /virtual-vehicle-utility/tmp/_build/VirtualVehicle /virtual-vehicle-utility/VirtualVehicle && \
    mv /virtual-vehicle-utility/tmp/tests/maps /virtual-vehicle-utility/maps && \
    rm -r /virtual-vehicle-utility/tmp

EXPOSE 1536
