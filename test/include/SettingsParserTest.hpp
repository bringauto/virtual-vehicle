#pragma once

#include <bringauto/settings/SettingsParser.hpp>
#include <ConfigMock.hpp>

#include <gtest/gtest.h>

#include <iostream>
#include <fstream>



class SettingsParserTest: public ::testing::Test {
protected:
	void SetUp() override {
        configFile.open(CONFIG_PATH);
	}

	void TearDown() override {
		std::remove(CONFIG_PATH);
	}

	bool parseConfig(ConfigMock::Config config, std::vector<std::string> cmdArgs = {}, bool overrideArgs = false) {
		ConfigMock configMock(config);
		configFile << configMock.getConfigString();
		configFile.close();

		std::vector<std::string> args = { "vvutests", "-c", CONFIG_PATH };
		if (overrideArgs) {
			args = cmdArgs;
		} else {
			args.insert(args.end(), cmdArgs.begin(), cmdArgs.end());
		}

		std::vector<char*> argv;
		for(auto& arg: args) {
			argv.push_back(arg.data());
		}
		return settingsParser.parseSettings(static_cast<int>(argv.size()), argv.data());
	}

	constexpr static const char* CONFIG_PATH = "test_config.json";

	bringauto::settings::SettingsParser settingsParser;
	std::ofstream configFile;
};
