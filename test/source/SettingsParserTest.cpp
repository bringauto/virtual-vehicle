#include <SettingsParserTest.hpp>
#include <bringauto/common_utils/EnumUtils.hpp>

#include <gtest/gtest.h>


/**
 * @brief Test if settings are correctly parsed from the config file
 */
TEST_F(SettingsParserTest, ValidConfig){
	ConfigMock::Config config {};
	auto result = parseConfig(config);
	EXPECT_TRUE(result);

	auto settings = settingsParser.getSettings();
	EXPECT_EQ(settings->messagePeriodMs, config.general_settings.period_ms);

	auto logging = config.logging;
	EXPECT_EQ(settings->loggingSettings.console.use, logging.console.use);
	EXPECT_EQ(bringauto::common_utils::EnumUtils::enumToString(settings->loggingSettings.console.level), logging.console.level);
	EXPECT_EQ(settings->loggingSettings.file.use, logging.file.use);
	EXPECT_EQ(bringauto::common_utils::EnumUtils::enumToString(settings->loggingSettings.file.level), logging.file.level);
	EXPECT_EQ(settings->loggingSettings.file.path, logging.file.path);

	auto gps_settings = config.vehicle_settings.gps_settings;
	auto sim_settings = config.vehicle_settings.simulation_settings;
	EXPECT_EQ(bringauto::common_utils::EnumUtils::enumToString(settings->vehicleProvider), config.vehicle_settings.vehicle_provider);

	// Gps provider values are defaults since the chosen vehicle provider is simulation
	EXPECT_EQ(settings->gpsProvider, bringauto::settings::GpsProvider::E_INVALID);
	EXPECT_EQ(settings->rutxIp, "");
	EXPECT_EQ(settings->rutxPort, 0);
	EXPECT_EQ(settings->rutxSlaveId, -1);
	EXPECT_EQ(settings->stopRadiusM, 0);
	EXPECT_EQ(settings->inStopDelayS.count(), 0);

	EXPECT_EQ(settings->speedOverride, sim_settings.speed_override);
	EXPECT_EQ(settings->speedOverrideMS, sim_settings.speed_override_mps);
	EXPECT_EQ(settings->stopWaitTime, sim_settings.wait_at_stop_s);
	EXPECT_EQ(settings->mapFilePath, sim_settings.map);
	EXPECT_EQ(settings->routeName, sim_settings.default_route);

	auto ip_settings = config.fleet_settings.internal_protocol_settings;
	EXPECT_EQ(bringauto::common_utils::EnumUtils::enumToString(settings->fleetProvider), config.fleet_settings.fleet_provider);
	EXPECT_EQ(settings->moduleGatewayIp, ip_settings.module_gateway_ip);
	EXPECT_EQ(settings->moduleGatewayPort, ip_settings.module_gateway_port);
	EXPECT_EQ(settings->deviceRole, ip_settings.device_role);
	EXPECT_EQ(settings->devicePriority, ip_settings.device_priority);
	EXPECT_EQ(settings->reconnectPeriodS, ip_settings.reconnect_period_s);
	EXPECT_EQ(settings->deviceName, ip_settings.device_name);
}


/**
 * @brief Test if settings are correctly parsed when the vehicle provider is GPS
 */
TEST_F(SettingsParserTest, ValidConfigWithGpsProviderMap){
	ConfigMock::Config config {};
	config.vehicle_settings.vehicle_provider = "GPS";
	auto result = parseConfig(config);
	EXPECT_TRUE(result);

	auto settings = settingsParser.getSettings();
	auto gps_settings = config.vehicle_settings.gps_settings;
	auto sim_settings = config.vehicle_settings.simulation_settings;
	EXPECT_EQ(bringauto::common_utils::EnumUtils::enumToString(settings->vehicleProvider), config.vehicle_settings.vehicle_provider);

	EXPECT_EQ(bringauto::common_utils::EnumUtils::enumToString(settings->gpsProvider), gps_settings.gps_provider);
	EXPECT_EQ(settings->rutxIp, "");
	EXPECT_EQ(settings->rutxPort, 0);
	EXPECT_EQ(settings->rutxSlaveId, -1);
	EXPECT_EQ(settings->stopRadiusM, gps_settings.stop_radius_m);
	EXPECT_EQ(settings->inStopDelayS.count(), gps_settings.in_stop_delay_s);
	EXPECT_EQ(settings->mapFilePath, gps_settings.map_settings.map);
	EXPECT_EQ(settings->routeName, gps_settings.map_settings.default_route);

	// Simulation provider values are defaults since the chosen vehicle provider is gps
	EXPECT_EQ(settings->speedOverride, false);
	EXPECT_EQ(settings->speedOverrideMS, 0);
	EXPECT_EQ(settings->stopWaitTime, 5);
}


/**
 * @brief Test if settings are correctly parsed when the vehicle provider is GPS and gps provider is RUTX09
 */
TEST_F(SettingsParserTest, ValidConfigWithGpsProviderRutx){
	ConfigMock::Config config {};
	config.vehicle_settings.vehicle_provider = "GPS";
	config.vehicle_settings.gps_settings.gps_provider = "RUTX09";
	auto result = parseConfig(config);
	EXPECT_TRUE(result);

	auto settings = settingsParser.getSettings();
	auto gps_settings = config.vehicle_settings.gps_settings;
	auto sim_settings = config.vehicle_settings.simulation_settings;
	EXPECT_EQ(bringauto::common_utils::EnumUtils::enumToString(settings->vehicleProvider), config.vehicle_settings.vehicle_provider);

	EXPECT_EQ(bringauto::common_utils::EnumUtils::enumToString(settings->gpsProvider), gps_settings.gps_provider);
	EXPECT_EQ(settings->rutxIp, gps_settings.rutx09_settings.rutx_ip);
	EXPECT_EQ(settings->rutxPort, gps_settings.rutx09_settings.rutx_port);
	EXPECT_EQ(settings->rutxSlaveId, gps_settings.rutx09_settings.rutx_slave_id);
	EXPECT_EQ(settings->stopRadiusM, gps_settings.stop_radius_m);
	EXPECT_EQ(settings->inStopDelayS.count(), gps_settings.in_stop_delay_s);
	EXPECT_EQ(settings->mapFilePath, "");
	EXPECT_EQ(settings->routeName, "");

	// Simulation provider values are defaults since the chosen vehicle provider is gps
	EXPECT_EQ(settings->speedOverride, false);
	EXPECT_EQ(settings->speedOverrideMS, 0);
	EXPECT_EQ(settings->stopWaitTime, 5);
}


/**
 * @brief Test if settings are correctly parsed from command line arguments
 */
TEST_F(SettingsParserTest, CmdOptionsPrioritySimulation){
	ConfigMock::Config config {};
	auto result = parseConfig(config, {
		"--period-ms=2000",
		"--vehicle-provider-type=simulation",
		"--gps-provider-type=map", // Only for vehicle provider GPS
		"--stop-radius-m=10", // Only for vehicle provider GPS
		"--in-stop-delay-s=20", // Only for vehicle provider GPS
		"--rutx-ip=192.168.10.10", // Only for gps provider RUTX09
		"--rutx-port=1234", // Only for gps provider RUTX09
		"--rutx-slave-id=10", // Only for gps provider RUTX09
		"--speed-override=10", // Only for vehicle provider SIMULATION
		"--wait-at-stop-s=20", // Only for vehicle provider SIMULATION
		"--map=maps/empty.osm", // Only for vehicle provider SIMULATION or gps provider MAP
		"--default-route=route", // Only for vehicle provider SIMULATION or gps provider MAP
		"--fleet-provider-type=internal-protocol",
		"--module-gateway-ip=192.168.20.20",
		"--module-gateway-port=1234"
	});
	EXPECT_TRUE(result);

	auto settings = settingsParser.getSettings();
	EXPECT_EQ(settings->messagePeriodMs, 2000);
	EXPECT_EQ(settings->vehicleProvider, bringauto::settings::VehicleProvider::E_SIMULATION);
	EXPECT_EQ(settings->gpsProvider, bringauto::settings::GpsProvider::E_INVALID);
	EXPECT_EQ(settings->rutxIp, ""); // Default value
	EXPECT_EQ(settings->rutxPort, 0); // Default value
	EXPECT_EQ(settings->rutxSlaveId, -1); // Default value
	EXPECT_EQ(settings->stopRadiusM, 0); // Default value
	EXPECT_EQ(settings->inStopDelayS.count(), 0); // Default value
	EXPECT_EQ(settings->mapFilePath, "maps/empty.osm");
	EXPECT_EQ(settings->routeName, "route");
	EXPECT_EQ(settings->speedOverride, true);
	EXPECT_EQ(settings->speedOverrideMS, 10);
	EXPECT_EQ(settings->stopWaitTime, 20);
	EXPECT_EQ(settings->fleetProvider, bringauto::settings::FleetProvider::E_INTERNAL_PROTOCOL);
	EXPECT_EQ(settings->moduleGatewayIp, "192.168.20.20");
	EXPECT_EQ(settings->moduleGatewayPort, 1234);
}


/**
 * @brief Test if settings are correctly parsed from command line arguments with vehicle provider GPS
 */
TEST_F(SettingsParserTest, CmdOptionsPriorityGpsMap){
	ConfigMock::Config config {};
	auto result = parseConfig(config, {
		"--period-ms=2000",
		"--vehicle-provider-type=gps",
		"--gps-provider-type=map", // Only for vehicle provider GPS
		"--stop-radius-m=10", // Only for vehicle provider GPS
		"--in-stop-delay-s=20", // Only for vehicle provider GPS
		"--rutx-ip=192.168.10.10", // Only for gps provider RUTX09
		"--rutx-port=1234", // Only for gps provider RUTX09
		"--rutx-slave-id=10", // Only for gps provider RUTX09
		"--speed-override=10", // Only for vehicle provider SIMULATION
		"--wait-at-stop-s=20", // Only for vehicle provider SIMULATION
		"--map=maps/empty.osm", // Only for vehicle provider SIMULATION or gps provider MAP
		"--default-route=route", // Only for vehicle provider SIMULATION or gps provider MAP
		"--fleet-provider-type=internal-protocol",
		"--module-gateway-ip=192.168.20.20",
		"--module-gateway-port=1234"
	});
	EXPECT_TRUE(result);

	auto settings = settingsParser.getSettings();
	EXPECT_EQ(settings->messagePeriodMs, 2000);
	EXPECT_EQ(settings->vehicleProvider, bringauto::settings::VehicleProvider::E_GPS);
	EXPECT_EQ(settings->gpsProvider, bringauto::settings::GpsProvider::E_MAP);
	EXPECT_EQ(settings->rutxIp, ""); // Default value
	EXPECT_EQ(settings->rutxPort, 0); // Default value
	EXPECT_EQ(settings->rutxSlaveId, -1); // Default value
	EXPECT_EQ(settings->stopRadiusM, 10);
	EXPECT_EQ(settings->inStopDelayS.count(), 20);
	EXPECT_EQ(settings->mapFilePath, "maps/empty.osm");
	EXPECT_EQ(settings->routeName, "route");
	EXPECT_EQ(settings->speedOverride, false); // Default value
	EXPECT_EQ(settings->speedOverrideMS, 0); // Default value
	EXPECT_EQ(settings->stopWaitTime, 5); // Default value
	EXPECT_EQ(settings->fleetProvider, bringauto::settings::FleetProvider::E_INTERNAL_PROTOCOL);
	EXPECT_EQ(settings->moduleGatewayIp, "192.168.20.20");
	EXPECT_EQ(settings->moduleGatewayPort, 1234);
}


/**
 * @brief Test if settings are correctly parsed from command line arguments with vehicle provider GPS and gps provider RUTX09
 */
TEST_F(SettingsParserTest, CmdOptionsPriorityGpsRutx){
	ConfigMock::Config config {};
	auto result = parseConfig(config, {
		"--period-ms=2000",
		"--vehicle-provider-type=gps",
		"--gps-provider-type=rutx09", // Only for vehicle provider GPS
		"--stop-radius-m=10", // Only for vehicle provider GPS
		"--in-stop-delay-s=20", // Only for vehicle provider GPS
		"--rutx-ip=192.168.10.10", // Only for gps provider RUTX09
		"--rutx-port=1234", // Only for gps provider RUTX09
		"--rutx-slave-id=10", // Only for gps provider RUTX09
		"--speed-override=10", // Only for vehicle provider SIMULATION
		"--wait-at-stop-s=20", // Only for vehicle provider SIMULATION
		"--map=maps/empty.osm", // Only for vehicle provider SIMULATION or gps provider MAP
		"--default-route=route", // Only for vehicle provider SIMULATION or gps provider MAP
		"--fleet-provider-type=internal-protocol",
		"--module-gateway-ip=192.168.20.20",
		"--module-gateway-port=1234"
	});
	EXPECT_TRUE(result);

	auto settings = settingsParser.getSettings();
	EXPECT_EQ(settings->messagePeriodMs, 2000);
	EXPECT_EQ(settings->vehicleProvider, bringauto::settings::VehicleProvider::E_GPS);
	EXPECT_EQ(settings->gpsProvider, bringauto::settings::GpsProvider::E_RUTX09);
	EXPECT_EQ(settings->rutxIp, "192.168.10.10");
	EXPECT_EQ(settings->rutxPort, 1234);
	EXPECT_EQ(settings->rutxSlaveId, 10);
	EXPECT_EQ(settings->stopRadiusM, 10);
	EXPECT_EQ(settings->inStopDelayS.count(), 20);
	EXPECT_EQ(settings->mapFilePath, ""); // Default value
	EXPECT_EQ(settings->routeName, ""); // Default value
	EXPECT_EQ(settings->speedOverride, 0); // Default value
	EXPECT_EQ(settings->speedOverrideMS, 0); // Default value
	EXPECT_EQ(settings->stopWaitTime, 5); // Default value
	EXPECT_EQ(settings->fleetProvider, bringauto::settings::FleetProvider::E_INTERNAL_PROTOCOL);
	EXPECT_EQ(settings->moduleGatewayIp, "192.168.20.20");
	EXPECT_EQ(settings->moduleGatewayPort, 1234);
}


/**
 * @brief Test if the help command option is correctly parsed
 */
TEST_F(SettingsParserTest, HelpCmdOption){
	auto result = parseConfig(ConfigMock::Config(), {"--help"});
	EXPECT_FALSE(result);
}


/**
 * @brief Test if duplicate command line options are correctly handled
 */
TEST_F(SettingsParserTest, DuplicateCmdOption){
	bool failed = false;
	try	{
		auto result = parseConfig(ConfigMock::Config(), {"--config=/path"});
	}catch (std::invalid_argument &e){
		EXPECT_STREQ(e.what(), "Cmd arguments are not correct.");
		failed = true;
	}
	EXPECT_TRUE(failed);
}


/**
 * @brief Test if unmatched command line options are correctly handled
 */
TEST_F(SettingsParserTest, UnmatchedCmdOption){
	bool failed = false;
	try	{
		auto result = parseConfig(ConfigMock::Config(), {"--made-up-option=value"});
	}catch (std::invalid_argument &e){
		EXPECT_STREQ(e.what(), "Cmd arguments are not correct.");
		failed = true;
	}
	EXPECT_TRUE(failed);
}


/**
 * @brief Test if missing required command line options are correctly handled
 */
TEST_F(SettingsParserTest, MissingRequiredConfigOption){
	bool failed = false;
	try	{
		auto result = parseConfig(ConfigMock::Config(), {"vvutests", "--module-gateway-ip=0.0.0.0"}, true);
	}catch (std::invalid_argument &e){
		EXPECT_STREQ(e.what(), "Cmd arguments are not correct.");
		failed = true;
	}
	EXPECT_TRUE(failed);
}


/**
 * @brief Test if invalid vehicle previder is correctly handled
 */
TEST_F(SettingsParserTest, InvalidVehicleProvider){
	ConfigMock::Config config {};
	config.vehicle_settings.vehicle_provider = "INVALID_PROVIDER";
	bool failed = false;
	try	{
		auto result = parseConfig(config);
	}catch (std::invalid_argument &e){
		EXPECT_STREQ(e.what(), "Arguments are not correct.");
		failed = true;
	}
	EXPECT_TRUE(failed);
}


/**
 * @brief Test if non existent log path is correctly handled
 */
TEST_F(SettingsParserTest, LogPathNonExistent){
	ConfigMock::Config config {};
	config.logging.file.path = "/non/existent/path";
	bool failed = false;
	try {
		auto result = parseConfig(config);
	}catch (std::invalid_argument &e){
		EXPECT_STREQ(e.what(), "Arguments are not correct.");
		failed = true;
	}
	EXPECT_TRUE(failed);
}


/**
 * @brief Test if log path being a file is correctly handled
 */
TEST_F(SettingsParserTest, LogPathNotDirectory){
	ConfigMock::Config config {};
	config.logging.file.path = "/dev/null";
	bool failed = false;
	try	{
		auto result = parseConfig(config);
	}catch (std::invalid_argument &e){
		EXPECT_STREQ(e.what(), "Arguments are not correct.");
		failed = true;
	}
	EXPECT_TRUE(failed);
}


/**
 * @brief Test if non existent map file is correctly handled
 */
TEST_F(SettingsParserTest, MapFileNonExistent){
	ConfigMock::Config config {};
	config.vehicle_settings.simulation_settings.map = "maps/non_existent.osm";
	bool failed = false;
	try	{
		auto result = parseConfig(config);
	}catch (std::invalid_argument &e){
		EXPECT_STREQ(e.what(), "Arguments are not correct.");
		failed = true;
	}
	EXPECT_TRUE(failed);
}


/**
 * @brief Test if invalid gps provider is correctly handled
 */
TEST_F(SettingsParserTest, InvalidGpsProvider){
	ConfigMock::Config config {};
	config.vehicle_settings.vehicle_provider = "GPS";
	config.vehicle_settings.gps_settings.gps_provider = "INVALID_PROVIDER";
	bool failed = false;
	try	{
		auto result = parseConfig(config);
	}catch (std::invalid_argument &e){
		EXPECT_STREQ(e.what(), "Arguments are not correct.");
		failed = true;
	}
	EXPECT_TRUE(failed);
}


/**
 * @brief Test if invalid fleet provider is correctly handled
 */
TEST_F(SettingsParserTest, InvalidFleetProvider){
	ConfigMock::Config config {};
	config.fleet_settings.fleet_provider = "INVALID_PROVIDER";
	bool failed = false;
	try {
		auto result = parseConfig(config);
	}catch (std::invalid_argument &e){
		EXPECT_STREQ(e.what(), "Arguments are not correct.");
		failed = true;
	}
	EXPECT_TRUE(failed);
}


/**
 * @brief Test if message period being zero is correctly handled
 */
TEST_F(SettingsParserTest, MessagePeriodZero){
	ConfigMock::Config config {};
	config.general_settings.period_ms = 0;
	bool failed = false;
	try	{
		auto result = parseConfig(config);
	}catch (std::invalid_argument &e){
		EXPECT_STREQ(e.what(), "Arguments are not correct.");
		failed = true;
	}
	EXPECT_TRUE(failed);
}
