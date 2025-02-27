#pragma once

#include <bringauto/settings/Settings.hpp>

#include <cxxopts.hpp>
#include <nlohmann/json.hpp>

#include <memory>



namespace bringauto::settings {

/**
 * @brief Class implementing functions for command line arguments parsing using cxxopts
 */
class SettingsParser {

public:
	/**
	 * @brief Method parses commandline arguments into Settings class
	 * Method throws exception if wrong arguments were provided, Settings can be obtained after
	 * calling this method with getSettings() method
	 * @param argc number of commandline arguments
	 * @param argv array of commandline arguments
	 * @return true if successful, false if help was printed and parameters were not provided
	 */
	bool parseSettings(int argc, char **argv);

	std::shared_ptr<Settings> getSettings();

	std::string getFormattedSettings();

private:
	static const std::string CONFIG_PATH;
	static const std::string MODULE_GATEWAY_IP;
	static const std::string MODULE_GATEWAY_PORT;
	static const std::string STATUS_MESSAGE_PERIOD;
	static const std::string VEHICLE_PROVIDER;
	static const std::string FLEET_PROVIDER;
	static const std::string HELP;

	static const std::string OSM_MAP;
	static const std::string OSM_ROUTE;
	static const std::string OSM_STOP_WAIT_TIME;
	static const std::string OSM_SPEED_OVERRIDE;
	static const std::string OSM_SPEED_OVERRIDE_MPS;

	static const std::string GPS_PROVIDER;
	static const std::string RUT_IP;
	static const std::string RUT_PORT;
	static const std::string RUT_SLAVE_ID;
	static const std::string STOP_RADIUS;
	static const std::string IN_STOP_DELAY_S;
	static const std::string DEVICE_NAME;
	static const std::string DEVICE_ROLE;
	static const std::string DEVICE_PRIORITY;
	static const std::string RECONNECT_PERIOD;

	static const std::string GENERAL_SETTINGS;
	static const std::string VEHICLE_SETTINGS;
	static const std::string GPS_SETTINGS;
	static const std::string RUTX_09_SETTINGS;
	static const std::string SIMULATION_SETTINGS;
	static const std::string FLEET_SETTINGS;
	static const std::string INTERNAL_PROTOCOL_SETTINGS;
	static const std::string MAP_SETTINGS;

	static const std::string LOGGING;
	static const std::string LOGGING_CONSOLE;
	static const std::string LOGGING_FILE;
	static const std::string LOG_LEVEL;
	static const std::string LOG_USE;
	static const std::string LOG_PATH;

	cxxopts::ParseResult cmdArguments_;
	std::shared_ptr<Settings> settings_;

	void parseCmdArguments(int argc, char **argv);

	bool areCmdArgumentsCorrect();

	bool areSettingsCorrect();

	void fillSettings();

	void fillGeneralSettings(const nlohmann::json &file);

	void fillLoggingSettings(const nlohmann::json &file);

	void fillVehicleSettings(const nlohmann::json &file);

	void fillFleetSettings(const nlohmann::json &file);

	void fillMapSettings(const nlohmann::json &file);

	void fillGpsSettings(const nlohmann::json &file);

	void fillRutx09Settings(const nlohmann::json &file);

	void fillSimulationSettings(const nlohmann::json &file);

	void fillInternalProtocolSettings(const nlohmann::json &file);

	template<typename T>
	T getAndValidateNonNegative(const nlohmann::json& section, const std::string& key);
};
}