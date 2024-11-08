#pragma once

#include <format>
#include <iostream>

class ConfigMock {
public:
	struct Config {
		struct GeneralSettings {
			int period_ms { 1000 };
		} general_settings;

		struct Logging {
			struct Console {
				std::string level { "DEBUG" };
				bool use  { true };
			} console;
			struct File {
				std::string level { "DEBUG" };
				bool use { true };
				std::string path { "./" };
			} file;
		} logging;

		struct VehicleSettings {
			std::string vehicle_provider { "SIMULATION" };

			struct GpsSettings {
				std::string gps_provider { "MAP" };
				struct Rutx09Settings {
					std::string rutx_ip { "192.168.2.1" };
					int rutx_port { 502 };
					int rutx_slave_id { 1 };
				} rutx09_settings;
				struct MapSettings {
					std::string map { "maps/virtual_vehicle_map.osm" };
					std::string default_route { "" };
				} map_settings;
				int stop_radius_m { 5 };
				int in_stop_delay_s { 10 };
			} gps_settings;

			struct SimulationSettings {
				bool speed_override { true };
				int speed_override_mps { 5 };
				int wait_at_stop_s { 10 };
				std::string map { "maps/virtual_vehicle_map.osm" };
				std::string default_route { "" };
			} simulation_settings;
		} vehicle_settings;

		struct FleetSettings {
			std::string fleet_provider { "INTERNAL-PROTOCOL" };
			struct InternalProtocolSettings {
				std::string module_gateway_ip { "localhost" };
				int module_gateway_port { 1636 };
				std::string device_role { "autonomy" };
				int device_priority { 1 };
				int reconnect_period_s { 20 };
				std::string device_name { "virtual_vehicle"};
			} internal_protocol_settings;
		} fleet_settings;
	};

	ConfigMock(Config config) : config_(config) {
		auto gps_settings = config_.vehicle_settings.gps_settings;
		auto sim_settings = config_.vehicle_settings.simulation_settings;
		auto ip_settings = config_.fleet_settings.internal_protocol_settings;

		configString_ = std::format(
			"{{\n"
				"\"general-settings\": {{\n"
					"\"period-ms\": {}\n"
				"}},\n"
				"\"logging\": {{\n"
					"\"console\": {{\n"
						"\"level\": \"{}\",\n"
						"\"use\": {}\n"
					"}},\n"
					"\"file\": {{\n"
						"\"level\": \"{}\",\n"
						"\"use\": {},\n"
						"\"path\": \"{}\"\n"
					"}}\n"
				"}},\n"
				"\"vehicle-settings\": {{\n"
					"\"vehicle-provider-type\": \"{}\",\n"
					"\"provider-gps-settings\": {{\n"
						"\"gps-provider-type\": \"{}\",\n"
						"\"rutx09-settings\" : {{\n"
							"\"rutx-ip\": \"{}\",\n"
							"\"rutx-port\": {},\n"
							"\"rutx-slave-id\": {}\n"
						"}},\n"
						"\"map-settings\": {{\n"
							"\"map\": \"{}\",\n"
							"\"default-route\": \"{}\"\n"
						"}},\n"
						"\"stop-radius-m\": {},\n"
						"\"in-stop-delay-s\": {}\n"
					"}},\n"
					"\"provider-simulation-settings\": {{\n"
						"\"speed-override\": {},\n"
						"\"speed-override-mps\": {},\n"
						"\"wait-at-stop-s\": {},\n"
						"\"map\": \"{}\",\n"
						"\"default-route\": \"{}\"\n"
					"}}\n"
				"}},\n"
				"\"fleet-settings\": {{\n"
					"\"fleet-provider-type\": \"{}\",\n"
					"\"provider-internal-protocol-settings\": {{\n"
						"\"module-gateway-ip\":\"{}\",\n"
						"\"module-gateway-port\": {},\n"
						"\"device-role\": \"{}\",\n"
						"\"device-priority\": {},\n"
						"\"reconnect-period-s\": {},\n"
						"\"device-name\": \"{}\"\n"
					"}}\n"
				"}}\n"
			"}}",
			config_.general_settings.period_ms,
			config_.logging.console.level, boolToString(config_.logging.console.use),
			config_.logging.file.level, boolToString(config_.logging.file.use), config_.logging.file.path,
			config_.vehicle_settings.vehicle_provider,
			gps_settings.gps_provider,              gps_settings.rutx09_settings.rutx_ip,
			gps_settings.rutx09_settings.rutx_port, gps_settings.rutx09_settings.rutx_slave_id,
			gps_settings.map_settings.map,          gps_settings.map_settings.default_route,
			gps_settings.stop_radius_m,	            gps_settings.in_stop_delay_s,
			boolToString(sim_settings.speed_override), sim_settings.speed_override_mps,
			sim_settings.wait_at_stop_s, sim_settings.map, sim_settings.default_route,
			config_.fleet_settings.fleet_provider,
			ip_settings.module_gateway_ip,  ip_settings.module_gateway_port,
			ip_settings.device_role,        ip_settings.device_priority,
			ip_settings.reconnect_period_s, ip_settings.device_name
		);
	}

	std::string getConfigString() {
		return configString_;
	}

private:
	std::string boolToString(bool value) {
		return value ? "true" : "false";
	}

	Config config_ {};
	std::string configString_;
};
