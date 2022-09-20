#pragma once

#include <filesystem>



namespace bringauto::settings {
/**
 * @brief Fleet protocol connection selection
 */
enum class FleetProvider {
	INVALID = -1,
	PROTOBUF,        /// Communication through protobuf with BA daemon
	NO_CONNECTION    /// Doesn't connect to anything
};

/**
 * @brief Source of GPS location of vehicle selection
 */
enum class VehicleProvider {
	INVALID = -1,
	SIMULATION,        /// location from .osm file and driving simulation
	GPS                /// location from GPS tracker
};

/**
 * @brief GPS tracker device select
 */
enum class GpsProvider {
	INVALID = -1,
	RUTX09,
	UBLOX

};

/**
 * @brief Struct to store settings from cmd arguments
 */
struct Settings {
	bool verbose;                       /// verbose switch, if true, logs will be also printed to console
	std::filesystem::path mapFilePath;  /// path to map file in .osm format
	std::string routeName;              /// name of route that will be used, route have to be present in map file
	std::filesystem::path logPath;      /// path to folder, where logs will be generated
	std::string daemonIpAddress;        /// ip address of module server
	int daemonPort;                     /// port of module server
	uint32_t stopWaitTime;              /// time in seconds, how long will car wait in stop before stopping or moving to other stop
	uint32_t messagePeriodMs;           /// maximal wait time between each message, in milliseconds
	bool speedOverride { false };       /// if true, speed will be override in map
	uint32_t speedOverrideMS;           /// new speed for map

	FleetProvider fleetProvider;		/// fleet communication
	VehicleProvider vehicleProvider;	/// gps location source

	GpsProvider gpsProvider;			/// gps tracker device
	std::string rutxIp;					/// ip address of RUTX gps router
	int rutxPort;						/// port of modbus service
	int rutxSlaveId;					/// modbus service slave ID
};
}