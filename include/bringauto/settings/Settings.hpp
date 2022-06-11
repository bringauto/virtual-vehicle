#pragma once

#include <filesystem>



namespace bringauto::settings {
class Settings {
public:
	bool verbose;                        ///verbose switch, if true, logs will be also printed to console
	std::filesystem::path mapFilePath;    ///path to map file in .osm format
	std::string routeName;                ///name of route that will be used, route have to be present in map file
	std::filesystem::path logPath;        ///path to folder, where logs will be generated
	std::string ipAddress;                ///ip address of module server
	int port;                            ///port of module server
	uint32_t stopWaitTime;                ///time in seconds, how long will car wait in stop before stopping or moving to other stop
	uint32_t messagePeriodMs;            ///maximal wait time between each message, in milliseconds
	bool speedOverride { false };            ///if true, speed will be override in map
	uint32_t speedOverrideMS;            ///new speed for map
};
}