#pragma once

#include <bringauto/osm/Route.hpp>

#include <string>



namespace bringauto::communication {
/**
 * @brief Enum representing state of the autonomy
 */
enum class EAutonomyState {
	E_BEGIN = 0,
	E_INVALID = E_BEGIN,
	E_IDLE,
	E_DRIVE,
	E_IN_STOP,
	E_OBSTACLE,
	E_ERROR,
	E_END
};

/**
 * @brief Structure representing fleet protocol status
 */
class Status {
public:
	[[nodiscard]] double getLongitude() const;

	void setLongitude(double longitude);

	[[nodiscard]] double getLatitude() const;

	void setLatitude(double latitude);

	[[nodiscard]] double getSpeed() const;

	void setSpeed(double speed);

	[[nodiscard]] EAutonomyState getState() const;

	void setState(EAutonomyState state);

	[[nodiscard]] const osm::Route::Station &getNextStop() const;

	void setNextStop(const osm::Route::Station &nextStop);

	friend std::ostream &operator<<(std::ostream &stream, const Status &matrix);

private:
	/// Current longitude position of the vehicle
	double longitude_ { 0.0 };
	/// Current latitude position of the vehicle
	double latitude_ { 0.0 };
	/// Current speed of the vehicle
	double speed_ { 0.0 };
	/// Current state of the autonomy
	EAutonomyState state_ { EAutonomyState::E_INVALID };
	/// Next stop on the route
	osm::Route::Station nextStop_ {};
};
}