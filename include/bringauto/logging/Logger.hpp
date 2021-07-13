#pragma once

#include <string>

namespace bringauto::logging {
    /**
     * Logger hides spdlog implementation for simple logging
     */
    class Logger {
    public:

        /**
         * Opens spdlog sinks
         * @param logPath path where logs will be saved
         * @param verbose if true info logs will be saved and all logs will also be printed to console
         * @param logName name of logger
         */
        static void initLogger(const std::string &logPath, bool verbose, const std::string &logName);

        /**
         * Log info state
         * @param message message to log
         */
        static void logInfo(const std::string &message);

        /**
         * Log warning state
         * @param message message to log
         */
        static void logWarning(const std::string &message);

        /**
         * Log error state
         * @param message message to log
         */
        static void logError(const std::string &message);

    };

}