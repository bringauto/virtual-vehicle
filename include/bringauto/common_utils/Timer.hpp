#pragma once
#include <chrono>

namespace bringauto::common_utils {
class Timer {
public:
	void start() {
		timerStart_ = std::chrono::steady_clock::now();
		timerRunning_ = true;
	}
	void stop() {
		timerRunning_ = false;
	}

	std::chrono::seconds getElapsedTime() {
		if(timerRunning_) {
			return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - timerStart_);
		} else {
			return std::chrono::seconds(0);
		}
	}

	bool isRunning() const {
		return timerRunning_;
	}
private:
	std::chrono::time_point<std::chrono::steady_clock> timerStart_;
	bool timerRunning_ = false;
};
}



