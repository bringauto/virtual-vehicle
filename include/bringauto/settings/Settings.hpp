#pragma once

#include <filesystem>



namespace bringauto::settings {
/**
 * @brief Fleet protocol connection selection
 */
enum class FleetProvider {
	E_BEGIN = 0,
	E_INVALID = E_BEGIN,
	/// Communication through internal client with module gateway
	E_INTERNAL_PROTOCOL,
	/// Doesn't connect to anything, just terminal output
	E_NO_CONNECTION,
	E_END
};

/**
 * @brief Source of the driving simulation of vehicle selection
 */
enum class VehicleProvider {
	E_BEGIN = 0,
	E_INVALID = E_BEGIN,
	/// driving simulation from .osm file and driving simulation
	E_SIMULATION,
	/// driving simulation from GPS tracker
	E_GPS,
	E_END
};

/**
 * @brief GPS tracker device select
 */
enum class GpsProvider {
	E_BEGIN = 0,
	E_INVALID = E_BEGIN,
	/// GPS data from RUTX09 modem
	E_RUTX09,
	/// GPS data from ublox device
	E_UBLOX,
	/// GPS data from .osm file
	E_MAP,
	E_END
};

/**
 * @brief Struct to store settings from cmd arguments
 */
struct Settings {
	/// path to configuration file
	std::filesystem::path config;

	/// Verbose switch, if true, logs will be also printed to console
	bool verbose{false};
	/// Path to map file in .osm format
	std::filesystem::path mapFilePath;
	/// Name of route that will be set on initialization, route have to be present in map file
	std::string routeName;
	/// Path to folder, where logs will be generated
	std::filesystem::path logPath;
	/// Time in seconds, how long will car wait in stop before stopping or moving to other stop
	uint32_t stopWaitTime{5};
	/// Maximal wait time between each message, in milliseconds
	uint32_t messagePeriodMs{1};
	/// If true, speed will be override in map
	bool speedOverride { false };
	/// New speed for map
	uint32_t speedOverrideMS;

	/// Type of fleet communication
	FleetProvider fleetProvider{FleetProvider::E_INVALID};
	/// IP address of module gateway
	std::string moduleGatewayIp;
	/// Port of module gateway service
	int moduleGatewayPort{-1};
	/// Name of device
	std::string deviceName;
	/// Role of device
	std::string deviceRole;
	/// Priority of device
	unsigned int devicePriority{10};
	/// How often we can try to connect to server in seconds
	int reconnectPeriodS{30};


	/// Driving provider
	VehicleProvider vehicleProvider{VehicleProvider::E_INVALID};

	/// GPS coordinates provider
	GpsProvider gpsProvider{GpsProvider::E_INVALID};
	/// IP address of RUTX gps router
	std::string rutxIp;
	/// Port of modbus service
	int rutxPort{-1};
	/// Modbus service slave ID
	int rutxSlaveId{-1};
	/// Distance from the stop enough to be marked as in stop
	int stopRadius{-1};
};
}