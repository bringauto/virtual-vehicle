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
	cxxopts::ParseResult cmdArguments_;
	std::shared_ptr<Settings> settings_;

	void parseCmdArguments(int argc, char **argv);

	bool areCmdArgumentsCorrect();

	bool areSettingsCorrect();

	void fillSettings();

	void fillGeneralSettings(const nlohmann::json &file);

	void fillVehicleSettings(const nlohmann::json &file);

	void fillFleetSettings(const nlohmann::json &file);

	void fillMapSettings(const nlohmann::json &file);

	void fillGpsSettings(const nlohmann::json &file);

	void fillRutx09Settings(const nlohmann::json &file);

	void fillSimulationSettings(const nlohmann::json &file);

	void fillProtobufSettings(const nlohmann::json &file);
};
}