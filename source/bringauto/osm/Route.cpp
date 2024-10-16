#include <bringauto/osm/Route.hpp>

#include <bringauto/settings/LoggerId.hpp>



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
		settings::Logger::logWarning(
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
	settings::Logger::logInfo("End of route has been reached, continuing another lap");
	if(!routeIsCircular_) {
		settings::Logger::logInfo("Route is not circular, reversing.");
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

	routeIsCircular_ = (distanceBetweenStartAndEnd < CIRCULAR_ROUTE_THRESHOLD_M);
	propagateSpeed();
}

std::shared_ptr<Point> Route::getPosition() {
	return *positionIt;
}

bool Route::areStopsPresent(const std::vector<osm::Route::Station> &stops) {
	for(const auto &stop: stops) {
		auto pointIt = std::find_if(stops_.begin(), stops_.end(),
									[stop](const auto &point) { return stop.name == point->getName(); });
		if(pointIt == stops_.end()) {
			settings::Logger::logError("Unknown stop: " + stop.name);
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
		if(distanceBetweenRoutes > ROUTES_DISTANCE_THRESHOLD_M) {
			settings::Logger::logWarning("Distance between part of routes is higher than threshold {}",
										ROUTES_DISTANCE_THRESHOLD_M);
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

std::shared_ptr<Point> Route::getClosestPoint(const Point &pointToFind) const {
	if(points_.empty()) {
		return nullptr;
	}

	auto haversineDistance = [&pointToFind](const std::shared_ptr<Point> &point) {
		return osmium::geom::haversine::distance(
				osmium::geom::Coordinates {point->getLatitude(), point->getLongitude()},
				osmium::geom::Coordinates {pointToFind.getLatitude(), pointToFind.getLongitude()});
	};

	auto closestPointIter = std::min_element(points_.begin(), points_.end(),
	                                        [&haversineDistance](const std::shared_ptr<Point> &a, const std::shared_ptr<Point> &b) {
                                            	return haversineDistance(a) < haversineDistance(b);
	                                        });

	if(closestPointIter == points_.end()) {
		return nullptr;
	}

	double closestDistance = haversineDistance(*closestPointIter);

	if(closestDistance < POINT_TOLERANCE_M) {
		return *closestPointIter;
	}

	return nullptr;
}

void Route::setPositionAndDirection(const Point &newPosition, const std::string &nextStopName) {
	positionIt = points_.begin();
	for(auto it = points_.begin(); it != points_.end(); it++) { //todo refactor
		auto distance = osmium::geom::haversine::distance(
				osmium::geom::Coordinates { it->get()->getLatitude(), it->get()->getLongitude() },
				osmium::geom::Coordinates { newPosition.getLatitude(), newPosition.getLongitude() });
		if(distance < DISTANCE_TOLERANCE_M) {
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
								osmium::geom::Coordinates { newPosition.getLatitude(),
															newPosition.getLongitude() });
						if(newDistance < DISTANCE_TOLERANCE_M) {
							break;
						}
					}
				}
			}
			positionIt = it;
			return;
		}
	}
	settings::Logger::logWarning("Position not found on route, setting to first point of route");
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
	for(auto commandStation : commandStations) {
		bool stationFound = false;
		for(const auto &stop: stops_) {
			if(stop->getName() == commandStation.name) {
				stationFound = true;
				auto stopDistance = osmium::geom::haversine::distance(
						osmium::geom::Coordinates { stop->getLatitude(), stop->getLongitude() },
						osmium::geom::Coordinates { commandStation.latitude, commandStation.longitude });
				if(stopDistance > POINT_TOLERANCE_M) {
					settings::Logger::logWarning(
							"Station {} is on different location. Station position in command: lat = {}, long = {}, position on route: lat = {}, long = {}",
							commandStation.name, commandStation.latitude, commandStation.longitude, stop->getLatitude(),
							stop->getLongitude());
				}
			}
		}
		if(!stationFound) {
			settings::Logger::logError("Station {} sent in command is not on the route", commandStation.name);
		}
	}
}
}