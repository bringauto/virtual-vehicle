# StateSmurf tests integration

Smurf integration tests are used to test Virtual vehicle integration with who Fleet Protocol stack using [Etna](https://github.com/bringauto/etna).

The tests are done by comparing state transitions of two different runs of the same scenario.


### Etna

Virtual vehicle must start Etna for each run to refresh the missions sent by
the [virtual fleet management]. This
is driven by `docker compose` in scenarios.json (see the file for more information).

Etna must contain the correct missions in Virtual Fleet Management configuration.

### Usage

When a new mission will be sent and what stops it will contain is set by the [virtual fleet management] scenario.

The map that the car operates on is set by the arguments of VVU.

The project must be configured with CMake option `-DSTATE_SMURF=ON` and build according to
root [README](README.md/build).

To run all scenarios, run the following commands in the project root directory.
The command assumes that StateSmurf is installed in `/usr/local/` and VVU is built in `_build/` directory.

```bash
mkdir test/smurf/_logs/

find test/smurf/**/scenarios.json > scenario_files

while read line; do
    python3 /usr/local/bin/smurf_compare_scenarios.py --scenario $line --executable _build/virtual-vehicle-utility --evaluator /usr/local/bin/smurf_evaluator
done < scenario_files
```

### Directory structure

For each scenario of the Virtual Fleet Management, there has to be one directory. This directory must contain:

* **scenarios.json** testing scenario, setting the arguments of the VVU
* **.env** file - for docker compose, this file influences the Virtual Fleet Management scenario and config
* **etalons** for given scenario

### Add test

In case a new car mission is to be tested, a scenario must be added to Etna configuration for Virtual Fleet Management.
After setting VFM, create a new directory in `test/smurf/` with .env parameters that will refer to the new mission
scenario.

Once everything is prepared, create etalons. Etalons have to be verified according to Virtual Fleet Management scenario.

#### Implementation details

State diagram is defined
in [bringauto/settings/StateSmurfDefinition.hpp](/include/bringauto/settings/StateSmurfDefinition.hpp)


[virtual fleet management]: https://github.com/bringauto/virtual-fleet-management