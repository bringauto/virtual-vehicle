#Virtual fleet
Virtual vehicleFleet.py provides ability to simulate car movement in bringauto industrial portal.

#Usage

**You have to provide**

- .osm file with named routes have to be provided
- .VirtualVehicle binary 
- [ba daemon](https://gitlab.bringauto.com/jan.kubalek/bringauto-daemon) binary
- json with tokens and route names, example:
```
{
  "data": {
    "CarQuery": {
      "cars": {
        "nodes": [
          {
            "token": "eyJhbGciOiJIUzI1NiIsInR5cCI",
            "route": "brno_luzanky"
          }
        ]
      }
    }
  }
}
```

**Usage**

Place all provided files and binaries to `VirtualFleet.py` folder, edit pathPrefix variable inside it and run `python3 VirtualFleet.py`. One car will be simulated for each entry in json file.