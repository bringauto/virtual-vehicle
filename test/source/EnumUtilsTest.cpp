#include <bringauto/common_utils/EnumUtils.hpp>

#include <MissionModule.pb.h>

#include <gtest/gtest.h>


/**
 * @brief Test valueToEnum function
 */
TEST(EnumUtilsTest, ValueToEnum){
	namespace bacu = bringauto::common_utils;
	namespace bas = bringauto::settings;
	namespace bacomm = bringauto::communication;
	namespace balog = bringauto::logging;
	bool failed = false;

	EXPECT_EQ(bacu::EnumUtils::valueToEnum<MissionModule::AutonomyStatus_State>(bacomm::EAutonomyState::E_IDLE), MissionModule::AutonomyStatus::IDLE);
	EXPECT_EQ(bacu::EnumUtils::valueToEnum<MissionModule::AutonomyStatus_State>(bacomm::EAutonomyState::E_DRIVE), MissionModule::AutonomyStatus::DRIVE);
	EXPECT_EQ(bacu::EnumUtils::valueToEnum<MissionModule::AutonomyStatus_State>(bacomm::EAutonomyState::E_IN_STOP), MissionModule::AutonomyStatus::IN_STOP);
	EXPECT_EQ(bacu::EnumUtils::valueToEnum<MissionModule::AutonomyStatus_State>(bacomm::EAutonomyState::E_OBSTACLE), MissionModule::AutonomyStatus::OBSTACLE);
	EXPECT_EQ(bacu::EnumUtils::valueToEnum<MissionModule::AutonomyStatus_State>(bacomm::EAutonomyState::E_ERROR), MissionModule::AutonomyStatus::ERROR);
	try{
		bacu::EnumUtils::valueToEnum<MissionModule::AutonomyStatus_State>(bacomm::EAutonomyState::E_INVALID);
	} catch (std::runtime_error &e){
		EXPECT_STREQ(e.what(), "Invalid enum value of EAutonomyState");
		failed = true;
	}
	EXPECT_TRUE(failed);

	EXPECT_EQ(bacu::EnumUtils::valueToEnum<bacomm::EAutonomyAction>(MissionModule::AutonomyCommand::START), bacomm::EAutonomyAction::E_START);
	EXPECT_EQ(bacu::EnumUtils::valueToEnum<bacomm::EAutonomyAction>(MissionModule::AutonomyCommand::STOP), bacomm::EAutonomyAction::E_STOP);
	EXPECT_EQ(bacu::EnumUtils::valueToEnum<bacomm::EAutonomyAction>(MissionModule::AutonomyCommand::NO_ACTION), bacomm::EAutonomyAction::E_NO_ACTION);
	
	EXPECT_EQ(bacu::EnumUtils::valueToEnum<bas::VehicleProvider>(std::string("SIMULATION")), bas::VehicleProvider::E_SIMULATION);
	EXPECT_EQ(bacu::EnumUtils::valueToEnum<bas::VehicleProvider>(std::string("GPS")), bas::VehicleProvider::E_GPS);
	EXPECT_EQ(bacu::EnumUtils::valueToEnum<bas::VehicleProvider>(std::string("INVALID")), bas::VehicleProvider::E_INVALID);

	EXPECT_EQ(bacu::EnumUtils::valueToEnum<bas::GpsProvider>(std::string("RUTX09")), bas::GpsProvider::E_RUTX09);
	EXPECT_EQ(bacu::EnumUtils::valueToEnum<bas::GpsProvider>(std::string("UBLOX")), bas::GpsProvider::E_UBLOX);
	EXPECT_EQ(bacu::EnumUtils::valueToEnum<bas::GpsProvider>(std::string("MAP")), bas::GpsProvider::E_MAP);
	EXPECT_EQ(bacu::EnumUtils::valueToEnum<bas::GpsProvider>(std::string("INVALID")), bas::GpsProvider::E_INVALID);

	EXPECT_EQ(bacu::EnumUtils::valueToEnum<bas::FleetProvider>(std::string("INTERNAL-PROTOCOL")), bas::FleetProvider::E_INTERNAL_PROTOCOL);
	EXPECT_EQ(bacu::EnumUtils::valueToEnum<bas::FleetProvider>(std::string("NO-CONNECTION")), bas::FleetProvider::E_NO_CONNECTION);
	EXPECT_EQ(bacu::EnumUtils::valueToEnum<bas::FleetProvider>(std::string("INVALID")), bas::FleetProvider::E_INVALID);

	EXPECT_EQ(bacu::EnumUtils::valueToEnum<balog::LoggerVerbosity>(std::string("DEBUG")), balog::LoggerVerbosity::Debug);
	EXPECT_EQ(bacu::EnumUtils::valueToEnum<balog::LoggerVerbosity>(std::string("INFO")), balog::LoggerVerbosity::Info);
	EXPECT_EQ(bacu::EnumUtils::valueToEnum<balog::LoggerVerbosity>(std::string("WARNING")), balog::LoggerVerbosity::Warning);
	EXPECT_EQ(bacu::EnumUtils::valueToEnum<balog::LoggerVerbosity>(std::string("ERROR")), balog::LoggerVerbosity::Error);
	EXPECT_EQ(bacu::EnumUtils::valueToEnum<balog::LoggerVerbosity>(std::string("CRITICAL")), balog::LoggerVerbosity::Critical);
	EXPECT_EQ(bacu::EnumUtils::valueToEnum<balog::LoggerVerbosity>(std::string("INVALID")), balog::LoggerVerbosity::Debug);
}


/**
 * @brief Test enumToString function
 */
TEST(EnumUtilTest, EnumToString){
	namespace bacu = bringauto::common_utils;
	namespace bas = bringauto::settings;
	namespace bacomm = bringauto::communication;
	namespace balog = bringauto::logging;

	EXPECT_EQ(bacu::EnumUtils::enumToString(bacomm::EAutonomyAction::E_NO_ACTION), "NO_ACTION");
	EXPECT_EQ(bacu::EnumUtils::enumToString(bacomm::EAutonomyAction::E_STOP), "STOP");
	EXPECT_EQ(bacu::EnumUtils::enumToString(bacomm::EAutonomyAction::E_START), "START");
	EXPECT_EQ(bacu::EnumUtils::enumToString(bacomm::EAutonomyAction::E_INVALID), "INVALID");

	EXPECT_EQ(bacu::EnumUtils::enumToString(bacomm::EAutonomyState::E_IDLE), "IDLE");
	EXPECT_EQ(bacu::EnumUtils::enumToString(bacomm::EAutonomyState::E_DRIVE), "DRIVE");
	EXPECT_EQ(bacu::EnumUtils::enumToString(bacomm::EAutonomyState::E_IN_STOP), "IN_STOP");
	EXPECT_EQ(bacu::EnumUtils::enumToString(bacomm::EAutonomyState::E_OBSTACLE), "OBSTACLE");
	EXPECT_EQ(bacu::EnumUtils::enumToString(bacomm::EAutonomyState::E_ERROR), "ERROR");
	EXPECT_EQ(bacu::EnumUtils::enumToString(bacomm::EAutonomyState::E_INVALID), "INVALID");

	EXPECT_EQ(bacu::EnumUtils::enumToString(bas::VehicleProvider::E_SIMULATION), "SIMULATION");
	EXPECT_EQ(bacu::EnumUtils::enumToString(bas::VehicleProvider::E_GPS), "GPS");
	EXPECT_EQ(bacu::EnumUtils::enumToString(bas::VehicleProvider::E_INVALID), "INVALID");
	EXPECT_EQ(bacu::EnumUtils::enumToString(bas::VehicleProvider::E_END), "INVALID");

	EXPECT_EQ(bacu::EnumUtils::enumToString(bas::GpsProvider::E_RUTX09), "RUTX09");
	EXPECT_EQ(bacu::EnumUtils::enumToString(bas::GpsProvider::E_UBLOX), "UBLOX");
	EXPECT_EQ(bacu::EnumUtils::enumToString(bas::GpsProvider::E_MAP), "MAP");
	EXPECT_EQ(bacu::EnumUtils::enumToString(bas::GpsProvider::E_INVALID), "INVALID");
	EXPECT_EQ(bacu::EnumUtils::enumToString(bas::GpsProvider::E_END), "INVALID");

	EXPECT_EQ(bacu::EnumUtils::enumToString(bas::FleetProvider::E_INTERNAL_PROTOCOL), "INTERNAL-PROTOCOL");
	EXPECT_EQ(bacu::EnumUtils::enumToString(bas::FleetProvider::E_NO_CONNECTION), "NO-CONNECTION");
	EXPECT_EQ(bacu::EnumUtils::enumToString(bas::FleetProvider::E_INVALID), "INVALID");
	EXPECT_EQ(bacu::EnumUtils::enumToString(bas::FleetProvider::E_END), "INVALID");

	EXPECT_EQ(bacu::EnumUtils::enumToString(balog::LoggerVerbosity::Debug), "DEBUG");
	EXPECT_EQ(bacu::EnumUtils::enumToString(balog::LoggerVerbosity::Info), "INFO");
	EXPECT_EQ(bacu::EnumUtils::enumToString(balog::LoggerVerbosity::Warning), "WARNING");
	EXPECT_EQ(bacu::EnumUtils::enumToString(balog::LoggerVerbosity::Error), "ERROR");
	EXPECT_EQ(bacu::EnumUtils::enumToString(balog::LoggerVerbosity::Critical), "CRITICAL");
}
