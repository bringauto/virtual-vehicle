#pragma once

#include <string>



namespace bringauto::communication {
struct Status {
	enum State {
		IDLE = 0, DRIVE = 1, IN_STOP = 2, OBSTACLE = 3, ERROR = 4
	};
	double longitude;
	double latitude;
	double speed;
	State state;
	std::string nextStop;

	friend std::ostream &operator<<(std::ostream &stream, const Status &matrix);
};
}