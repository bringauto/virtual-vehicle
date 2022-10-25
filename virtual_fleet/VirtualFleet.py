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
    required_named = parser.add_argument_group('required arguments')
    required_named.add_argument('--daemon_path',
                               required=True,
                               help='path to daemon')
    required_named.add_argument('--virtual_path',
                               required=True,
                               help='path to virtual vehicle')
    required_named.add_argument('--json_path',
                               required=True,
                               help='path to json file')
    required_named.add_argument('-mp', '--min_port', type=int,
                               required=True,
                               help='minimal port for first subprocess')
    required_named.add_argument('--mqtt_address',
                               required=True,
                               help='Ip address of mqtt')

    return parser.parse_args()


def check_paths(arguments):
    if not path.exists(arguments.daemon_path):
        raise FileDoesntExistException("Daemon file " + arguments.daemon_path + " doesnt exist")
    if not path.exists(arguments.virtual_path):
        raise FileDoesntExistException("Virtual Vehicle file " + arguments.virtual_path + " doesnt exist")
    if not path.exists(arguments.json_path):
        raise FileDoesntExistException("Json file " + arguments.json_path + " doesnt exist")


def run_program(arguments):
    # Opening JSON file
    file = open(arguments.json_path)
    tokens = json.load(file)['virtualVehicles']
    file.close()
    daemon_app = arguments.daemon_path
    vehicle_app = arguments.virtual_path

    for token in tokens:
        if (arguments.min_port <= 1024) or (arguments.min_port > 65535):
            raise PortOutOfRangeException()
        daemons.append(subprocess.Popen(
            [daemon_app, '--vehicle-name=' + token['name'], '--mode=mqtt', '--company=' + token['company'], '--place=default',
             '--broker-port=' + str(arguments.min_port), '--broker-ip=' + arguments.mqtt_address,
             '--port=' + str(arguments.min_port)]))
        time.sleep(2)
        vehicles.append(subprocess.Popen(
            [vehicle_app, '--map=' + token['mapPath'], '--daemon-ip=127.0.0.1',
             '--wait=' + str(token['stopWaitTime']), '--period=' + str(token['period']),
             '--daemon-port=' + str(arguments.min_port)]))
        arguments.min_port += 1
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
        check_paths(args)
        run_program(args)
    except Exception as e:
        print(str(e))
    finally:
        exit_gracefully(None, None)
