#include <bringauto/logging/Logger.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>

#include <iostream>

void bringauto::logging::Logger::initLogger(const std::string &logPath, bool verbose, const std::string &logName) {
    try {
        std::vector<spdlog::sink_ptr> sinks;

        //console sink and info file log
        if (true) {
            sinks.push_back(std::make_shared<spdlog::sinks::stdout_sink_st>());
            sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(logPath + logName + "_info_log.txt",
                                                                                   1024 * 1024 * 5, 1, false));

        }

        //rotating file sink for error and warn logs
        auto console_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(logPath + logName + "_error_log.txt",
                                                                                   1024 * 1024 * 5, 1, false);
        console_sink->set_level(spdlog::level::info);
        sinks.push_back(console_sink);

        //create synchronous  loggers
        auto readerLogger = std::make_shared<spdlog::logger>("logger", begin(sinks), end(sinks));

        readerLogger->flush_on(spdlog::level::info);

        // globally register the loggers so they can be accessed using spdlog::get(logger_name)
        spdlog::register_logger(readerLogger);
    }
    catch (const spdlog::spdlog_ex &ex) {
        std::cerr << "Log initialization failed: " << ex.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}

void bringauto::logging::Logger::Logger::logInfo(const std::string &message) {
    std::shared_ptr<spdlog::logger> logger;
    logger = spdlog::get("logger");
    logger->info(message);
}

void bringauto::logging::Logger::Logger::logWarning(const std::string &message) {
    std::shared_ptr<spdlog::logger> logger;
    logger = spdlog::get("logger");
    logger->warn(message);
}

void bringauto::logging::Logger::Logger::logError(const std::string &message) {
    std::shared_ptr<spdlog::logger> logger;
    logger = spdlog::get("logger");
    logger->error(message);
}
