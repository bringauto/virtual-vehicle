import json
import subprocess
import time
import signal
import sys
import os

pathPrefix = "/home/nothrax/Documents/virtual-vehicle-utility/virtual_fleet/"
daemonPath = pathPrefix + "daemon"
vehiclePath = pathPrefix + "VirtualVehicle"
jsonPath = pathPrefix + "virtual_fleet.json"
mapPath = pathPrefix + "virtual_vehicle_map.osm"
daemons = []
vehicles = []

def run_program():
    # Opening JSON file
    file = open(jsonPath)
    tokens = json.load(file)['data']['CarQuery']['cars']['nodes']
    file.close()
    port = 60246

    for token in tokens:
        daemons.append(subprocess.Popen([daemonPath, '--vehicle-token=' + token['token'], '--port=' + str(port)]))
        time.sleep(2)
        vehicles.append(subprocess.Popen(
            [vehiclePath, '--map=' + mapPath, '--route=' + token['route'], '--ip=127.0.0.1', '--port=' + str(port)]))
        port += 1
    vehicles[1].wait()


def exit_gracefully(signum, frame):
    # restore the original signal handler as otherwise evil things will happen
    # in raw_input when CTRL+C is pressed, and our signal handler is not re-entrant
    signal.signal(signal.SIGINT, original_sigint)
    print("Ok ok, quitting")
    try:
        for vehicle in vehicles:
            print("Ok ok, quitting veh")
            vehicle.terminate()
        for daemon in daemons:
            print("Ok ok, quitting dae")
            daemon.terminate()

    except KeyboardInterrupt:
        print("Ok ok, quitting")
        sys.exit(1)

    # restore the exit gracefully handler here
    signal.signal(signal.SIGINT, exit_gracefully)

if __name__ == '__main__':
    # store the original SIGINT handler
    original_sigint = signal.getsignal(signal.SIGINT)
    signal.signal(signal.SIGINT, exit_gracefully)
    run_program()



