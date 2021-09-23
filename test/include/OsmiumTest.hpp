#pragma once

#include <bringauto/logging/Logger.hpp>
#include <bringauto/logging/ConsoleSink.hpp>
#include <gtest/gtest.h>

class OsmiumTest : public ::testing::Test {
protected:
    void SetUp() override {
        bringauto::logging::Logger::addSink<bringauto::logging::ConsoleSink>();
        bringauto::logging::Logger::LoggerSettings params{"virtual-vehicle-osmiumTest", bringauto::logging::Logger::Verbosity::Info};
        bringauto::logging::Logger::init(params);
    }
};