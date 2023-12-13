#pragma once

#include <bringauto/osm/Route.hpp>
#include <bringauto/common_utils/CommonUtils.hpp>

#include <vector>
#include <string>



namespace bringauto::communication {
/**
 * @brief Enum representing command action
 */
enum class EAutonomyAction {
	E_BEGIN = 0,
	E_INVALID = E_BEGIN,
	/// Car should take no action based on command
	E_NO_ACTION,
	/// Car should stop immediately
	E_STOP,
	/// Car can start moving if mission is set
	E_START,
	E_END
};

/**
 * @brief Structure representing fleet protocol command
 */
class Command {
public:
	[[nodiscard]] const std::vector<osm::Route::Station> &getMission() const;

	void setMission(const std::vector<osm::Route::Station> &mission);

	[[nodiscard]] EAutonomyAction getAction() const;

	void setAction(EAutonomyAction action);

	[[nodiscard]] const std::string &getRoute() const;

	void setRoute(const std::string &route);

	[[nodiscard]] const std::vector<osm::Route::Station> &getRouteStations() const;

	void setRouteStations(const std::vector<osm::Route::Station> &routeStations);

	inline bool operator==(const Command &toCompare) const {
		if(action_ != toCompare.getAction()) {
			return false;
		}
		if(!common_utils::CommonUtils::compareMissions(getMission(), toCompare.getMission())) {
			return false;
		}
		if(route_ != toCompare.getRoute()) {
			return false;
		}
		return true;
	}

	inline bool operator!=(const Command &toCompare) const {
		return !(*this == toCompare);
	}

	friend std::ostream &operator<<(std::ostream &stream, const Command &matrix);
private:
	/// Mission, vector contains list of stops that car should go to
	std::vector<osm::Route::Station> mission_;
	/// Action autonomy should take
	EAutonomyAction action_ { EAutonomyAction::E_INVALID };
	/// Name of the route
	std::string route_;
	/// Stations on the route.
	std::vector<osm::Route::Station> routeStations_;
};
}