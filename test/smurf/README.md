# StateSmurf tests integration
Guide on how [StateSmurf](https://github.com/Melky-Phoe/StateSmurf) is integrated to the Virtual-vehicle utility and how to add a new test scenarios.


### Etna
VVU must start Etna for each run to refresh the missions sent by the [virtual industrial portal](https://gitlab.bringauto.com/bring-auto/host-platform/virtual-industrial-portal). This is driven by docker compose in scenarios.json (see the file for more information).

Etna must be set to run the right virtual industrial portal docker image that contains the correct missions. 


### Usage
When will new mission be sent and what stops it will contain is set by the [virtual industrial portal](https://gitlab.bringauto.com/bring-auto/host-platform/virtual-industrial-portal) scenario.

The route which the car follows is set by the command line arguments of VVU. This can be any route, preferably containing stops with same names as in the VIP scenario.
One mission scenario can be tested on a multiple roads.

Tests are now started with ctest.

### Directory structure
For each scenario of the virtual industrial portal there has to be one directory. This directory must contain:
* **.env** file - for docker-compose, this file influences industrial portal scenario
* **etalons** for given scenario


### Add test
In case a new route is to be tested, add scenario to scenario.json file and save .osm file to directory `test/smurf/maps`.

In case a new car mission is to be tested, a scenario must be added to the virtual industrial portal. This scenario then must be added to docker image that Etna refers to. 
After setting VIP, create a new directory in `test/smurf/` with .env parameters that will refer to the new mission scenario.

Once everything is prepared, create etalons. Etalons have to be verified according to Virtual Industrial portal scenario.


#### Implementation details
State diagram is defined in [bringauto/settings/StateSmurfDefinition.hpp](/include/bringauto/settings/StateSmurfDefinition.hpp)
