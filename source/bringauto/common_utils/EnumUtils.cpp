#include <bringauto/common_utils/EnumUtils.hpp>
#include <bringauto/logging/LoggerVerbosity.hpp>

#include <MissionModule.pb.h>

#include <algorithm>



namespace bringauto::common_utils {

template <>
MissionModule::AutonomyStatus::State EnumUtils::valueToEnum(communication::EAutonomyState toEnum) {
	switch(toEnum) {
		case communication::EAutonomyState::E_IDLE:
			return MissionModule::AutonomyStatus::IDLE;
		case communication::EAutonomyState::E_DRIVE:
			return MissionModule::AutonomyStatus::DRIVE;
		case communication::EAutonomyState::E_IN_STOP:
			return MissionModule::AutonomyStatus::IN_STOP;
		case communication::EAutonomyState::E_OBSTACLE:
			return MissionModule::AutonomyStatus::OBSTACLE;
		case communication::EAutonomyState::E_ERROR:
			return MissionModule::AutonomyStatus::ERROR;
		default:
			throw std::runtime_error("Invalid enum value of EAutonomyState");
	}
}

template <>
communication::EAutonomyAction EnumUtils::valueToEnum(MissionModule::AutonomyCommand::Action toEnum) {
	switch(toEnum) {
		case MissionModule::AutonomyCommand::START:
			return communication::EAutonomyAction::E_START;
		case MissionModule::AutonomyCommand::STOP:
			return communication::EAutonomyAction::E_STOP;
		case MissionModule::AutonomyCommand::NO_ACTION:
			return communication::EAutonomyAction::E_NO_ACTION;
		default:
			return communication::EAutonomyAction::E_INVALID;
	}
}

template <>
settings::VehicleProvider EnumUtils::valueToEnum(std::string toEnum) {
	std::transform(toEnum.begin(), toEnum.end(), toEnum.begin(), ::toupper);
	if(toEnum == "SIMULATION") {
		return settings::VehicleProvider::E_SIMULATION;
	} else if(toEnum == "GPS") {
		return settings::VehicleProvider::E_GPS;
	}
	return settings::VehicleProvider::E_INVALID;
}

template <>
settings::GpsProvider EnumUtils::valueToEnum(std::string toEnum) {
	std::transform(toEnum.begin(), toEnum.end(), toEnum.begin(), ::toupper);
	if(toEnum == "RUTX09") {
		return settings::GpsProvider::E_RUTX09;
	} else if(toEnum == "UBLOX") {
		return settings::GpsProvider::E_UBLOX;
	} else if(toEnum == "MAP") {
		return settings::GpsProvider::E_MAP;
	}
	return settings::GpsProvider::E_INVALID;
}

template <>
settings::FleetProvider EnumUtils::valueToEnum(std::string toEnum) {
	std::transform(toEnum.begin(), toEnum.end(), toEnum.begin(), ::toupper);
	if(toEnum == "INTERNAL-PROTOCOL") {
		return settings::FleetProvider::E_INTERNAL_PROTOCOL;
	} else if(toEnum == "NO-CONNECTION") {
		return settings::FleetProvider::E_NO_CONNECTION;
	}
	return settings::FleetProvider::E_INVALID;
}

template <>
logging::LoggerVerbosity EnumUtils::valueToEnum(std::string toEnum) {
	std::transform(toEnum.begin(), toEnum.end(), toEnum.begin(), ::toupper);
	if(toEnum == "DEBUG") {
		return logging::LoggerVerbosity::Debug;
	} else if(toEnum == "INFO") {
		return logging::LoggerVerbosity::Info;
	} else if(toEnum == "WARNING") {
		return logging::LoggerVerbosity::Warning;
	} else if(toEnum == "ERROR") {
		return logging::LoggerVerbosity::Error;
	} else if(toEnum == "CRITICAL") {
		return logging::LoggerVerbosity::Critical;
	}
	std::cerr << "Invalid verbosity level: " << toEnum << std::endl;
	return logging::LoggerVerbosity::Debug;
}

template <>
std::string EnumUtils::enumToString(communication::EAutonomyAction value) {
	switch(value) {
		case communication::EAutonomyAction::E_NO_ACTION:
			return "NO_ACTION";
		case communication::EAutonomyAction::E_STOP:
			return "STOP";
		case communication::EAutonomyAction::E_START:
			return "START";
		default:
			return "INVALID";
	}
}

template <>
std::string EnumUtils::enumToString(communication::EAutonomyState value) {
	switch(value) {

		case communication::EAutonomyState::E_IDLE:
			return "IDLE";
		case communication::EAutonomyState::E_DRIVE:
			return "DRIVE";
		case communication::EAutonomyState::E_IN_STOP:
			return "IN_STOP";
		case communication::EAutonomyState::E_OBSTACLE:
			return "OBSTACLE";
		case communication::EAutonomyState::E_ERROR:
			return "ERROR";
		default:
			return "INVALID";
	}
}

template <>
std::string EnumUtils::enumToString(settings::VehicleProvider value) {
	switch(value) {
		case settings::VehicleProvider::E_SIMULATION:
			return "SIMULATION";
		case settings::VehicleProvider::E_GPS:
			return "GPS";
		case settings::VehicleProvider::E_INVALID:
		default:
			return "INVALID";
	}
}

template <>
std::string EnumUtils::enumToString(settings::GpsProvider value) {
	switch(value) {
		case settings::GpsProvider::E_RUTX09:
			return "RUTX09";
		case settings::GpsProvider::E_UBLOX:
			return "UBLOX";
		case settings::GpsProvider::E_MAP:
			return "MAP";
		case settings::GpsProvider::E_INVALID:
		default:
			return "INVALID";
	}
}

template <>
std::string EnumUtils::enumToString(settings::FleetProvider value) {
	switch(value) {
		case settings::FleetProvider::E_INTERNAL_PROTOCOL:
			return "INTERNAL-PROTOCOL";
		case settings::FleetProvider::E_NO_CONNECTION:
			return "NO-CONNECTION";
		case settings::FleetProvider::E_INVALID:
		default:
			return "INVALID";
	}
}

template <>
std::string EnumUtils::enumToString(logging::LoggerVerbosity value) {
	switch(value) {
		case logging::LoggerVerbosity::Debug:
			return "DEBUG";
		case logging::LoggerVerbosity::Info:
			return "INFO";
		case logging::LoggerVerbosity::Warning:
			return "WARNING";
		case logging::LoggerVerbosity::Error:
			return "ERROR";
		case logging::LoggerVerbosity::Critical:
			return "CRITICAL";
		default:
			return "INVALID";
	}
}


}