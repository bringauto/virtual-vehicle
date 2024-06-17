#include <bringauto/virtual_vehicle/Map.hpp>
#include <bringauto/virtual_vehicle/vehicle_provider/SimVehicle.hpp>
#include <bringauto/virtual_vehicle/vehicle_provider/GpsVehicle.hpp>
#include <bringauto/communication/fleet_protocol/FleetProtocol.hpp>
#include <bringauto/communication/terminal/TerminalOutput.hpp>
#include <bringauto/virtual_vehicle/GlobalContext.hpp>
#include <bringauto/settings/SettingsParser.hpp>



#ifdef STATE_SMURF
#include <bringauto/settings/StateSmurfDefinition.hpp>
#endif

#include <libbringauto_logger/bringauto/logging/Logger.hpp>
#include <libbringauto_logger/bringauto/logging/FileSink.hpp>
#include <libbringauto_logger/bringauto/logging/ConsoleSink.hpp>



void initLogger(const std::string &logPath, bool verbose) {
	using namespace bringauto::logging;
#ifdef STATE_SMURF
	verbose = true;
#endif
	if(verbose) {
		Logger::addSink<bringauto::logging::ConsoleSink>();
	}
	FileSink::Params paramFileSink { logPath, "virtual-vehicle-utility.log" };
	paramFileSink.maxFileSize = 50_MiB;
	paramFileSink.numberOfRotatedFiles = 5;
	paramFileSink.verbosity = Logger::Verbosity::Debug;

	Logger::addSink<bringauto::logging::FileSink>(paramFileSink);
	Logger::LoggerSettings params { "virtual-vehicle-utility",
									Logger::Verbosity::Debug };
	Logger::init(params);
}

int main(int argc, char **argv) {
	using namespace bringauto;
	int exitCode = EXIT_SUCCESS;
	std::unique_ptr<virtual_vehicle::vehicle_provider::IVirtualVehicle> vehicle;
	std::shared_ptr<communication::ICommunication> fleet;
	std::shared_ptr<virtual_vehicle::GlobalContext> context;
	std::shared_ptr<settings::Settings> settings;
	std::thread contextThread;

	try {
		settings::SettingsParser settingsParser;
		if(!settingsParser.parseSettings(argc, argv)) {
			return exitCode;
		}
		context = std::make_shared<virtual_vehicle::GlobalContext>();
		settings = settingsParser.getSettings();
		context->settings = settings;
		initLogger(settings->logPath, settings->verbose);
		logging::Logger::logInfo("Version: {} Settings:\n{}", VIRTUAL_VEHICLE_UTILITY_VERSION,
								 settingsParser.getFormattedSettings());

	} catch(std::exception &e) {
		std::cerr << "[ERROR] Error occurred during initialization: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}


	try {
		boost::asio::signal_set signals { context->ioContext, SIGINT, SIGTERM };
		signals.async_wait([context](auto, auto) { context->ioContext.stop(); });
		contextThread = std::thread([context]() { context->ioContext.run(); });

		switch(settings->fleetProvider) {
			case bringauto::settings::FleetProvider::E_INTERNAL_PROTOCOL:
				fleet = std::make_shared<bringauto::communication::fleet_protocol::FleetProtocol>(context);
				break;
			case bringauto::settings::FleetProvider::E_NO_CONNECTION:
				fleet = std::make_shared<bringauto::communication::terminal::TerminalOutput>(context);
				break;
			case bringauto::settings::FleetProvider::E_INVALID:
			default:
				throw std::runtime_error("Unsupported fleet provider");
		}

		switch(settings->vehicleProvider) {
			case bringauto::settings::VehicleProvider::E_SIMULATION:
				vehicle = std::make_unique<bringauto::virtual_vehicle::vehicle_provider::SimVehicle>(fleet, context);
				break;
			case bringauto::settings::VehicleProvider::E_GPS:
				vehicle = std::make_unique<bringauto::virtual_vehicle::vehicle_provider::GpsVehicle>(fleet, context);
				break;
			case bringauto::settings::VehicleProvider::E_INVALID:
			default:
				throw std::runtime_error("Unsupported vehicle provider");
		}
#ifdef STATE_SMURF
		auto stateDiagram = bringauto::settings::StateSmurfDefinition::createStateDiagram();
		auto transitions = std::make_shared<state_smurf::transition::StateTransition>(stateDiagram);
		context->transitions = transitions;
#endif
		vehicle->initialize();
		vehicle->drive();

	} catch(const std::exception &e) {
		exitCode = EXIT_FAILURE;
		bringauto::logging::Logger::logError(e.what());
	} catch(...) {
		exitCode = EXIT_FAILURE;
		bringauto::logging::Logger::logError("error: unknown exceptions");
	}
	if(!context->ioContext.stopped()) {
		context->ioContext.stop();
	}
	contextThread.join();

	return exitCode;
}
