#pragma once

#include <bringauto/logging/Logger.hpp>
#include <bringauto/logging/ConsoleSink.hpp>
#include <gtest/gtest.h>



class MapTest: public ::testing::Test {
protected:
	void SetUp() override {
		bringauto::logging::Logger::addSink<bringauto::logging::ConsoleSink>();
		bringauto::logging::Logger::LoggerSettings params { "virtual-vehicle-mapTest",
															bringauto::logging::Logger::Verbosity::Info };
		bringauto::logging::Logger::init(params);
	}

	void TearDown() override {
		bringauto::logging::Logger::destroy();
	}
};