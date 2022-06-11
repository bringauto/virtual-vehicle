#pragma once

#include <vector>
#include <string>



namespace bringauto::communication {
/**
 * @brief Structure representing fleet protocol command
 */
struct Command {
	enum Action {
		NO_ACTION = 0,
		STOP = 1,
		START = 2
	};

	std::vector<std::string> stops; ///mission, vector contains list of stops that car should go to
	Action action { NO_ACTION };    ///car action

	inline bool operator==(const Command &toCompare) const {
		if(action != toCompare.action) {
			return false;
		}
		if(stops != toCompare.stops) {
			return false;
		}
		return true;
	}

	inline bool operator!=(const Command &toCompare) const {
		return !(*this == toCompare);
	}

	friend std::ostream &operator<<(std::ostream &stream, const Command &matrix);
};
}