#include <bringauto/virtual_vehicle/Map.hpp>
#include <bringauto/virtual_vehicle/Vehicle.hpp>
#include <bringauto/communication/ProtoBuffer.hpp>
#include <bringauto/virtual_vehicle/GlobalContext.hpp>
#include <bringauto/settings/SettingsParser.hpp>
#include <bringauto/settings/StateSmurfDefinition.hpp>

#include <libbringauto_logger/bringauto/logging/Logger.hpp>
#include <libbringauto_logger/bringauto/logging/FileSink.hpp>
#include <libbringauto_logger/bringauto/logging/ConsoleSink.hpp>


void initLogger(const std::string &logPath, bool verbose) {
#ifdef STATE_SMURF
	verbose = true;
#endif
	if(verbose) {
		bringauto::logging::Logger::addSink<bringauto::logging::ConsoleSink>();
	}
	bringauto::logging::FileSink::Params paramFileSink{logPath, "vvu"};
	paramFileSink.maxFileSize = 1024 * 1024 * 50; //50MB
	paramFileSink.numberOfRotatedFiles = 5;
	paramFileSink.verbosity = bringauto::logging::Logger::Verbosity::Debug;

	bringauto::logging::Logger::addSink<bringauto::logging::FileSink>({ logPath, "virtual-vehicle-utility.log" });
	bringauto::logging::Logger::LoggerSettings params { "virtual-vehicle-utility",
														bringauto::logging::Logger::Verbosity::Debug };
	bringauto::logging::Logger::init(params);
}

int main(int argc, char **argv) {
	int exitCode = EXIT_SUCCESS;
	bringauto::settings::SettingsParser settingsParser;

	try {
		if(!settingsParser.parseSettings(argc, argv)) {
			return exitCode;
		}
	} catch(std::exception &e) {
		std::cerr << "[ERROR] Unable to parse cmd arguments: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	std::thread contextThread;
	auto context = std::make_shared<bringauto::virtual_vehicle::GlobalContext>();

	try {
		auto settings = settingsParser.getSettings();
		context->settings = settings;
		initLogger(settings->logPath, settings->verbose);

#ifdef STATE_SMURF
		auto stateDiagram = bringauto::settings::StateSmurfDefinition::createStateDiagram();
		auto transitions = std::make_shared<state_smurf::transition::StateTransition>(stateDiagram);
		context->transitions = transitions;
#endif

		boost::asio::signal_set signals { context->ioContext, SIGINT, SIGTERM };
		signals.async_wait([context](auto, auto) { context->ioContext.stop(); });
		contextThread = std::thread([context]() { context->ioContext.run(); });

		bringauto::virtual_vehicle::Map map;
		map.loadMapFromFile(settings->mapFilePath);
		if(settings->speedOverride) {
			map.speedOverride(settings->speedOverrideMS);
		}

		auto route = map.getRoute(settings->routeName);

		auto com = std::make_shared<bringauto::communication::ProtoBuffer>(context);
		bringauto::virtual_vehicle::Vehicle vehicle(route, com, context);

		vehicle.initialize();
		vehicle.drive();

	} catch(std::exception &e) {
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