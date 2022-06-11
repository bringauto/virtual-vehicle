#include <bringauto/common_utils/EnumUtils.hpp>



namespace bringauto::common_utils {

template <>
std::string EnumUtils::enumToString(communication::Command::Action action) {
	switch(action) {
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
std::string EnumUtils::enumToString(communication::Status::State state) {
	switch(state) {

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
}