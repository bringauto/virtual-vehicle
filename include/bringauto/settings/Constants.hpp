#pragma once

namespace bringauto::settings {
/// Constant strings for parsing command line arguments

const std::string verbose_ { "verbose" };
const std::string logPath_ { "log-path" };
const std::string daemonIp_ { "daemon-ip" };
const std::string daemonPort_ { "daemon-port" };
const std::string statusMessagePeriod_ { "period" };
const std::string vehicleProvider_ { "vehicle-provider" };
const std::string fleetProvider_ { "fleet-provider" };
const std::string help_ { "help" };

const std::string osmMap_ { "map" };
const std::string osmRoute_ { "route" };
const std::string osmStopWaitTime_ { "wait" };
const std::string osmSpeedOverride_ { "speed-override" };

const std::string gpsProvider_ { "gps-provider" };
const std::string rutxIp_ { "rutx-ip" };
const std::string rutxPort_ { "rutx-port" };
const std::string rutxSlaveId_ { "rutx-slave-id" };
const std::string stopRadius_ { "stop-radius" };
}