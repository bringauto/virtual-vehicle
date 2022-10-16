import argparse
import json
import signal
import sys
import time
import docker
import logging
from os import path

runningContainers = []


class FileDoesntExistException(Exception):
    def __init__(self, message) -> None:
        super().__init__(message)


class PortOutOfRangeException(Exception):
    def __init__(self, message="Ports out of range, it has to be: 1024 < my port < 65535") -> None:
        super().__init__(message)


def parse_arguments():
    parser = argparse.ArgumentParser()
    required_named = parser.add_argument_group('required arguments')
    required_named.add_argument('--json_path',
                                required=True,
                                help='path to json file')
    return parser.parse_args()


def check_paths(arguments):
    if not path.exists(arguments.json_path):
        raise FileDoesntExistException("Json file " + arguments.json_path + " doesnt exist")


def run_program(arguments):
    file = open(arguments.json_path)
    settings = json.load(file)
    file.close()

    client = docker.from_env()
    port = settings["start-port"]
    logging.info("Starting docker containers")

    for vehicle in settings["vehicles"]:
        if (port <= 1024) or (port > 65535):
            raise PortOutOfRangeException()
        start_containers(client, settings, vehicle, port)
        port += 1

    logging.info("Start was successful")

    end = False
    while not end:
        for container in runningContainers:
            container.reload()
            if not container.attrs["State"]["Running"]:
                logging.info("Error, container with id " + container.short_id + " is not running, app will exit,"
                                                                                " container output: " + str(
                    container.logs()))
                stop_containers()
                end = True

        time.sleep(5)


def start_containers(docker_client, settings, vehicle, port):
    daemon_container = docker_client.containers.run(
        settings["daemon-docker-image"] + ":" + settings["daemon-docker-tag"],
        detach=True, network="host", auto_remove=True, entrypoint=[
            "/bringauto-daemon/_build/components/industrial_portal/industrial_portal",
            '--vehicle-name=' + vehicle['name'], '--mode=mqtt', '--company=' + vehicle['company'], '--place=default',
            '--broker-port=' + str(settings["mqtt-port"]), '--broker-ip=' + settings["mqtt-address"],
            '--port=' + str(port)])
    logging.info("Started a daemon docker container with id " + daemon_container.short_id)
    runningContainers.append(daemon_container)

    time.sleep(1)

    vehicle_container = docker_client.containers.run(
        settings["vehicle-docker-image"] + ":" + settings["vehicle-docker-tag"],
        detach=True, network="host", auto_remove=True, entrypoint=[
            "/virtual-vehicle-utility/virtual-vehicle-utility", '--map=' + vehicle['docker-map-path'],
                                                       '--daemon-ip=' + "127.0.0.1",
                                                       '--wait=' + str(vehicle['stop-wait-seconds']),
                                                       '--period=' + str(vehicle['message-period-millisecond']),
                                                       '--daemon-port=' + str(port)])
    logging.info("Started a vehicle docker container with id " + vehicle_container.short_id)
    runningContainers.append(vehicle_container)


def exit_gracefully(signum, frame):
    signal.signal(signal.SIGINT, original_sigint)
    logging.info("Signal received, quitting")
    try:
        stop_containers()
        sys.exit(0)
    except KeyboardInterrupt:
        sys.exit(1)


def stop_containers():
    for index, container in enumerate(runningContainers):
        logging.info("Stopping container with id " + container.short_id + " [" + str(index + 1) + "/"
                     + str(len(runningContainers)) + "]")
        try:
            container.stop()
        except:
            logging.error("Container " + container.short_id + " was already stopped")


if __name__ == '__main__':
    logging.basicConfig(format='[%(asctime)s] [%(levelname)s] %(message)s',
                        level=logging.INFO,
                        datefmt='%Y-%m-%d %H:%M:%S')
    original_sigint = signal.getsignal(signal.SIGINT)
    signal.signal(signal.SIGINT, exit_gracefully)
    signal.signal(signal.SIGTERM, exit_gracefully)

    args = parse_arguments()
    try:
        check_paths(args)
        run_program(args)
    except Exception as e:
        print(str(e))
        exit_gracefully(None, None)
