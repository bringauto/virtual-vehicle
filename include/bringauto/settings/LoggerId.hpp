#pragma once

#include <bringauto/logging/Logger.hpp>

namespace bringauto::settings {

constexpr bringauto::logging::LoggerId logId = {.id = "VirtualVehicle"};
using Logger = bringauto::logging::Logger<logId, bringauto::logging::LoggerImpl>;

}
