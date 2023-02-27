#pragma once

namespace bringauto::settings {
/// Constant strings for parsing command line arguments

const std::string configPath_ { "config" };
const std::string verbose_ { "verbose" };
const std::string logPath_ { "log-path" };
const std::string daemonIp_ { "daemon-ip" };
const std::string daemonPort_ { "daemon-port" };
const std::string statusMessagePeriod_ { "period" };
const std::string vehicleProvider_ { "vehicle-provider" };
const std::string fleetProvider_ { "fleet-provider" };
const std::string help_ { "help" };

const std::string osmMap_ { "map" };
const std::string osmRoute_ { "default-route" };
const std::string osmStopWaitTime_ { "wait" };
const std::string osmSpeedOverride_ { "speed-override" };

const std::string gpsProvider_ { "gps-provider" };
const std::string rutxIp_ { "rutx-ip" };
const std::string rutxPort_ { "rutx-port" };
const std::string rutxSlaveId_ { "rutx-slave-id" };
const std::string stopRadius_ { "stop-radius" };


/// Additional constant strings for parsing json
const std::string generalSettings_ { "general-settings" };
const std::string vehicleSettings_ { "vehicle-settings" };
const std::string gpsSettings_ { "gps-settings" };
const std::string rutx09Settings_ { "rutx09-settings" };
const std::string simulationSettings_ { "simulation-settings" };
const std::string fleetSettings_ { "fleet-settings" };
const std::string protobufSettings_ { "protobuf-settings" };
const std::string empty_ { "empty" };
const std::string mapSettings_ { "map-settings" };








}