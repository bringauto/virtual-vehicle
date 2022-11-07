#include <bringauto/osm/Route.hpp>

#include <bringauto/logging/Logger.hpp>



namespace bringauto::osm {
void Route::setRouteName(const std::optional<std::string> &routeName) {
	routeName_ = routeName;
}

std::string Route::getRouteName() {
	return routeName_.has_value() ? routeName_.value() : "";
}

void Route::propagateSpeed() {
	double speed;
	if(points_.front()->getSpeedInMetersPerSecond() > 0) {
		speed = points_.front()->getSpeedInMetersPerSecond();
	} else {
		speed = 5;
		logging::Logger::logWarning(
				"First point of route {} does not contain speed, defaulting to 5m/s", getRouteName());
	}

	for(const auto &point: points_) {
		if(point->getSpeedInMetersPerSecond() > 0) {
			speed = point->getSpeedInMetersPerSecond();
		} else {
			point->setSpeed(speed);
		}
	}
}

void Route::setNextPosition() {
	positionIt++;
	if(positionIt != points_.end()) {
		return;
	}
	logging::Logger::logInfo("End of route has been reached, continuing another lap");
	if(!routeIsCircular_) {
		logging::Logger::logInfo("Route is not circular, reversing.");
		std::reverse(points_.begin(), points_.end());
		positionIt = points_.begin();
		positionIt++;
	} else {
		positionIt = points_.begin();
	}

}

void Route::prepareRoute() {
	if(points_.empty()) {
		throw std::runtime_error("Route " + getRouteName() + " has no points.");
	}
	positionIt = points_.begin();

	double distanceBetweenStartAndEnd = osmium::geom::haversine::distance(
			osmium::geom::Coordinates { points_.front()->getLatitude(), points_.front()->getLongitude() },
			osmium::geom::Coordinates { points_.back()->getLatitude(), points_.back()->getLongitude() });

	routeIsCircular_ = (distanceBetweenStartAndEnd < roundRouteLimitInMeters);
	propagateSpeed();
}

std::shared_ptr<Point> Route::getPosition() {
	return *positionIt;
}

bool Route::areStopsPresent(const std::vector<std::string> &stopNames) {
	for(const auto &stopName: stopNames) {
		auto pointIt = std::find_if(stops_.begin(), stops_.end(),
									[stopName](const auto &point) { return stopName == point->getName(); });
		if(pointIt == stops_.end()) {
			logging::Logger::logError("Unknown stop: " + stopName);
			return false;
		}
	}
	return true;
}

void Route::appendWay(const std::shared_ptr<Way> &way) {
	auto points = way->getPoints();
	auto stops = way->getStops();
	if(!points_.empty()) {
		double distanceBetweenRoutes = osmium::geom::haversine::distance(
				osmium::geom::Coordinates { points_.back()->getLatitude(), points_.back()->getLongitude() },
				osmium::geom::Coordinates { points.front()->getLatitude(), points.front()->getLongitude() });
		if(distanceBetweenRoutes > routesDistanceThresholdInMeters_) {
			logging::Logger::logWarning("Distance between part of routes is higher than threshold {}",
										roundRouteLimitInMeters);
		}
	}
	points_.insert(points_.end(), points.begin(), points.end());
	stops_.insert(stops_.end(), stops.begin(), stops.end());
}

void Route::speedOverride(unsigned int speed) {
	for(auto const &point: points_) {
		point->setSpeed(speed);
	}
}

bool Route::isPointPresent(const Point &pointToFind) {
	return std::any_of(points_.begin(), points_.end(), [pointToFind](const std::shared_ptr<Point> &point) {
		return osmium::geom::haversine::distance(
				osmium::geom::Coordinates { point->getLatitude(), point->getLongitude() },
				osmium::geom::Coordinates { pointToFind.getLatitude(), pointToFind.getLongitude() }) < pointToleranceInMeters_;
	});
}

void Route::setPositionAndDirection(const Point &actualPosition, const std::string &nextStopName) {
	positionIt = points_.begin();
	for(auto it = points_.begin(); it != points_.end(); it++) { //todo refactor
		auto distance = osmium::geom::haversine::distance(
				osmium::geom::Coordinates { it->get()->getLatitude(), it->get()->getLongitude() },
				osmium::geom::Coordinates { actualPosition.getLatitude(), actualPosition.getLongitude() });
		if(distance < distanceToleranceInMeters_) {
			if(!routeIsCircular_) { // Circular routes must not be reversed
				bool reverse = true;
				for(auto it2 = it; it2 != points_.end(); it2++) {
					if(nextStopName == it2->get()->getName()) { // next stop is found in current direction
						reverse = false;
						break;
					}
				}
				if(reverse) {
					std::reverse(points_.begin(), points_.end());
					for(it = points_.begin(); it != points_.end(); it++) {
						auto newDistance = osmium::geom::haversine::distance(
								osmium::geom::Coordinates { it->get()->getLatitude(), it->get()->getLongitude() },
								osmium::geom::Coordinates { actualPosition.getLatitude(),
															actualPosition.getLongitude() });
						if(newDistance < distanceToleranceInMeters_) {
							break;
						}
					}
				}
			}
			positionIt = it;
			return;
		}
	}
}

std::vector<std::shared_ptr<Point>> Route::getStops() {
	std::vector<std::shared_ptr<Point>> stops;
	for(const auto &point: points_) {
		if(point->isStop()) {
			stops.push_back(point);
		}
	}
	return stops;
}

void Route::compareStations(std::vector<Station> commandStations) {
	if(commandStations.size() != stops_.size()) {
		logging::Logger::logError("There isn't the same number of stops in the command ({}) as on the route ({})", commandStations.size(), stops_.size());
		return;
	}
	for(auto commandStation : commandStations) {
		bool stationFound = false;
		for(const auto &stop: stops_) {
			if(stop->getName() == commandStation.name) {
				stationFound = true;
				auto stopDistance = osmium::geom::haversine::distance(
						osmium::geom::Coordinates { stop->getLatitude(), stop->getLongitude() },
						osmium::geom::Coordinates { commandStation.latitude, commandStation.longitude });
				if(stopDistance > pointToleranceInMeters_) {
					logging::Logger::logWarning(
							"Station {} is on different location. Station position in command: lat = {}, long = {}, position on route: lat = {}, long = {}",
							commandStation.name, commandStation.latitude, commandStation.longitude, stop->getLatitude(),
							stop->getLongitude());
				}
			}
		}
		if(!stationFound) {
			logging::Logger::logError("Station {} sent in command is not on the route", commandStation.name);
		}
	}
}
}