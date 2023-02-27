# Prerequisites #

* [cxxopts](https://github.com/jarro2783/cxxopts) >= 3.0.0
* [protobuf](https://github.com/protocolbuffers/protobuf/tree/main/src) = 3.17.3
* [balogger](https://github.com/bringauto/ba-logger) >= 1.2.0
* [statesmurf](https://github.com/Melky-Phoe/StateSmurf) >= 2.0.1
* [modbuspp](https://github.com/bringauto/modbuspp) >= 0.3.1

* [libosmium](https://github.com/osmcode/libosmium) >= 2.17.3

```
  $ sudo apt install libosmium2-dev
```

* [cmlib](https://github.com/cmakelib/cmakelib), need to export CMLIB_DIR

# Arguments
* `--config-path <string>` path to json configuration file
* `--map <string>` full path to .osm file containing map
* `--default-route <string>` - name of route that will be set as default
* `-v | --verbose` logs will be printed to console
* `--log-path <path>` logs will be saved to provided path, default is .\/
* `--daemon-ip <string>` IPv4 or hostname of ba daemon
* `--daemon-port <int>` ba daemon port
* `-h | --help` print help
* `--wait=<int>` how many seconds will car wait in stop, default is 10s
* `--period=<int>` maximum time period between two status messages send to daemon
* `--speed-override=<int>` override map speed
* `--fleet-provider=<string>` choose fleet provider, `protobuf` for use of protobuf protocol `empty` for use of dummy connection
* `--vehicle-provider=<string>` choose vehicle provider, `simulation` for use of osm map `gps` for use of gps provider
* `--gps-provider=<string>` choose gps provider, `rutx09` or `ublox`
* `--rutx-ip=<int>` ip address to modbus server on rutx09
* `--rutx-port=<int>` port of modbus server on rutx09
* `--rutx-slave-id=<int>` slave id of modbus server on rutx09
* `--stop-radius=<int>` distance from stop that will be determined as arrival at stop

## Settings

Json file is used for setting for all parameters of program. Those settings can be overwritten by commandline arguments.
examples in ./config/*.json

#Fleet provider
Virtual vehicle provides ability to choose a fleet communication provider with `--fleet-provider=<string>` argument. The optins are:
* `protobuf` - connection to daemon using protobuf will be established, requires `--ip=<string>` and `--port=<int>` arguments to specify daemon connection 
* `empty` - no connection will be established, statuses will be discarded and default command will be returned, for testing purposes

#Vehicle provider
Virtual vehicle provides ability to choose a different simulation implementations with `--vehicle-provider=<string>` argument. The options are:
* `simulation` - car movement will be simulated and position will be based of given map file (.osm format). Required parameters: `--map=<string>`, `--route=<string>`,
  optional arguments: `--speed-override=<int>`, `--wait=<int>`
* `gps` - virtual vehicle will report position based on external gps source, no mission logic is implemented. Required parameters: `--gps-provider=<string>`

#Gps provider
Provider used for gps based position reporting. The options are:
* `rutx09` - position is obtained from RUTX09 router from Teltonika. Required parameters: `--rutx-ip=<string>`, `--rutx-port=<int>`, `rutx-slave-id=<int>`
* `ublox` - position is obtained from ublox device, NOT IMPLEMENTED

# Cmake parameters
* BRINGAUTO_TESTS - if set to ON, tests will be also compiled, tests can be run with command ctest after successful build
* BRINGAUTO_PACKAGE - if set to ON creates package of vvu
* BRINGAUTO_INSTALL - if set to ON enables make install command
* BRINGAUTO_SYSTEM_DEP - if set to ON cmake will use system dependencies instead of cmake
* BRINGAUTO_SAMPLES - if set to ON sample app will be compiled
* STATE_SMURF - enable state smurf compilation
* CMLIB_DIR - specify path to cmake lib

# Build
```
mkdir _build && cd _build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j 8
```
# Tests
Tests are configured by CMake parameter `-DBRINGAUTO_TESTS=ON`
start unit tests:
```asm
cd _build
ctest
```
## Smurf integration tests
### Smurf state diagram

<img src="documentation/vvu_state_graph.png" alt="state graph">

### Start tests
There are three scenarios for VVU StateSmurf integration tests using [Etna](https://github.com/bringauto/etna).

Template run command from project root directory `python3 <smurf_compare_scenarios.py path> --evaluator <smurf_evaluator path> --executable _build/VirtualVehicle --scenario <scenarios.json path`

If StateSmurf is installed on host pc, use following commands:
```
python3 /usr/local/bin/smurf_compare_scenarios.py --scenario test/smurf/circularRouteScenario/scenarios.json --executable _build/virtual-vehicle-utility --evaluator /usr/local/bin/smurf_evaluator
python3 /usr/local/bin/smurf_compare_scenarios.py --scenario test/smurf/linearRouteScenario/scenarios.json --executable _build/virtual-vehicle-utility --evaluator /usr/local/bin/smurf_evaluator
python3 /usr/local/bin/smurf_compare_scenarios.py --scenario test/smurf/duplicitStopScenario/scenarios.json --executable _build/virtual-vehicle-utility --evaluator /usr/local/bin/smurf_evaluator
```

# Map creation

For creating maps we use openstreet map editor. Maps are saved in open street map format (.osm). Each map contains list
of nodes (point on route), list of ways (parts of routes, connecting points) and list of routes (composed of ways). One
.osm file can contain multiple routes and they have to be named. Routes can share points and ways. Point can represent
stop and if it is a stpo, it have to be named.

## Start new map

* Install java openstreet map editor (https://josm.openstreetmap.de/wiki/Cs:WikiStart)
* Under Imagery choose map source to visualize map (b&w open street map recommended)
* Under File create new layer, name it (right side of app), layer represents .osm file

## Route creation

* Choose draw nodes tool(left tool bar) and draw way (list of points connected by line) by adding points, press esc when
  finished, repeat until you have all ways you need (usually one is enough if you do not what to share ways in multiple
  routes)
* With select/move tool select all ways you want to include in route and in relation tab add relation with plus button
* in pop-up add required tags - type=way and name=<route name>
* in same pop-up window add ways (selection tab) to route (members tab) and apply role "way"
* repeat if you want multiple routes, routes can share some parts with each other, that is way they consist of ways. If
  you do not intent to share parts of routes use one way per route

  <img src="documentation/josm_selection.png" alt="pop up image">

## Stop addition

* select point you want to make into stop (yellow dot and stop can be defined only on nodes, not on line between them)
* in tags/membership tab click plus button with selected point and add tag stop=true and name=<stop name> (name has to
  be unique)
* you can also add speed=<target speed in m/s> as tag to ANY point, it will set MAXIMUM speed on that point

## Export map

* Under file save map as <map name>.osm file
* .osm map example:

```
<?xml version='1.0' encoding='UTF-8'?>
<osm version='0.6' generator='JOSM'>
  <node id='-101752' action='modify' visible='true' lat='49.17426200443' lon='16.55092476726' />
  <node id='-101754' action='modify' visible='true' lat='49.17320987972' lon='16.55007718921' />
  <node id='-101755' action='modify' visible='true' lat='49.17327300432' lon='16.55111252725' />
  <node id='-101757' action='modify' visible='true' lat='49.17411470831' lon='16.55023812175' />
  <node id='-101758' action='modify' visible='true' lat='49.17438124381' lon='16.54963730693' />
  <node id='-101916' action='modify' visible='true' lat='49.17337821766' lon='16.55093550146' />
  <node id='-101918' action='modify' visible='true' lat='49.17347290948' lon='16.55081748426' />
  <node id='-101920' action='modify' visible='true' lat='49.17363774295' lon='16.55071556032'>
    <tag k='name' v='zastavka' />
    <tag k='stop' v='true' />
  </node>
  <node id='-101922' action='modify' visible='true' lat='49.17383063249' lon='16.55066728055' />
  <node id='-101924' action='modify' visible='true' lat='49.17397091532' lon='16.54929398954'>
    <tag k='speed' v='7' />
  </node>
  <node id='-101925' action='modify' visible='true' lat='49.17361670041' lon='16.54920815885' />
  <node id='-101926' action='modify' visible='true' lat='49.17394987292' lon='16.54962658346' />
  <node id='-101927' action='modify' visible='true' lat='49.17355006564' lon='16.54951393068'>
    <tag k='name' v='stopka' />
    <tag k='speed' v='5' />
    <tag k='stop' v='true' />
  </node>
  <node id='-101928' action='modify' visible='true' lat='49.17361582035' lon='16.54921151664' />
  <node id='-101929' action='modify' visible='true' lat='49.17354962397' lon='16.5495055538' />
  <node id='-101962' action='modify' visible='true' lat='49.17397266578' lon='16.54933623902' />
  <node id='-101963' action='modify' visible='true' lat='49.17397617285' lon='16.54929600589' />
  <node id='-101964' action='modify' visible='true' lat='49.17395469207' lon='16.5496272587' />
  <way id='-101782' action='modify' visible='true'>
    <nd ref='-101752' />
    <nd ref='-101922' />
    <nd ref='-101754' />
    <nd ref='-101755' />
    <nd ref='-101916' />
    <nd ref='-101918' />
    <nd ref='-101920' />
    <nd ref='-101757' />
    <nd ref='-101758' />
    <nd ref='-101752' />
  </way>
  <way id='-101939' action='modify' visible='true'>
    <nd ref='-101924' />
    <nd ref='-101925' />
  </way>
  <way id='-101940' action='modify' visible='true'>
    <nd ref='-101926' />
    <nd ref='-101927' />
  </way>
  <way id='-101941' action='modify' visible='true'>
    <nd ref='-101928' />
    <nd ref='-101929' />
  </way>
  <way id='-101951' action='modify' visible='true'>
    <nd ref='-101963' />
    <nd ref='-101964' />
  </way>
  <relation id='-99764' action='modify' visible='true'>
    <member type='way' ref='-101782' role='' />
    <tag k='name' v='nahoda' />
    <tag k='type' v='way' />
  </relation>
  <relation id='-99775' action='modify' visible='true'>
    <member type='way' ref='-101940' role='' />
    <member type='way' ref='-101939' role='' />
    <member type='way' ref='-101941' role='' />
    <member type='way' ref='-101951' role='' />
    <tag k='name' v='route 2' />
    <tag k='type' v='way' />
  </relation>
</osm>
```

## Build and run docker image

Build docker image using

```
docker build --tag virtual-vehicle-utility .
```

Run docker with parameters

```
docker run -ti --rm virtual-vehicle-utility /virtual-vehicle-utility/VirtualVehicle --config-path=<path to json file> --map=<path to map file> -v --route=<route name> --ip=<daemon ip> --port=<daemon port> --wait=<time to wait in stop in sec>
```

Examples:
```
docker run -ti --rm virtual-vehicle-utility /virtual-vehicle-utility/VirtualVehicle --config-path=/virtual-vehicle-utility/config/example.json
```

```
docker run -ti --rm virtual-vehicle-utility /virtual-vehicle-utility/VirtualVehicle --config-path=/virtual-vehicle-utility/config/example.json --map=/virtual-vehicle-utility/tests/maps/BorsodChem.osm -v --route=borsodchem --ip=127.0.0.1 --port=1536 --wait=10 
```
