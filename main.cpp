#include <bringauto/virtual_vehicle/Map.hpp>
#include <bringauto/communication/ICommunication.hpp>
#include <bringauto/communication/TerminalOutput.hpp>
#include <bringauto/virtual_vehicle/Vehicle.hpp>
#include <bringauto/communication/ProtoBuffer.hpp>
#include <bringauto/virtual_vehicle/GlobalContext.hpp>

#include <libbringauto_logger/bringauto/logging/Logger.hpp>
#include <libbringauto_logger/bringauto/logging/FileSink.hpp>
#include <libbringauto_logger/bringauto/logging/ConsoleSink.hpp>
#include <cxxopts.hpp>

#include <iostream>
#include <filesystem>

void checkArgs(const cxxopts::ParseResult &args) {

	auto mapPath = args["map"].as<std::string>();
	if (!std::filesystem::exists(mapPath)) {
		std::cerr << "Given params path (" << mapPath << ") does not exist." << std::endl;
		exit(EXIT_FAILURE);
	}
}


cxxopts::ParseResult parseArgs(int argc, char *argv[]) { // NOLINT
	cxxopts::Options options{"BringAuto virtual vehicle utility", "BringAuto virtual vehicle utility"};
	// clang-format off
	options.add_options()
			("v, verbose", "Verbose into terminal and log info messages");
	// clang-format on
	options.add_options()("m, map", "Path to .osm map file", cxxopts::value<std::string>());
	options.add_options()("r, route", "Name of route that will be used for vehicle", cxxopts::value<std::string>());
	options.add_options()("l, log-path", "Path to logs", cxxopts::value<std::string>()->default_value("./"));
	options.add_options()("i, ip", "IPv4 address or hostname of server side application",
						  cxxopts::value<std::string>());
	options.add_options()("p, port", "Port of server side application", cxxopts::value<int>());
	options.add_options()("c, cruise", "Turn on cruise mode, car will drive along route even without orders.");
	options.add_options()("w, wait", "Wait time in stops in seconds, default is 10s",
						  cxxopts::value<double>()->default_value("10.0"));
	options.add_options()("period", "Period in ms for sending status messages",
						  cxxopts::value<uint32_t>()->default_value("100"));

	options.add_options()("speed-override", "Override map speed on all points, in m/s",
						  cxxopts::value<uint32_t>());
	options.add_options()("h, help", "Print usage");
	auto args = options.parse(argc, argv);

	if (args.count("map") < 1 || args.count("route") < 1 || args.count("ip") < 1 || args.count("port") < 1) {
		std::cerr << "Please provide map path and route name.\n" << options.help() << std::endl;
		exit(EXIT_FAILURE);
	}
	if (args.count("help")) {
		std::cout << options.help() << std::endl;
		exit(EXIT_SUCCESS);
	}
	checkArgs(args);
	return args;
}

void initLogger(const std::string &logPath, bool verbose) {
	if (verbose) {
		bringauto::logging::Logger::addSink<bringauto::logging::ConsoleSink>();
	}
	bringauto::logging::Logger::addSink<bringauto::logging::FileSink>({logPath, "virtual-vehicle-utility.log"});
	bringauto::logging::Logger::LoggerSettings params{"virtual-vehicle-utility",
													  bringauto::logging::Logger::Verbosity::Info};
	bringauto::logging::Logger::init(params);
}

int main(int argc, char **argv) {
	int exitCode = EXIT_SUCCESS;
	std::thread contextThread;
	auto context = std::make_shared<bringauto::virtual_vehicle::GlobalContext>();
	auto args = parseArgs(argc, argv);

	try {
		initLogger(args["log-path"].as<std::string>(), args.count("verbose"));
	} catch (std::exception &e) {
		std::cerr << "Unable to initialize logger: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	try {
		context->cruise = args.count("cruise") > 0;
		context->stopWaitSeconds = args["wait"].as<double>();
		boost::asio::signal_set signals{context->ioContext, SIGINT, SIGTERM};
		signals.async_wait([context](auto, auto) { context->ioContext.stop(); });
		contextThread = std::thread([context]() { context->ioContext.run(); });

		bringauto::virtual_vehicle::Map map;
		map.loadMapFromFile(args["map"].as<std::string>());
		if(args.count("speed-override")){
			map.speedOverride(args["speed-override"].as<unsigned int>());
		}

		auto route = map.getRoute(args["route"].as<std::string>());

		auto com = std::make_shared<bringauto::communication::ProtoBuffer>(context, args["period"].as<uint32_t>(),
																		   args["ip"].as<std::string>(),
																		   args["port"].as<int>(), context);

		//auto com = std::make_shared<bringauto::communication::TerminalOutput>();
		bringauto::virtual_vehicle::Vehicle vehicle(route, com, context);

		vehicle.initialize();
		vehicle.drive();

	} catch (std::exception &e) {
		exitCode = EXIT_FAILURE;
		bringauto::logging::Logger::logError(e.what());
	} catch (...) {
		exitCode = EXIT_FAILURE;
		bringauto::logging::Logger::logError("error: unknown exceptions");
	}
	if (!context->ioContext.stopped()) {
		context->ioContext.stop();
	}
	contextThread.join();

	return exitCode;
}