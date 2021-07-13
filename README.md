# Prerequisites #
- [cxxopts](https://github.com/jarro2783/cxxopts)
- [libosmium](https://github.com/osmcode/libosmium)
- [spdlog](https://github.com/gabime/spdlog)
```
  $ git clone https://github.com/gabime/spdlog.git
  $ cd spdlog && mkdir build && cd build
  $ cmake .. && make -j
  $ sudo make install
 ```
- [protobufer](https://developers.google.com/protocol-buffers)
```
  apt install -y protobuf-compiler
  protoc -I=. --cpp_out=. ./status.proto`
 ```

# Arguments
- **-m | --map <file path>** full path to .osm file containing map
- **-r | --route <route name>** - name of route that will be used for car
- **-v** logs will be printed to console
- **-l | --log <path>** logs will be saved to provided path, default is .\/
- **-i | --ip <IPv4>** IPv4 of ba daemon
- **-p | --port <port number>** ba daemon port
- **-h | --help** print help
- **-c | --cruise** vehicle will keep driving without any orders to fill
- **-w | --wait <time in seconds>** how many seconds will car wait in stop, default is 10s