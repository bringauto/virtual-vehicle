import argparse
import json
import signal
import subprocess
import sys
import time
from os import path

daemons = []
vehicles = []


class FileDoesntExistException(Exception):
    def __init__(self, message) -> None:
        super().__init__(message)


class PortOutOfRangeException(Exception):
    def __init__(self, message="Ports out of range, it has to be: 1024 < my port < 65535") -> None:
        super().__init__(message)


def parse_arguments():
    parser = argparse.ArgumentParser()
    requiredNamed = parser.add_argument_group('required arguments')
    requiredNamed.add_argument('--deamon_path',
                               required=True,
                               help='path to deamon')
    requiredNamed.add_argument('--virtual_path',
                               required=True,
                               help='path to virtual vehicle')
    requiredNamed.add_argument('--json_path',
                               required=True,
                               help='path to json file')
    requiredNamed.add_argument('-mp', '--min_port', type=int,
                               required=True,
                               help='minimal port for first subprocess')

    return parser.parse_args()


def checkPaths(args):
    if (path.exists(args.deamon_path) == False):
        raise FileDoesntExistException("Deamon file " + args.deamon_path + " doesnt exist")
    if (path.exists(args.virtual_path) == False):
        raise FileDoesntExistException("Virtual Vehicle file " + args.virtual_path + " doesnt exist")
    if (path.exists(args.json_path) == False):
        raise FileDoesntExistException("Json file " + args.json_path + " doesnt exist")


def run_program(args):
    # Opening JSON file
    file = open(args.json_path)
    tokens = json.load(file)['virtualVehicles']
    file.close()
    daemonApp = args.deamon_path
    vehicleApp = args.virtual_path

    for token in tokens:
        if (args.min_port <= 1024) or (args.min_port > 65535):
            raise PortOutOfRangeException()
        daemons.append(subprocess.Popen(
            [daemonApp, '--vehicle-name=' + token['name'], '--vehicle-token=' + token['token'],
             '--port=' + str(args.min_port)]))
        time.sleep(2)
        vehicles.append(subprocess.Popen(
            [vehicleApp, '--map=' + token['mapPath'], '--route=' + token['route'], '--ip=127.0.0.1',
             '--wait=' + str(token['stopWaitTime']), '--cruise' if token['cruise'] == 'TRUE' else '',
             '--port=' + str(args.min_port)]))
        args.min_port += 1
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


if __name__ == '__main__':
    # store the original SIGINT handler
    original_sigint = signal.getsignal(signal.SIGINT)
    signal.signal(signal.SIGINT, exit_gracefully)
    signal.signal(signal.SIGTERM, exit_gracefully)

    args = parse_arguments()
    try:
        checkPaths(args)
        run_program(args)
    except Exception as e:
        print(str(e))
    finally:
        exit_gracefully(None, None)
