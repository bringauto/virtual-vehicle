# Virtual fleet

Virtual `VirtualFleet.py` provides ability to simulate car movement in bringauto industrial portal. Script will run one
pair of VirtualVehicle and BADaemon for each entry in json and simulate its movement for Industrial portal (IP).


## Docker version
Script [VirtualFleetDocker.py](VirtualFleetDocker.py) uses docker images to run every vehicle. Example settings are present in [json file](virtual_fleet_docker.json)

### Cmd line arguments
* `--json_path=<string>` - path to json configuration file

### JSON configuration
* `daemon-docker-image` - daemon docker image name/address
* `daemon-docker-tag` - docker image tag for daemon
* `vehicle-docker-image` - virtual vehicle docker image name/address
* `vehicle-docker-tag` - virtual vehicle image tag for daemon
* `start-port` - starting port used for communication between vehicle and daemon, port will be iterated, so 8 virtual vehicles will use ports 42 000 - 42 007
* `mqtt-address` - ip address of mqtt broker
* `mqtt-port` - port of mqtt broker
* `vehicles` - array of vehicles to create
* `vehicles.name` - name of the vehicle, used for mqtt topics - `bringauto/default/<name>/<daemon|industrial_portal>`
* `vehicles.docker-map-path` - path to map inside vehicle docker container used for simulation
* `vehicles.route` - name of route used for simulation, route must exist in given map
* `vehicles.stop-wait-seconds` - how many seconds should vehicle wait in stop
* `vehicles.message-period-millisecond` - minimal period of status message

## Non docker version
Script [VirtualFleet.py](VirtualFleet.py) starts binaries at given path. Example settings are present in [json file](virtual_fleet.json)

**You have to provide**

- [Virtual Vehicle](https://gitlab.bringauto.com/bring-auto/host-platform/virtual-vehicle-utility) binary
- [BringAuto daemon](https://gitlab.bringauto.com/jan.kubalek/bringauto-daemon) binary
- json with Virtual Vehicle entries:
    - mapPath - .osm file with named routes have to be provided
    - name of vehicle - has to be unique and same correspond with vehicle in database in IP an token
    - token - unique token, it has to be same with server, is unique for each car
    - route - name of the route in the map file
    - cruise - it defines, if car should ride around, when it has nothing to do
    - stopWaitTime - it defines, how many seconds should car stay on the stop

Example of json file:

```
{
  "virtualVehicles": [
    {
      "name": "BringAuto",
      "mapPath": "/home/zak/Dokumenty/virtual-vehicle-utility/virtual_fleet/virtual_vehicle_map.osm",
      "token": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJDYXJJZCI6IjIiLCJDYXJOYW1lIjoiQnJpbmdBdXRvIiwiaXNzIjoiQnJpbmcuQXV0by5hcnRpbi5jeiIsImF1ZCI6IkJyaW5nLkF1dG8ifQ.6w11EkV3zKvG3Ly_CcF_SPsxSEbVEUOY9u0HvDw_Cr0",
      "route": "brno_luzanky",
      "cruise": "TRUE",
      "stopWaitTime": 5
    }
  ]
}
```

**Usage**

Example run:
`python3 VirtualFleet.py`
`--deamon_path home/zak/Dokumenty/virtual-vehicle-utility/virtual_fleet/BAIndustrialPortal`
`--virtual_path home/zak/Dokumenty/virtual-vehicle-utility/virtual_fleet/VirtualVehicle`
`--json_path home/zak/Dokumenty/virtual-vehicle-utility/virtual_fleet/virtual_fleet.json`
`--min_port 61234`

Parameters:

- --deamon_path PATH = path to daemon binary
- --virtual_path PATH = path to virtual vehicle binary
- --json_path PATH = path to json file
- --min_port PORT, -mp PORT = starting port of the first entry from json file, each next entry will have automatically
  min_port + 1 than the previous one

One car will be simulated for each entry in json file. It will start his own daemon and virtual vehicle on new port.

**Service**

Included files in systemd directory. Before start, it needs change paths to the binary files.