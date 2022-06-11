#pragma once

#include <bringauto/settings/Settings.hpp>

#include <cxxopts.hpp>

#include <memory>



namespace bringauto::settings {
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

private:
	cxxopts::ParseResult cmdArguments_;
	std::shared_ptr<Settings> settings_;

	void parseCmdArguments(int argc, char **argv);

	bool areCmdArgumentsCorrect();

	bool areSettingsCorrect();

	void fillSettings();
};
}