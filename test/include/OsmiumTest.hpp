#pragma once

#include <bringauto/settings/LoggerId.hpp>
#include <bringauto/logging/ConsoleSink.hpp>
#include <gtest/gtest.h>



class OsmiumTest: public ::testing::Test {
protected:
	void SetUp() override {
		bringauto::settings::Logger::addSink<bringauto::logging::ConsoleSink>();
		bringauto::logging::LoggerSettings params { "virtual-vehicle-osmiumTest",
													bringauto::logging::LoggerVerbosity::Info };
		bringauto::settings::Logger::init(params);
	}

	void TearDown() override {
		bringauto::settings::Logger::destroy();
	}
};