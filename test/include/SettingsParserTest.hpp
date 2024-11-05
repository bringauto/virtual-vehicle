#pragma once

#include <bringauto/settings/SettingsParser.hpp>
#include <ConfigMock.hpp>

#include <gtest/gtest.h>

#include <iostream>
#include <fstream>



class SettingsParserTest: public ::testing::Test {
protected:
	void SetUp() override {
        configFile.open("test_config.json");
	}

	void TearDown() override {
		std::remove("test_config.json");
	}

	bool parseConfig(ConfigMock::Config config, std::vector<std::string> cmdArgs = {}) {
		ConfigMock configMock(config);
		configFile << configMock.getConfigString();
		configFile.close();

		std::vector<std::string> args = { "vvutests", "--config=test_config.json" };
		args.insert(args.end(), cmdArgs.begin(), cmdArgs.end());

		std::vector<char*> argv;
		for(auto& arg: args) {
			argv.push_back(arg.data());
		}
		return settingsParser.parseSettings(static_cast<int>(argv.size()), argv.data());
	}

	bringauto::settings::SettingsParser settingsParser;
	std::ofstream configFile;
};