#pragma once

#include <string>

namespace bringauto::logging {
    class Logger {
    public:
        static void initLogger(const std::string &logPath, bool verbose, const std::string &logName);

        static void logInfo(const std::string &message);

        static void logWarning(const std::string &message);

        static void logError(const std::string &message);

    };

}