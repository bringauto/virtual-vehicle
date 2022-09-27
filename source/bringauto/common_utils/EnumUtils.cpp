#include <bringauto/common_utils/EnumUtils.hpp>

#include <algorithm>



namespace bringauto::common_utils {
settings::VehicleProvider EnumUtils::stringToVehicleProvider(std::string toEnum) {
	std::transform(toEnum.begin(), toEnum.end(), toEnum.begin(), ::toupper);
	if(toEnum == "SIMULATION"){
		return settings::VehicleProvider::SIMULATION;
	}else if(toEnum == "GPS"){
		return settings::VehicleProvider::GPS;
	}
	return settings::VehicleProvider::INVALID;
}

settings::GpsProvider EnumUtils::stringToGpsProvider(std::string toEnum) {
	std::transform(toEnum.begin(), toEnum.end(), toEnum.begin(), ::toupper);
	if(toEnum == "RUTX09"){
		return settings::GpsProvider::RUTX09;
	}else if(toEnum == "UBLOX"){
		return settings::GpsProvider::UBLOX;
	}else if(toEnum == "MAP"){
		return settings::GpsProvider::MAP;
	}
	return settings::GpsProvider::INVALID;
}

settings::FleetProvider EnumUtils::stringToFleetProvider(std::string toEnum) {
	std::transform(toEnum.begin(), toEnum.end(), toEnum.begin(), ::toupper);
	if(toEnum == "PROTOBUF"){
		return settings::FleetProvider::PROTOBUF;
	}else if(toEnum == "EMPTY"){
		return settings::FleetProvider::NO_CONNECTION;
	}
	return settings::FleetProvider::INVALID;
}

template <>
std::string EnumUtils::enumToString(communication::Command::Action value) {
	switch(value) {
		case communication::Command::NO_ACTION:
			return "NO_ACTION";
		case communication::Command::STOP:
			return "STOP";
		case communication::Command::START:
			return "START";
	}
	return "unknown action";
}

template <>
std::string EnumUtils::enumToString(communication::Status::State value) {
	switch(value) {

		case communication::Status::IDLE:
			return "IDLE";
		case communication::Status::DRIVE:
			return "DRIVE";
		case communication::Status::IN_STOP:
			return "IN_STOP";
		case communication::Status::OBSTACLE:
			return "OBSTACLE";
		case communication::Status::ERROR:
			return "ERROR";
	}
	return "unknown state";
}

template <>
std::string EnumUtils::enumToString(settings::VehicleProvider value) {
	switch(value) {
		case settings::VehicleProvider::SIMULATION:
			return "SIMULATION";
		case settings::VehicleProvider::GPS:
			return "GPS";
		case settings::VehicleProvider::INVALID:
		default:
			return "INVALID";
	}
}

template <>
std::string EnumUtils::enumToString(settings::GpsProvider value) {
	switch(value) {
		case settings::GpsProvider::RUTX09:
			return "RUTX09";
		case settings::GpsProvider::UBLOX:
			return "UBLOX";
		case settings::GpsProvider::INVALID:
		default:
			return "INVALID";
	}
}

template <>
std::string EnumUtils::enumToString(settings::FleetProvider value) {
	switch(value) {
		case settings::FleetProvider::PROTOBUF:
			return "PROTOBUF";
		case settings::FleetProvider::NO_CONNECTION:
			return "NO_CONNECTION";
		case settings::FleetProvider::INVALID:
		default:
			return "INVALID";
	}
}


}