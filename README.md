# Prerequisites #
- [cxxopts](https://github.com/jarro2783/cxxopts)
- [libosmium](https://github.com/osmcode/libosmium)
```
  $ sudo apt install libosmium2-dev
 ```
-[cmlib](https://github.com/cmakelib/cmakelib), need to export CMLIB_DIR

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